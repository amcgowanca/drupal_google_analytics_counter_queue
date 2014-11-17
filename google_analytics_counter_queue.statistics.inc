<?php
/**
 * @file
 * google_analytics_counter_queue.statistics.inc
 */

/**
 * Returns a structured array of statically cached statistic objects.
 *
 * @param bool $reset
 *   A boolean indicating whether or not the static cache should be reset.
 *
 * @return array
 *   Returns an associative array containing two primary elements, `ids` and
 *   `paths`. The path element's value contains an associative array of all
 *   loaded statistics keyed by the primary statistic path. The `ids` element's
 *   value contains an associative array of all loaded statistic paths keyed by
 *   the statistic identifier.
 */
function &google_analytics_counter_queue_statistics_static_cache($reset = FALSE) {
  $static_fast_cache = &drupal_static('google_analytics_counter_queue_statistics', array());
  // Initialize the static cache should it be empty, or re-initialize if the
  // reset parameter is TRUE.
  if (empty($static_fast_cache) || $reset) {
    $static_fast_cache = array(
      'ids' => array(),
      'paths' => array(),
    );
  }
  return $static_fast_cache;
}

/**
 * Retrieves statistic objects by a statistic object's identifier.
 *
 * @param int $statistic_id
 *   The statistic identifier.
 *
 * @param bool $reset
 *   A boolean indicating whether or not the static cached results should be
 *   reset. Default value is FALSE.
 *
 * @return mixed
 *   Returns an object representing a statistics records and all associated
 *   paths for a particular path. Should no statistics exist for the path,
 *   FALSE is returned.
 */
function google_analytics_counter_queue_statistic_load($statistic_id, $reset = FALSE) {
  $static_fast_cache = &google_analytics_counter_queue_statistics_static_cache();
  if (!isset($static_fast_cache['ids'][$statistic_id]) || $reset) {
    $query = db_select('google_analytics_counter_queue_statistics', 's')
      ->fields('s')
      ->condition('s.statistic_id', $statistic_id)
      ->execute();
    if ($row = $query->fetchObject()) {
      $row->items = array();
      $query = db_select('google_analytics_counter_queue_statistics_path', 'p')
        ->fields('p')
        ->condition('p.statistic_id', $row->statistic_id)
        ->execute();
      while ($item = $query->fetchObject()) {
        $row->items[$item->id] = $item;
      }
      $static_fast_cache['ids'][$row->statistic_id] = $row->path;
      $static_fast_cache['path'][$row->path] = $row;
    }
    else {
      return FALSE;
    }
  }
  return $static_fast_cache['paths'][$static_fast_cache['ids'][$statistic_id]];
}

/**
 * Retrieves statistics for a particular path.
 *
 * @param string $path
 *   The path to retrieve the statistics for.
 *
 * @param bool $reset
 *   A boolean indicating whether or not the static cached results should be
 *   reset. Default value is FALSE.
 *
 * @return mixed
 *   Returns an object representing a statistics records and all associated
 *   paths for a particular path. Should no statistics exist for the path,
 *   FALSE is returned.
 */
function google_analytics_counter_queue_statistics_path($path, $reset = FALSE) {
  $static_fast_cache = &google_analytics_counter_queue_statistics_static_cache();
  if (!isset($static_fast_cache['paths'][$path]) || isset($static_fast_cache['paths'][$path]) && $reset) {
    $query = db_select('google_analytics_counter_queue_statistics', 's')
      ->fields('s')
      ->condition('s.path', $path)
      ->execute();
    if ($row = $query->fetchObject()) {
      $row->items = array();
      $query = db_select('google_analytics_counter_queue_statistics_path', 'p')
        ->fields('p')
        ->condition('p.statistic_id', $row->statistic_id)
        ->execute();
      while ($item = $query->fetchObject()) {
        $row->items[$item->path] = $item;
      }
      $static_fast_cache['ids'][$row->statistic_id] = $row->path;
      $static_fast_cache['paths'][$path] = $row;
    }
    else {
      return FALSE;
    }
  }
  return $static_fast_cache['paths'][$path];
}

/**
 * Creates a new statistics object for a particular path.
 *
 * @param string $path
 *   The path to create the statistics object for.
 * @param int $total_count
 *   The total count for this statistics object. Default value 0.
 * @param int $timestamp
 *   The timestamp to use for the created and updated properties. Default value
 *   is set to that of the REQUEST_TIME value.
 *
 * @return mixed
 *   Returns the statistics object for the path if it was created. Should a
 *   statistics object exists for the specified path or an error occurred while
 *   inserting the statistic object, FALSE will be returned.
 */
function google_analytics_counter_queue_statistics_create($path, $total_count = 0, $timestamp = REQUEST_TIME) {
  if ($exists = google_analytics_counter_queue_statistics_path($path)) {
    return FALSE;
  }

  $statistic = array(
    'created' => $timestamp,
    'updated' => $timestamp,
    'path' => $path,
    'totalcount' => $total_count,
  );

  $statistic_id = db_insert('google_analytics_counter_queue_statistics')
    ->fields($statistic)
    ->execute();
  if ($statistic_id) {
    $statistic['statistic_id'] = $statistic_id;
    $statistic['items'] = array();
    $statistic = (object) $statistic;
    // Include this $statistic object within the static cached set.
    $static_fast_cache = &google_analytics_counter_queue_statistics_static_cache();
    $static_fast_cache['ids'][$statistic->statistic_id] = $statistic->path;
    $static_fast_cache['paths'][$statistic->path] = $statistic;
    return $statistic;
  }
  return FALSE;
}

/**
 * Page callback for viewing a single statistic.
 *
 * @param object $statistic
 *   The statistic object to view.
 *
 * @return array
 *   A Drupal render page array of elements to render.
 */
function google_analytics_counter_queue_statistic_view_page($statistic) {
  drupal_set_title(t('<em>View statistics for</em> @path', array('@path' => $statistic->path)), PASS_THROUGH);

  $statistics_info_header = array(
    'label' => array('data' => t('Label'), 'class' => array('invisible')),
    'value' => array('data' => t('Value'), 'class' => array('invisible')),
  );

  $statistics_info_rows = array(
    'totalcount' => array(
      'label' => array('data' => array(
        '#markup' => t('Total count'),
        '#prefix' => '<strong>',
        '#suffix' => '</strong>',
      )),
      'value' => array('data' => $statistic->totalcount),
    ),
    'number_of_paths' => array(
      'label' => array('data' => array(
        '#markup' => t('Number of paths'),
        '#prefix' => '<strong>',
        '#suffix' => '</strong>',
      )),
      'value' => array('data' => count($statistic->items)),
    ),
  );

  // Load all modules which implement the Google Analytics Counter Queue API
  // to ensure that they are all within scope.
  google_analytics_counter_queue_module_include();
  // Allow other modules to alter the output. Because this is unique data and
  // relatively difficult to alter in standard Drupal page alters, preprocessors
  // we will allow modules to alter the data via this hook.
  drupal_alter('google_analytics_counter_queue_statistic_overview_info', $statistics_info_rows, $statistics_info_header, $statistic);

  $statistics_by_path_rows = array();
  foreach ($statistic->items as $item) {
    $statistics_by_path_rows[] = array(
      'path' => array('data' => $item->path),
      'views' => array('data' => $item->count)
    );
  }

  $render = array(
    'statistical_info' => array(
      '#type' => 'table',
      '#header' => $statistics_info_header,
      '#rows' => $statistics_info_rows,
      '#prefix' => theme('html_tag', array(
        'element' => array(
          '#tag' => 'h2',
          '#value' => t('Information'),
        ),
      )),
    ),
    'statistics_by_paths' => array(
      '#type' => 'table',
      '#header' => array(
        'path' => array('data' => t('Path')),
        'views' => array('data' => t('Page views')),
      ),
      '#rows' => $statistics_by_path_rows,
      '#prefix' => theme('html_tag', array(
        'element' => array(
          '#tag' => 'h2',
          '#value' => t('Statistics by path'),
        ),
      )),
    ),
  );
  return $render;
}

/**
 * Statistic object delete confirmation form.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 * @param object $statistic
 *   The statistic object to delete.
 *
 * @return array
 *   The confirmation form array.
 */
function google_analytics_counter_queue_statistic_delete_confirm($form, &$form_state, $statistic) {
  $form['statistic_id'] = array(
    '#type' => 'value',
    '#value' => $statistic->statistic_id,
  );
  return confirm_form($form, t('Are you sure you want to delete the statistical information for %path?', array('%path' => $statistic->path)), 'admin/config/system/google-analytics-counter-queue/statistics', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Form submit handler for the statistic delete confirmation form.
 *
 * Upon successful deletion, the user is redirected to the statistics overview.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 */
function google_analytics_counter_queue_statistic_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $statistic = google_analytics_counter_queue_statistic_load($form_state['values']['statistic_id']);
    google_analytics_counter_queue_statistic_delete($form_state['values']['statistic_id']);
    drupal_set_message(t('You have successfully deleted statistic information for path %path.', array(
      '%path' => $statistic->path,
    )));
    drupal_goto('admin/config/system/google-analytics-counter-queue/statistics');
  }
}

/**
 * Deletes a statistic object and its associated paths.
 *
 * @param int $statistic_id
 *   The identifier of the statistic to delete.
 *
 * @throws Exception
 *   Throws any exception caught.
 */
function google_analytics_counter_queue_statistic_delete($statistic_id) {
  $transaction = db_transaction();
  try {
    db_delete('google_analytics_counter_queue_statistics_path')
      ->condition('statistic_id', $statistic_id)
      ->execute();
    db_delete('google_analytics_counter_queue_statistics')
      ->condition('statistic_id', $statistic_id)
      ->execute();
    // Reset the static cache to ensure there are no stale loaded statistics.
    $static_fast_cache = &google_analytics_counter_queue_statistics_static_cache();
    if (isset($static_fast_cache['ids'][$statistic_id])) {
      unset($static_fast_cache['paths'][$static_fast_cache['ids'][$statistic_id]]);
      unset($static_fast_cache['ids'][$statistic_id]);
    }
  }
  catch (Exception $exception) {
    $transaction->rollback();
    throw $exception;
  }
}
