<?php
/**
 * @file
 * google_analytics_counter_queue.statistics.inc
 */

/**
 *
 *
 * @param $path
 * @return mixed
 */
function google_analytics_counter_queue_statistics_path($path, $reset = FALSE) {
  $static_fast_cache = &drupal_static(__FUNCTION__, array());
  if (!isset($static_fast_cache[$path]) || isset($static_fast_cache[$path]) && $reset) {
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
      $static_fast_cache[$path] = $row;
    }
    else {
      $static_fast_cache[$path] = FALSE;
    }
  }

  return $static_fast_cache[$path];
}

/**
 * @param $path
 * @param int $total_count
 * @param int $timestamp
 * @return bool
 */
function _google_analytics_counter_queue_statistics_create($path, $total_count = 0, $timestamp = REQUEST_TIME) {
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

    $static_fast_cache = &drupal_static('google_analytics_counter_queue_statistics_path', array());
    $static_fast_cache[$path] = (object) $statistic;
    return $static_fast_cache[$path];
  }

  return FALSE;
}



/**
 * @param $statistic
 * @return string
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
 * @param $form
 * @param $form_state
 * @param $statistic
 * @return mixed
 */
function google_analytics_counter_queue_statistic_delete_confirm($form, &$form_state, $statistic) {
  $form['statistic_id'] = array(
    '#type' => 'value',
    '#value' => $statistic->statistic_id,
  );
  return confirm_form($form, t('Are you sure you want to delete the statistical information for %path?', array('%path' => $statistic->path)), 'admin/config/system/google-analytics-counter-queue/statistics', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

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

function google_analytics_counter_queue_statistic_load($statistic_id) {
  // TODO: Re-implement so that it shares data or common implementation with that of google_analytics_coutner_queue_statistics_path().
  $static_fast_cache = &drupal_static('google_analytics_counter_queue_statistics', array());
  if (!isset($static_fast_cache[$statistic_id])) {
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
      $static_fast_cache[$row->statistic_id] = $row;
    }
    else {
      $static_fast_cache[$statistic_id] = FALSE;
    }
  }
  return $static_fast_cache[$statistic_id];
}

function google_analytics_counter_queue_statistic_delete($statistic_id) {
  // TODO: Probably wrap this in a try-catch?
  db_delete('google_analytics_counter_queue_statistics_path')
    ->condition('statistic_id', $statistic_id)
    ->execute();
  db_delete('google_analytics_counter_queue_statistics')
    ->condition('statistic_id', $statistic_id)
    ->execute();
  $static_fast_cache = &drupal_static('google_analytics_counter_queue_statistics', array());
  if (isset($static_fast_cache[$statistic_id])) {
    unset($static_fast_cache[$statistic_id]);
  }
}
