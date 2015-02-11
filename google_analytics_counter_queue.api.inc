<?php
/**
 * @file
 * google_analytics_counter_queue.api.inc
 */

/**
 * Registers Google Analytics Counter Queue API information.
 *
 * @return array
 *   Returns an array containing the following keys:
 *     * api (required): The version of API the module implements.
 *     * path (optional): If includes are stored somewhere other than within the
 *       root module directory, specify its path here.
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
 *   The display name to change.
 */
function hook_google_analytics_counter_queue_xoauth_display_name_alter(&$display_name) {
  $display_name = variable_get('site_name', $display_name);
}

/**
 * Notifies modules after the API access token has been revoked.
 */
function hook_google_analytics_counter_queue_api_revoked() {
  drupal_set_message(t('Google Analytics API token has been revoked for Google Analytics Counter Queue.'), 'status');
}

/**
 * The Queue Item has been successfully created and inserted into the database.
 *
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
 * The Queue Item has been successfully updated.
 *
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The updated queue item.
 */
function hook_google_analytics_counter_queue_queue_item_update(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Queue item @id has been updated.', array('@id' => $queue_item->getIdentifier()));
}

/**
 * The Queue Item has been deleted.
 *
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The deleted queue item.
 */
function hook_google_analytics_counter_queue_queue_item_delete(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'Queue item @id has been deleted from queue @queue.', array('@id' => $queue_item->getIdentifier(), '@queue' => $queue_item->getQueue()->getName()));
}

/**
 * @param $statistics
 * @param $source_path
 */
function hook_google_analytics_counter_queue_statistics_processed($statistics, $source_path) {
  google_analytics_counter_queue_log(WATCHDOG_DEBUG, 'The statistics for the source path @path have been processed.', array('@path' => $source_path));
}

/**
 * Allows other modules to alter the statistical overview information.
 *
 * @param array $statistics_info
 *   The statistic information.
 * @param array $statistics_info_header
 *   The statistic table headers.
 */
function hook_google_analytics_counter_queue_statistic_overview_info_alter(array &$statistics_info, array &$statistics_info_header) {
  // TODO: Add a basic example.
}
