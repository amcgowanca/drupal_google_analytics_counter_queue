<?php
/**
 * @file
 * statistics.google_analytics_counter_queue.inc
 */

/**
 * Implements hook_google_analytics_counter_queue_statistics_processed().
 */
function statistics_google_analytics_counter_queue_statistics_processed($statistics, $source_path) {
  if (!empty($statistics) && google_analytics_counter_queue_statistics_allow_update()) {
    if ($node_id = google_analytics_counter_queue_path_is_node_view($source_path)) {
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
