<?php
/**
 * @file
 * Contains GoogleAnalyticsCounterQueueQueue.
 */

/**
 * Class GoogleAnalyticsCounterQueueQueue.
 */
class GoogleAnalyticsCounterQueueQueue implements DrupalReliableQueueInterface, Iterator, Countable {
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
   * A boolean indicating whether this queue is a priority queue.
   *
   * @access public
   *
   * @return bool
   *   Returns TRUE if this queue is a priority based on `weight` property,
   *   otherwise returns FALSE.
   */
  public function isPriority() {
    return FALSE;
  }

  /**
   * Returns the name of the database table containing the queue items.
   *
   * @access public
   *
   * @return string
   *   Returns the database table name.
   */
  public function getTable() {
    return 'google_analytics_counter_queue_queue';
  }

  /**
   * Add a queue item and store it directly to the queue.
   *
   * @param $data
   *   Arbitrary data to be associated with the new task in the queue.
   *
   * @return boolean
   *   TRUE if the item was successfully created and was (best effort) added
   *   to the queue, otherwise FALSE. We don't guarantee the item was
   *   committed to disk etc, but as far as we know, the item is now in the
   *   queue.
   */
  public function createItem($data, $weight = NULL) {
    $item = new GoogleAnalyticsCounterQueueQueueItem(NULL, $data, 0, $weight, $this);
    $query = db_insert($this->getTable())
      ->fields(array(
        'path' => $item,
        'created' => time(),
        'expire' => 0,
      ));
    return (bool) $query->execute();
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
  public function numberOfItems()
  {
    return (int) db_select($this->getTable(), 'q')->fields('q', array('item_id'))->execute()->rowCount();
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
   * @return
   *   On success we return an item object. If the queue is unable to claim an
   *   item it returns false. This implies a best effort to retrieve an item
   *   and either the queue is empty or there is some other non-recoverable
   *   problem.
   */
  public function claimItem($lease_time = 3600) {
    while (TRUE) {
      $item = db_select($this->getTable(), 'q')
        ->fields('q')
        ->condition('q.expire', 0)
        ->orderBy('q.weight', 'ASC')
        ->orderBy('q.created', 'ASC')
        ->execute()
        ->fetchObject();
      if ($item) {
        $queue_item = GoogleAnalyticsCounterQueueQueueItem::factory($item, $this);
        $update = db_update($this->getTable())->fields(array(
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
   * Delete a finished item from the queue.
   *
   * @param GoogleAnalyticsCounterQueueQueueItem $item
   *   The item returned by DrupalQueueInterface::claimItem().
   * @param bool $force_delete
   *   Forces the deletion of a queue item.
   */
  public function deleteItem($item, $force_delete = FALSE) {
    if (!$item instanceof GoogleAnalyticsCounterQueueQueueItem) {
      throw new InvalidArgumentException();
    }

    if (!$force_delete && (!$item->getProcessedState() && $item->getAttempts() < google_analytics_counter_queue_google_analytics_queue_item_threshold())) {
      watchdog(WATCHDOG_DEBUG, t('Attempt to delete queue item @queue; attempts to process is less than threshold.', array(
        '@queue' => $item->getIdentifier(),
      )));
      $item->save();
      return;
    }

    if (!$force_delete && ($item->getTotalRecords() && $item->getRemainingRecords())) {
      watchdog(WATCHDOG_DEBUG, t('Attempt to delete a queue item which has records remaining; saving records instead.'));
      $item->save();
      return;
    }

    if ($item_id = $item->getIdentifier()) {
      if (!empty($this->items) && isset($this->item_keys[$item_id])) {
        if (isset($this->items[$this->item_keys[$item_id]])) {
          unset($this->items[$this->item_keys[$item_id]]);
        }
        unset($this->item_keys[$item_id]);
      }

      db_delete($this->getTable())
        ->condition('item_id', $item_id)
        ->execute();
      // TODO: Clear any cache items that may be needed.
    }
  }

  /**
   * Release an item that the worker could not process, so another
   * worker can come in and process it before the timeout expires.
   *
   * @param $item
   * @return boolean
   */
  public function releaseItem($item) {
    // TODO: Implement releaseItem() method.
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
    db_delete($this->getTable())->execute();
  }

  protected $items = NULL;
  protected $item_keys = NULL;
  protected $position = 0;

  protected function initializeIterator() {
    if (NULL === $this->items) {
    $this->items = array();
    $this->item_keys = array();
    $this->position = 0;

      $query = db_select($this->getTable(), 'q')
        ->fields('q')
        ->orderBy('q.weight')
        ->orderBy('q.item_id')
        ->execute();
      while ($row = $query->fetchObject()) {
        /* if (!empty($row->data) && is_string($row->data)) {
          $row->data = @unserialize($row->data);
        } */

        $item = GoogleAnalyticsCounterQueueQueueItem::factory($row, $this);
        $this->items[$item->getIdentifier()] = $item;
        $this->item_keys[] = $item->getIdentifier();
      }
    }
  }

  /**
   * (PHP 5 &gt;= 5.0.0)<br/>
   * Return the current element
   * @link http://php.net/manual/en/iterator.current.php
   * @return mixed Can return any type.
   */
  public function current() {
    $this->initializeIterator();
    return $this->items[$this->item_keys[$this->position]];
  }

  /**
   * (PHP 5 &gt;= 5.0.0)<br/>
   * Move forward to next element
   * @link http://php.net/manual/en/iterator.next.php
   * @return void Any returned value is ignored.
   */
  public function next() {
    $this->initializeIterator();
    $this->position++;
  }

  /**
   * (PHP 5 &gt;= 5.0.0)<br/>
   * Return the key of the current element
   * @link http://php.net/manual/en/iterator.key.php
   * @return mixed scalar on success, or null on failure.
   */
  public function key() {
    $this->initializeIterator();
    return $this->item_keys[$this->position];
  }

  /**
   * (PHP 5 &gt;= 5.0.0)<br/>
   * Checks if current position is valid
   * @link http://php.net/manual/en/iterator.valid.php
   * @return boolean The return value will be casted to boolean and then evaluated.
   * Returns true on success or false on failure.
   */
  public function valid() {
    $this->initializeIterator();
    return isset($this->item_keys[$this->position]) && isset($this->items[$this->item_keys[$this->position]]);
  }

  /**
   * (PHP 5 &gt;= 5.0.0)<br/>
   * Rewind the Iterator to the first element
   * @link http://php.net/manual/en/iterator.rewind.php
   * @return void Any returned value is ignored.
   */
  public function rewind() {
    $this->initializeIterator();
    $this->position = 0;
  }

  /**
   *
   * @link http://php.net/manual/en/countable.count.php
   */
  public function count() {
    return $this->numberOfItems();
  }
}
