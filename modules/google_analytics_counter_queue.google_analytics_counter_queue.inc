<?php
/**
 * @file
 * google_analytics_counter_queue.google_analytics_counter_queue.inc
 */

/**
 * Implements hook_google_analytics_counter_queue_api_revoked().
 */
function google_analytics_counter_queue_google_analytics_counter_queue_api_revoked() {
  // Reset the statically cached objects.
  drupal_static_reset('google_analytics_counter_queue_api_oauth_consumer');
  drupal_static_reset('google_analytics_counter_queue_api_oauth_token');
  // Reset Google API oauth token data variables.
  variable_set('google_analytics_counter_queue_api_oauth_consumer_key', NULL);
  variable_set('google_analytics_counter_queue_api_oauth_consumer_secret', NULL);
  variable_set('google_analytics_counter_queue_api_oauth_token', NULL);
  variable_set('google_analytics_counter_queue_api_oauth_token_secret', NULL);
  // Clear any and all Google Analytics Counter Queue related cached objects.
  cache_clear_all('google_analytics_counter_queue', 'cache', TRUE);
}
