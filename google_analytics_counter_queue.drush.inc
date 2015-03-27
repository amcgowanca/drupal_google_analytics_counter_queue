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
    'options' => array(
      'multiple' => 'Process multiple queue items at once.'
    ),
  );
  $commands['google-analytics-counter-queue-item-run'] = array(
    'description' => 'Process a single queue item and ensure it is complete (processing all records).',
    'arguments' => array(
      'item_id' => 'The queue item id to process.',
    ),
    'required-arguments' => 1,
  );
  $commands['google-analytics-counter-queue-process-chunks'] = array(
    'description' => 'Process a chunk of received data from Google Analytics API.',
    'options' => array(
      'chunks' => 'The number of chunks to process. Maximum is 100; default 1.',
      'sleep' => 'The number of seconds to sleep the process for after each chunk processing.',
    ),
  );
  $commands['google-analytics-counter-queue-reset-processing'] = array(
    'description' => 'Truncates queue table and resets processing variable metadata values.',
  );
  $commands['google-analytics-counter-queue-reset-statistics'] = array(
    'description' => 'Truncates the statistics database tables.',
  );
  return $commands;
}

/**
 * Drush command callback for `google-analytics-counter-queue-oauth-revoke-token.`
 */
function drush_google_analytics_counter_queue_oauth_revoke_token() {
  if ($client = google_analytics_counter_queue_api_client()) {
    $client->revokeToken();
  }
}

/**
 * Truncates the statistic related database tables.
 */
function drush_google_analytics_counter_queue_reset_statistics() {
  foreach (array('google_analytics_counter_queue_statistics', 'google_analytics_counter_queue_statistics_path') as $table) {
    db_truncate($table)->execute();
  }
}

/**
 * Truncates the queue table and resets any metadata processing variable values.
 */
function drush_google_analytics_counter_queue_reset_processing() {
  db_truncate('google_analytics_counter_queue_queue')->execute();
  google_analytics_counter_queue_chunk_process_start_index_reset();
}

/**
 * Drush callback for processing a single queue, specified by its name.
 *
 * When the `--multiple` option exists, it may exist as a boolean (without a
 * value) and therefore is used to determine whether or not queue items
 * are processed together. When no value exists, the variable named
 * `google_analytics_counter_queue_queue_process_multiple` is used to determine
 * the number of queue items to group together. The number of items to group
 * can be overridden by specifying a numeric value of the `--multiple` option.
 */
function drush_google_analytics_counter_queue_run() {
  @ignore_user_abort(TRUE);
  drupal_set_time_limit(0);

  $queue = google_analytics_counter_queue_queue();
  $lock_name = 'google_analytics_counter_queue_run';
  if (lock_acquire($lock_name)) {
    if (drush_get_option('multiple', FALSE)) {
      $i = 0;
      $items = array();
      $multiple = (int) drush_get_option('multiple', google_analytics_counter_queue_queue_process_multiple());
      while (($item = $queue->claimItem()) && $i < $multiple) {
        $items[] = $item;
        $i++;
      }

      google_analytics_counter_queue_process_queue($items);
      while ($item = array_pop($items)) {
        $queue->deleteItem($item);
      }
    }
    else {
      // TODO: Add time, option to ignore.
      while ($item = $queue->claimItem()) {
        google_analytics_counter_queue_process_queue($item);
        $queue->deleteItem($item);
      }
    }

    lock_release($lock_name);
  }
  else {
    if (drush_get_option('verbose')) {
      drush_log(dt('You cannot this queue while another process is running.'), 'error');
    }
  }
}

/**
 * Drush callback for processing a single queue item, specified by its id.
 *
 * @param int $item_id
 *   The queue item identifier to process.
 */
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

/**
 * Drush callback for processing chunks of data from Google Analytics API.
 */
function drush_google_analytics_counter_queue_process_chunks() {
  $number_of_chunks = (int) drush_get_option('chunks', 1);
  if (100 < $number_of_chunks) {
    $number_of_chunks = 100;
  }
  elseif (0 > $number_of_chunks) {
    $number_of_chunks = 1;
  }

  if (!$number_of_chunks) {
    return drush_log(dt('The number of chunks to process was zero, exiting early.'));
  }

  drupal_set_time_limit(0);
  // TODO: Add lock mechanism.

  $default_sleep_time = 3;
  $sleep_time = (int) drush_get_option('sleep', $default_sleep_time);
  $sleep_time = 0 < $sleep_time ? $sleep_time : $default_sleep_time;
  for ($i = 0; $i < $number_of_chunks; ) {
    drush_log(dt('Google Analytics Counter Queue: processing chunk @i of @n', array('@i' => ++$i, '@n' => $number_of_chunks)));
    @ignore_user_abort(TRUE);
    google_analytics_counter_queue_process_chunks();
    @ignore_user_abort(FALSE);
    @sleep($sleep_time);
  }
}
