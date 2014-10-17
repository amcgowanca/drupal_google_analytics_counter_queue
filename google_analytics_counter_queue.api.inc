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

function hook_google_analytics_counter_queue_queue_item_insert(GoogleAnalyticsCounterQueueQueueItem $queue_item) {

}

function hook_google_analytics_counter_queue_queue_item_update(GoogleAnalyticsCounterQueueQueueItem $queue_item) {

}

function hook_google_analytics_counter_queue_queue_item_delete(GoogleAnalyticsCounterQueueQueueItem $queue_item) {

}
