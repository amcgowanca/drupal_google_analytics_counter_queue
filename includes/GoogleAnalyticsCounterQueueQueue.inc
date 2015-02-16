<?php
/**
 * @file
 * Contains class GoogleAnalyticsCounterQueueQueue.
 */

/**
 * Class GoogleAnalyticsCounterQueueQueue.
 */
class GoogleAnalyticsCounterQueueQueue implements DrupalReliableQueueInterface, Countable {
  /**
   * The name of this queue; required by Drupal API.
   *
   * @var string
   */
  protected $name;

  /**
   * Creates new instance of GoogleAnalyticsCounterQueueQueue.
   *
   * @access public
   *
   * @param string $name
   *   The machine name of this queue.
   */
  public function __construct($name = 'google_analytics_counter_queue_queue') {
    $this->name = $name;
  }

  /**
   * @return string
   */
  public function getName() {
    return $this->name;
  }

  /**
   * Do not use.
   *
   * @access public
   *
   * @param $data
   *   Arbitrary data to be associated with the new task in the queue.
   *
   * @return boolean
   *   TRUE if the item was successfully created and was (best effort) added
   *   to the queue, otherwise FALSE. We don't guarantee the item was
   *   committed to disk etc, but as far as we know, the item is now in the
   *   queue.
   *
   * @throws Exception
   *   Always thrown.
   */
  public function createItem($data) {
    throw new Exception(t('Creation of queue items should be achieved by instantiation of the GoogleAnalyticsCounterQueueQueueItem class.'));
  }

  /**
   * Retrieve the number of items in the queue.
   *
   * This is intended to provide a "best guess" count of the number of items in
   * the queue. Depending on the implementation and the setup, the accuracy of
   * the results of this function may vary.
   *
   * e.g. On a busy system with a large number of consumers and items, the
   * result might only be valid for a fraction of a second and not provide an
   * accurate representation.
   *
   * @return
   *   An integer estimate of the number of items in the queue.
   */
  public function numberOfItems() {
    return (int) db_select('google_analytics_counter_queue_queue', 'q')->fields('q', array('item_id'))->execute()->rowCount();
  }

  /**
   * Claim an item in the queue for processing.
   *
   * @param $lease_time
   *   How long the processing is expected to take in seconds, defaults to an
   *   hour. After this lease expires, the item will be reset and another
   *   consumer can claim the item. For idempotent tasks (which can be run
   *   multiple times without side effects), shorter lease times would result
   *   in lower latency in case a consumer fails. For tasks that should not be
   *   run more than once (non-idempotent), a larger lease time will make it
   *   more rare for a given task to run multiple times in cases of failure,
   *   at the cost of higher latency.
   * @return mixed
   *   On success we return an item object. If the queue is unable to claim an
   *   item it returns false. This implies a best effort to retrieve an item
   *   and either the queue is empty or there is some other non-recoverable
   *   problem.
   */
  public function claimItem($lease_time = 3600) {
    while (TRUE) {
      $item = db_select('google_analytics_counter_queue_queue', 'q')
        ->fields('q')
        ->condition('q.expire', 0)
        ->orderBy('q.weight', 'ASC')
        ->orderBy('q.created', 'ASC')
        ->execute()
        ->fetchObject();
      if ($item) {
        $queue_item = GoogleAnalyticsCounterQueueQueueItem::factory($item);
        $update = db_update('google_analytics_counter_queue_queue')->fields(array(
          'expire' => time() + $lease_time,
        ))->condition('item_id', $queue_item->getIdentifier())
          ->condition('expire', 0);
        if ($update->execute()) {
          return $queue_item;
        }
      }
      else {
        // No items currently available to claim.
        return FALSE;
      }
    }
  }

  /**
   * "Deletes" a finished item from the queue.
   *
   * @access public
   *
   * @param object $item
   *   The queue item to process.
   * @param bool $force_delete
   *   Forces the deletion of a queue item. Default value is FALSE.
   *
   * @throws InvalidArgumentException
   *   Thrown if $item is not of type GoogleAnalyticsCounterQueueQueueItem.
   */
  public function deleteItem($item, $force_delete = FALSE) {
    if (!$item instanceof GoogleAnalyticsCounterQueueQueueItem) {
      throw new InvalidArgumentException(t('Argument must be of type GoogleAnalyticsCounterQueueQueueItem.'));
    }

    /* if (!$force_delete && (!$item->getProcessedState() && $item->getAttempts() < google_analytics_counter_queue_google_analytics_queue_item_threshold())) {
      google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Attempt to delete queue item @queue; attempts to process is less than threshold.', array(
        '@queue' => $item->getIdentifier(),
      ));
      $item->save();
      return;
    }

    if (!$force_delete && ($item->getTotalRecords() && $item->getRemainingRecords())) {
      google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Attempt to delete a queue item which has records remaining; saving records instead.');
      $item->save();
      return;
    } */

    if ($item_id = $item->getIdentifier()) {
      db_delete('google_analytics_counter_queue_queue')
        ->condition('item_id', $item_id)
        ->execute();
    }

    google_analytics_counter_queue_module_include();
    module_invoke_all('google_analytics_counter_queue_queue_item_delete', $item);
  }

  /**
   * Release an item that the worker could not process, so another
   * worker can come in and process it before the timeout expires.
   *
   * @access public
   *
   * @param object $item
   *   The queue item to release.
   *
   * @return bool
   *   Returns TRUE, always.
   *
   * @throws InvalidArgumentException
   *   Thrown if $item is not of type GoogleAnalyticsCounterQueueQueueItem.
   */
  public function releaseItem($item) {
    if (!$item instanceof GoogleAnalyticsCounterQueueQueueItem) {
      throw new InvalidArgumentException(t('Argument must be of type GoogleAnalyticsCounterQueueQueueItem.'));
    }
    $item->setExpires(0)->save();
    return TRUE;
  }

  /**
   * Create a queue.
   *
   * Called during installation and should be used to perform any necessary
   * initialization operations. This should not be confused with the
   * constructor for these objects, which is called every time an object is
   * instantiated to operate on a queue. This operation is only needed the
   * first time a given queue is going to be initialized (for example, to make
   * a new database table or directory to hold tasks for the queue -- it
   * depends on the queue implementation if this is necessary at all).
   */
  public function createQueue() {
    // This implementation is not needed as the queue is already created during
    // installation of the Google Analytics Counter Queue module.
  }

  /**
   * Delete a queue and every item in the queue.
   *
   * @access public
   */
  public function deleteQueue() {
    // Delete all items from within the queue; however, we will not delete the
    // queue table itself as we will need it again.
    db_delete('google_analytics_counter_queue_queue')->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function count() {
    return $this->numberOfItems();
  }
}
