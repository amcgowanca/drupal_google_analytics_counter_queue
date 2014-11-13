<?php
/**
 * @file
 */

/**
 * Implements hook_google_analytics_counter_queue_queue_item_processed().
 */
function statistics_google_analytics_counter_queue_queue_item_processed(GoogleAnalyticsCounterQueueQueueItem $queue_item, $statistics) {
  if (NULL !== $statistics && google_analytics_counter_queue_statistics_allow_update()) {
    // Attempt to lookup the queue item's path source.
    $normalized_path = drupal_lookup_path('source', $queue_item->getPath());
    // If FALSE is returned from `drupal_lookup_path`, assume that the queue
    // item path is already normalized (we hope).
    $normalized_path = FALSE === $normalized_path ? $queue_item->getPath() : $normalized_path;
    if (preg_match('|^node/([0-9]+)?|', $normalized_path, $matches)) {
      $node_id = isset($matches[1]) ? $matches[1] : NULL;
      if (is_numeric($node_id)) {
        $node_id = intval($node_id);
        db_merge('node_counter')
          ->key(array('nid' => $node_id))
          ->fields(array(
            'daycount' => 0,
            'totalcount' => $statistics->totalcount,
            'timestamp' => time(),
          ))
          ->execute();
        google_analytics_counter_queue_log(WATCHDOG_DEBUG, "Updated Drupal core statistics' {node_counter} table for node @node_id.", array('@node_id' => $node_id));
      }
    }
  }
}
