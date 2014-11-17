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
      'multiple' => 'Process multiple queue items at once.'
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
 * Drush callback for processing a single queue, specified by its name.
 *
 * When the `--multiple` option exists, it may exist as a boolean (without a
 * value) and therefore is used to determine whether or not queue items
 * are processed together. When no value exists, the variable named
 * `google_analytics_counter_queue_queue_process_multiple` is used to determine
 * the number of queue items to group together. The number of items to group
 * can be overridden by specifying a numeric value of the `--multiple` option.
 *
 * @param string $queue_name
 *   The name of the queue to process items on.
 */
function drush_google_analytics_counter_queue_run($queue_name = '') {
  if (!in_array($queue_name, array('google_analytics_counter_queue', 'google_analytics_counter_priority_queue'))) {
    return drush_log(dt('The specified queue name, @queue, is not defined by Google Analytics Counter Queue.', array('@queue' => $queue_name)), 'error');
  }

  @ignore_user_abort(TRUE);
  drupal_set_time_limit(0);

  $queue = DrupalQueue::get($queue_name);
  $lock_name = 'google_analytics_counter_queue_run:' . $queue_name;
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
