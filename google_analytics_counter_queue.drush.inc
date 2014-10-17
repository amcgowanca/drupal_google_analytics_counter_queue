<?php
/**
 * @file
 * google_analytics_counter_queue.drush.inc
 */

/**
 * Implements hook_drush_command().
 */
function google_analytics_counter_queue_drush_command() {
  $commands = array();
  $commands['google-analytics-counter-queue-oauth-revoke-token'] = array(
    'description' => 'Performs an HTTP request and revokes OAuth Access token.',
    'aliases' => array('gacq-oauth-revoke-token'),
  );
  $commands['google-analytics-counter-queue-run'] = array(
    'description' => 'Processes a queue and set number of items within the queue at a single time.',
    'arguments' => array(
      'queue_name' => 'The name of the queue.',
    ),
    'options' => array(
      'total-items' => 'Total number of items to process at a single time.',
    ),
    'required-arguments' => 1,
  );
  $commands['google-analytics-counter-queue-item-run'] = array(
    'description' => 'Process a single queue item and ensure it is complete (processing all records).',
    'arguments' => array(
      'item_id' => 'The queue item id to process.',
    ),
    'required-arguments' => 1,
  );
  $commands['google-analytics-counter-queue-reset-statistics'] = array(
    'description' => 'Truncates the statistic related database tables.',
  );
  return $commands;
}

/**
 * Drush command callback for `google-analytics-counter-queue-oauth-revoke-token.`
 */
function drush_google_analytics_counter_queue_oauth_revoke_token() {
  if ($client = google_analytics_counter_queue_google_analytics_client()) {
    $client->revokeToken();
  }
}

/**
 * @param string $queue_name
 *   The name of the queue to process items on.
 */
function drush_google_analytics_counter_queue_run($queue_name = '') {
  // TODO: Process items in "bulk".
  // TODO: The `total-items` refers to then number of queue items to process at a single time (re: paths).
  drush_print(dt('Running queue @queue', array('@queue' => $queue_name)));
}

function drush_google_analytics_counter_queue_reset_statistics() {
  foreach (array('google_analytics_counter_queue_statistics', 'google_analytics_counter_queue_statistics_path') as $table) {
    db_truncate($table)->execute();
  }
}

function drush_google_analytics_counter_queue_item_run($item_id = NULL) {
  $queue_item = google_analytics_counter_queue_queue_item_load($item_id);
  if (!$queue_item) {
    return drush_print(dt('No queue item exists with id @id.', array('@id' => $item_id)));
  }

  if (!$queue_item->getTotalRecords() && !$queue_item->getRemainingRecords() || $queue_item->getRemainingRecords()) {
    do {
      google_analytics_counter_queue_process_queue($queue_item);
      $queue_item->save();
    }
    while ($queue_item->getRemainingRecords());
    $queue_item->delete();
  }
}

