<?php
/**
 * @file
 * rules.google_analytics_counter_queue.inc
 */

/**
 * Implements hook_google_analytics_counter_queue_queue_item_insert().
 */
function rules_google_analytics_counter_queue_queue_item_insert(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  rules_invoke_event('google_analytics_counter_queue_queue_item_insert', $queue_item);
}

/**
 * Implements hook_google_analytics_counter_queue_queue_item_update().
 */
function rules_google_analytics_counter_queue_queue_item_update(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  rules_invoke_event('google_analytics_counter_queue_queue_item_update', $queue_item);
}

/**
 * Implements hook_google_analytics_counter_queue_queue_item_delete().
 */
function rules_google_analytics_counter_queue_queue_item_delete(GoogleAnalyticsCounterQueueQueueItem $queue_item) {
  rules_invoke_event('google_analytics_counter_queue_queue_item_delete', $queue_item);
}

/**
 * Implements hook_google_analytics_counter_queue_statistics_processed().
 */
function rules_google_analydtics_counter_queue_statistics_processed($statistics, $source_path) {
  rules_invoke_event('google_analytics_counter_queue_statistics_processed', $statistics, $source_path);
}
