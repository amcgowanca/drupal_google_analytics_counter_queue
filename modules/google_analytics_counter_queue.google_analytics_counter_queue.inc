<?php
/**
 * @file
 * google_analytics_counter_queue.google_analytics_counter_queue.inc
 */

/**
 * Implements hook_google_analytics_counter_queue_api_revoked().
 */
function google_analytics_counter_queue_google_analytics_counter_queue_api_revoked() {
  // Clear any and all Google Analytics Counter Queue related cached objects.
  cache_clear_all('google_analytics_counter_queue', 'cache', TRUE);
  // Reset the default profile id for processing data from.
  variable_set('google_analytics_counter_queue_api_profile_id', NULL);
}
