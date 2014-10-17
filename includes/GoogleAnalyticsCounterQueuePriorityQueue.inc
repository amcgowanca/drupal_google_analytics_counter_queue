<?php
/**
 * @file
 * GoogleAnalyticsCounterQueuePriorityQueue.inc
 */

/**
 *
 */
class GoogleAnalyticsCounterQueuePriorityQueue extends GoogleAnalyticsCounterQueueQueue {
  /**
   *
   */
  public function __construct() {
    parent::__construct('Google Analytics Counter Queue');
  }

  /**
   * {@inheritdoc}
   */
  public function getTable() {
    return 'google_analytics_counter_queue_priority_queue';
  }

  /**
   * {@inheritdoc}
   */
  public function isPriority() {
    return TRUE;
  }
}
