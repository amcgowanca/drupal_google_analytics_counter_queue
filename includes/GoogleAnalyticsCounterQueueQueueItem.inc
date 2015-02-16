<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueQueueItem.inc
 */

/**
 * Represents a single queue item.
 */
class GoogleAnalyticsCounterQueueQueueItem {
  /**
   * Creates a new instance from a queue item stdClass object.
   *
   * @access public
   *
   * @param stdClass $item
   *   An stdClass instance which contains the required properties for creation
   *   and instantiation of a new GoogleAnalyticsCounterQueueQueueItem instance.\
   *
   * @return GoogleAnalyticsCounterQueueQueueItem
   *   Returns a newly created instantiated instance using the specified data.
   */
  public static function factory(stdClass $item) {
    $instance = new self($item->item_id, $item->path, $item->created, $item->expire, $item->weight, $item->total_records, $item->remaining_records, $item->attempts);
    return $instance;
  }

  /**
   * The unique identifier.
   *
   * @var integer
   */
  private $id = NULL;

  /**
   * The path to process.
   *
   * @var string
   */
  private $path = '';

  /**
   * The timestamp of when this queue item was created.
   *
   * @var int
   */
  private $created = 0;

  /**
   * The timestamp of when this queue item expires.
   *
   * @var int
   */
  private $expires = 0;

  /**
   * The weight of this queue item within the queue.
   *
   * The weight of a queue item is primarily used for determining the ordering
   * in which queue items are retrieved and processed within a single queue.
   *
   * @var int
   */
  private $weight = NULL;

  /**
   * A boolean used to indicate if this queue item's state has changed.
   *
   * @var bool
   */
  private $changed = FALSE;

  /**
   * The queue object in which this queue item belongs to.
   *
   * @var GoogleAnalyticsCounterQueueQueue
   */
  private $queue = NULL;

  /**
   * The number of records remaining to be processed.
   *
   * @var int
   */
  private $remaining_records = NULL;

  /**
   * The total number of records (possible matches) when processed on the API.
   *
   * @var int
   */
  private $total_records = NULL;

  /**
   * The number of attempts to process this queue item.
   *
   * @var int
   */
  private $attempts = 0;

  /**
   * @var bool
   */
  private $processed = FALSE;

  /**
   * Creates a new instance of GoogleAnalyticsCounterQueueQueueItem.
   *
   * @access public
   *
   * @param int $id
   *   The queue item's identifier.
   * @param string $path
   *   The queue item's path.
   * @param int $created
   *   The queue item's created timestamp.
   * @param int $expires
   *   The queue item's expiration timestamp.
   * @param int $weight
   *   The weight of this queue item.
   * @param int $total_records
   *   The total records this queue item has for processing.
   * @param int $remaining_records
   *   The remaining number of records to process.
   * @param int $attempts
   *   The number of attempts to process this queue item.
   */
  public function __construct($id = NULL, $path = '', $created = 0, $expires = 0, $weight = NULL, $total_records = NULL, $remaining_records = NULL, $attempts = 0) {
    $this->id = $id;
    $this->path = $path;
    $this->created = $created;
    $this->expires = $expires;
    $this->attempts = 0;
    $this->weight = NULL === $weight ? google_analytics_counter_queue_queue_item_next_weight('bottom') : $weight;
    $this->total_records = $total_records;
    $this->remaining_records = $remaining_records;
    $this->setQueue(google_analytics_counter_queue_queue());
  }

  /**
   * Returns the unique identifier.
   *
   * @access public
   *
   * @return int
   *   The unique identifier.
   */
  public function getIdentifier() {
    return $this->id;
  }

  final protected function setQueue(GoogleAnalyticsCounterQueueQueue $queue) {
    $this->queue = $queue;
    $this->changed = TRUE;
    return $this;
  }

  public function getQueue() {
    return $this->queue;
  }

  public function setCreated($timestamp) {
    $this->created = $timestamp;
    $this->changed = TRUE;
    return $this;
  }

  public function getCreated() {
    return $this->created;
  }

  public function setExpires($timestamp) {
    $this->expires = $timestamp;
    $this->changed = TRUE;
    return $this;
  }

  public function getExpires() {
    return $this->expires;
  }

  public function setWeight($weight) {
    $this->weight = $weight;
    $this->changed = TRUE;
    return $this;
  }

  public function getWeight() {
    return $this->weight;
  }

  public function getPath() {
    return $this->path;
  }

  public function setPath($path) {
    $this->path = $path;
    $this->changed = TRUE;
    return $this;
  }

  public function setTotalRecords($value) {
    $this->total_records = $value;
    $this->changed = TRUE;
    return $this;
  }

  public function getTotalRecords() {
    return $this->total_records;
  }

  public function setRemainingRecords($value) {
    $this->remaining_records = 0 > $value ? 0 : $value;
    $this->changed = TRUE;
    return $this;
  }

  public function getRemainingRecords() {
    return $this->remaining_records;
  }

  public function setAttempts($value) {
    $this->attempts = $value;
    $this->changed = TRUE;
    return $this;
  }

  public function getAttempts() {
    return $this->attempts;
  }

  public function setProcessedState($value) {
    $this->processed = $value;
    return $this;
  }

  public function getProcessedState() {
    return $this->processed;
  }

  public function save() {
    if (!$this->changed) {
      return;
    }

    $transaction = db_transaction();
    try {
      if (empty($this->id)) {
        $query = db_insert('google_analytics_counter_queue_queue');
        $op = 'insert';
      }
      else {
        $query = db_update('google_analytics_counter_queue_queue')
          ->condition('item_id', $this->id);
        $op = 'update';
      }

      $result = $query->fields(array(
        'created' => $this->created,
        'expire' => $this->expires,
        'path' => $this->path,
        'weight' => NULL === $this->weight ? 0 : $this->weight,
        'total_records' => $this->total_records,
        'remaining_records' => $this->remaining_records,
        'attempts' => $this->attempts,
      ))
      ->execute();

      if ('insert' == $op) {
        $this->id = $result;
      }

      google_analytics_counter_queue_module_include();
      module_invoke_all('google_analytics_counter_queue_queue_item_' . $op, $this);

      db_ignore_slave();
    }
    catch (Exception $exception) {
      google_analytics_counter_queue_log_exception($exception);
      $transaction->rollback();
      throw $exception;
    }
  }

  /**
   * Deletes this individual queue item from the queue.
   *
   * @access public
   *
   * @param bool $force
   *   A boolean indicating whether this item should be force deleted or not.
   */
  public function delete($force = FALSE) {
    if (!empty($this->queue)) {
      $this->queue->deleteItem($this, $force);
    }
  }

  /**
   * Magical method for the intent of satisfying Drupal.
   *
   * Only purpose is to return this instance when the 'data' property is
   * requested. Due to the Google Analytics Counter Queue queue item
   * implementation being radically different than that of Drupal's core queue
   * items, we do make use of any "data" property.
   *
   * @access public
   *
   * @param string $property
   *   The only valid value is `data`.
   *
   * @return GoogleAnalyticsCounterQueueQueueItem
   *   Returns this queue item instance when `$property` is `data`.
   *
   * @throws InvalidArgumentException
   *   Thrown if the value of `$property` is not `data`.
   */
  public function __get($property) {
    if ('data' === $property) {
      return $this;
    }
    throw new InvalidArgumentException(t('The specified property %property does not exist. The use of the __get() should be restricted to only the <em>data</em> property.', array(
      '%property' => $property,
    )));
  }
}