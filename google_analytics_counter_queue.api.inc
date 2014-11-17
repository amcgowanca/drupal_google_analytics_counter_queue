<?php
/**
 * @file
 * google_analytics_counter_queue.api.inc
 */

/**
 * @return array
 */
function hook_google_analytics_counter_queue_api() {
  return array(
    'version' => '1.0',
    'path' => drupal_get_path('module', 'google_analytics_counter_queue'),
  );
}

/**
 * Enables modules to alter the xoauth_display_name property of HTTP requests.
 *
 * @param string $display_name.
 *
 *   The display name to change.
 */
function hook_google_analytics_counter_queue_xoauth_display_name_alter(&$display_name) {
  $display_name = variable_get('site_name', $display_name);
}

/**
 *
 */
function hook_google_analytics_counter_queue_api_revoked() {
  drupal_set_message(t('Google Analytics API token has been revoked for Google Analytics Counter Queue.'), 'status');
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The queue item which was created.
 */
function hook_google_analytics_counter_queue_queue_item_insert(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'New queue item created in @queue with id @id.', array(
    '@queue' => $queue_item->getQueue()->getName(),
    '@id' => $queue_item->getIdentifier(),
  ));
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The updated queue item.
 */
function hook_google_analytics_counter_queue_queue_item_update(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Queue item @id has been updated.', array('@id' => $queue_item->getIdentifier()));
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The deleted queue item.
 */
function hook_google_analytics_counter_queue_queue_item_delete(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Queue item @id has been deleted from queue @queue.', array('@id' => $queue_item->getIdentifier(), '@queue' => $queue_item->getQueue()->getName()));
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The processed queue item.
 * @param mixed $statistics
 *   The statistics object containing information about the queue item's path.
 *   If the parameter value is NULL, the queue item has still be processed,
 *   however, no statistical information was collected as it did not exist.
 */
function hook_google_analytics_counter_queue_queue_item_processed(GoogleAnalyticsCounterQueueQueueItem $queue_item, $statistics = NULL) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Queue item @id has been processed.', array('@id' => $queue_item->getIdentifier()));
}

/**
 * @param array $statistics_info
 * @param array $statistics_info_header
 */
function hook_google_analytics_counter_queue_statistic_overview_info_alter(array &$statistics_info, array &$statistics_info_header) {
  // TODO: Add a basic example.
}
