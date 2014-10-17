<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueQueueItem.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueQueueItem
 */
class GoogleAnalyticsCounterQueueQueueItem implements Serializable {
  /**
   * @access public
   *
   * @param stdClass $item
   *   An stdClass instance which contains the required properties for creation
   *   and instantiation of a new GoogleAnalyticsCounterQueueQueueItem instance.
   *
   * @param GoogleAnalyticsCounterQueueQueue $queue
   *   The queue which this item belongs too.
   *
   * @return GoogleAnalyticsCounterQueueQueueItem
   *   Returns a newly created instantiated instance using the specified data.
   */
  public static function factory(stdClass $item, GoogleAnalyticsCounterQueueQueue $queue = NULL) {
    $instance = new self($item->item_id, $item->path, $item->created, $item->expire, $item->weight, $item->total_records, $item->remaining_records, $queue);
    return $instance;
  }

  /**
   * Loads a queue item from the database and creates a new instance.
   *
   * @access public
   *
   * @param int $item_id
   *   The queue item's identifier.
   *
   * @return GoogleAnalyticsCounterQueueQueueItem
   *   Returns the queue item instance if exists otherwise returns FALSE.
   */
  public static function load($item_id) {
    $query = self::loadQueryBuild(google_analytics_counter_queue_priority_queue(), $item_id);
    $query = $query->execute();
    if ($record = $query->fetchObject()) {
      $instance = self::factory($record, google_analytics_counter_queue_priority_queue());
    }
    else {
      $query = self::loadQueryBuild(google_analytics_counter_queue_queue(), $item_id);
      $query = $query->execute();
      if ($record = $query->fetchObject()) {
        $instance = self::factory($record, google_analytics_counter_queue_queue());
      }
      else {
        return FALSE;
      }
    }
    return $instance;
  }

  /**
   * Helper method for initializing the SelectQuery for load() method.
   *
   * @access protected
   *
   * @param GoogleAnalyticsCounterQueueQueue $queue
   *   The queue object to build the query for.
   *
   * @param int $item_id
   *   The queue item identifier to load.
   *
   * @return SelectQuery
   *   The SelectQuery instance for the `$queue`'s table and fields.
   */
  protected static function loadQueryBuild(GoogleAnalyticsCounterQueueQueue $queue, $item_id) {
    $query = db_select($queue->getTable(), 'q')
      ->fields('q')
      ->condition('q.item_id', $item_id);
    return $query;
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
   * @param GoogleAnalyticsCounterQueueQueue $queue
   *   The queue which this queue item belongs to.
   */
  public function __construct($id = NULL, $path = '', $created = 0, $expires = 0, $weight = NULL, $total_records = NULL, $remaining_records = NULL, GoogleAnalyticsCounterQueueQueue $queue = NULL) {
    $this->id = $id;
    $this->path = $path;
    $this->created = $created;
    $this->expires = $expires;

    if ($queue) {
      $this->setQueue($queue);
      if (NULL === $weight) {
        $this->weight = google_analytics_counter_queue_queue_item_next_weight('bottom', $queue);
      }
      else {
        $this->weight = $weight;
      }
    }
    else {
      $this->weight = (int) $weight;
    }

    $this->total_records = $total_records;
    $this->remaining_records = $remaining_records;
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

  public function setQueue(GoogleAnalyticsCounterQueueQueue $queue) {
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

  public function save() {
    if (!$this->changed) {
      return;
    }

    $transaction = db_transaction();
    try {
      if (empty($this->id)) {
        $query = db_insert($this->queue->getTable());
        $op = 'insert';
      }
      else {
        $query = db_update($this->queue->getTable())
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
      ))
      ->execute();

      if ('insert' == $op) {
        $this->id = $result;
      }

      module_invoke_all('google_analytics_counter_queue_queue_item_' . $op, $this);

      db_ignore_slave();
    }
    catch (Exception $exception) {
      watchdog_exception('google_analytics_counter_queue', $exception);
      $transaction->rollback();
      throw $exception;
    }
  }

  /**
   * Deletes this individual queue item from the queue.
   *
   * @access public
   */
  public function delete() {
    if (!empty($this->queue)) {
      $this->queue->deleteItem($this);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function serialize() {
    return serialize(array(
      'created' => $this->created,
      'expires' => $this->expires,
      'weight' => $this->weight,
      'path' => $this->path,
    ));
  }

  /**
   * {@inheritdoc}
   */
  public function unserialize($serialized_data) {
    $serialized_data = unserialize($serialized_data);

    $this->created = isset($serialized_data['created']) ? $serialized_data['created'] : 0;
    $this->expires = isset($serialized_data['expires']) ? $serialized_data['expires'] : 0;
    $this->weight = isset($serialized_data['weight']) ? $serialized_data['weight'] : NULL;
    $this->path = isset($serialized_data['path']) ? $serialized_data['path'] : '';
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