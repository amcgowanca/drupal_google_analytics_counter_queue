<?php
/**
 * @file
 * google_analytics_counter_queue.api.inc
 */

/**
 * Enables altering a GoogleAnalyticsCounterQueueGoogleApiQuery instance.
 *
 * This hook is executed within the `execute` method of the query,
 *
 * @param GoogleAnalyticsCounterQueueGoogleApiQuery $query
 *   The query object to alter prior to executing.
 */
function hook_google_analytics_counter_queue_google_analytics_api_query_alter(GoogleAnalyticsCounterQueueGoogleApiQuery $query) {
  $query->setMethod(GoogleAnalyticsCounterQueueGoogleApiQuery::HTTP_METHOD_POST);
}

/**
 * Enables modules to alter the xoauth_display_name property of HTTP requests.
 *
 * @param string $display_name
 *   The display name to change.
 */
function hook_google_analytics_counter_queue_xoauth_display_name_alter(&$display_name) {
  $display_name = variable_get('site_name', $display_name);
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The queue item which was created.
 */
function hook_google_analytics_counter_queue_queue_item_insert(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  watchdog(WATCHDOG_DEBUG, t('New queue item created in @queue with id @id.', array(
    '@queue' => $queue_item->getQueue()->getName(),
    '@id' => $queue_item->getIdentifier(),
  )));
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The updated queue item.
 */
function hook_google_analytics_counter_queue_queue_item_update(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  watchdog(WATCHDOG_DEBUG, t('Queue item @id has been updated.', array('@id' => $queue_item->getIdentifier())));
}

/**
 * @param GoogleAnalyticsCounterQueueQueueItem $queue_item
 *   The deleted queue item.
 */
function hook_google_analytics_counter_queue_queue_item_delete(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  watchdog(WATCHDOG_DEBUG, t('Queue item @id has been deleted from queue @queue.', array('@id' => $queue_item->getIdentifier(), '@queue' => $queue_item->getQueue()->getName())));
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
  watchdog(WATCHDOG_DEBUG, t('Queue item @id has been processed.', array('@id' => $queue_item->getIdentifier())));
}
