<?php
/**
 * @file
 * google_analytics_counter_queue.theme.inc
 */

/**
 *
 */
function theme_google_analytics_counter_queue_google_analytics_profile_label($variables) {
  return $variables['profile']['name'] . ' (' . $variables['profile']['id'] . ')';
}

/**
 * Renders a table enabling `tableselect` like functionality with `tabledrag`.
 *
 * @param array $variables
 *   An array of variables used for processing and theming the element.
 *
 * @return string
 *   The rendered table with `tableselect` and `drag` ordering.
 */
function theme_google_analytics_counter_queue_tableselectdrag($variables) {
  $element = $variables['element'];
  $rows = array();
  $header = $element['#header'];

  if (!empty($element['#rows'])) {
    foreach (element_children($element['#rows']) as $key) {
      $row = array();
      $row['data'] = array();

      if (isset($element['#rows'][$key]['#attributes'])) {
        $row += $element['#rows'][$key]['#attributes'];
      }

      foreach ($element['#header'] as $field_name => $title) {
        if (!isset($element['#rows'][$key][$field_name]['data']['#name'])) {
          $name_value = $element['#name'] . '[' . implode('][', array($field_name, $key)) . ']';
          $element['#rows'][$key][$field_name]['data']['#name'] = $name_value;
        }

        $row['data'][] = drupal_render($element['#rows'][$key][$field_name]);
      }
      $rows[] = $row;
    }

    if ($element['#js_select']) {
      drupal_add_js('misc/tableselect.js');
      // Retrieve first element so that required classes are attached.
      $header_keys = array_keys($header);
      $header[$header_keys[0]]['class'] = array('select-all', 'google-analytics-counter-queue--tableselectdrag-select-all');
    }

    $tabledrag_js_added = &drupal_static('drupal_add_tabledrag', FALSE);
    if (!$tabledrag_js_added) {
      drupal_add_library('system', 'jquery.cookie');
      drupal_add_js('misc/tabledrag.js', array('weight' => -1));
      $tabledrag_js_added = TRUE;
    }

    $target = !empty($element['#tableselectdrag_settings']['subgroup']) ? $element['#tableselectdrag_settings']['subgroup'] : $element['#tableselectdrag_settings']['group'];
    $source = !empty($element['#tableselectdrag_settings']['source']) ? $element['#tableselectdrag_settings']['source'] : $element['#attributes']['id'];

    $js_settings['tableDrag'][$element['#attributes']['id']][$element['#tableselectdrag_settings']['group']][] = array(
      'target' => $target,
      'source' => $source,
      'relationship' => $element['#tableselectdrag_settings']['relationship'],
      'action' => $element['#tableselectdrag_settings']['action'],
      'hidden' => $element['#tableselectdrag_settings']['hidden'],
      'limit' => $element['#tableselectdrag_settings']['limit'],
    );
    drupal_add_js($js_settings, 'setting');
    google_analytics_counter_queue_add_admin_css();
  }

  return theme('table', array('header' => $header, 'rows' => $rows, 'empty' => $element['#empty'], 'attributes' => $element['#attributes']));
}
