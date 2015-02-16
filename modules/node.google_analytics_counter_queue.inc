<?php
/**
 * @file
 * node.google_analytics_counter_queue.inc
 */

/**
 * Implements hook_google_analytics_counter_queue_statistics_overview_alter().
 */
function node_google_analytics_counter_queue_statistics_overview_alter(&$statistic_rows, &$statistic_headers) {
  if (!empty($statistic_rows)) {
    foreach ($statistic_rows as $statistic_id => &$statistic) {
      if ($node_id = google_analytics_counter_queue_path_is_node_view($statistic['#statistic']->path)) {
        $node = node_load($node_id);
        $statistic['path']['data']['#title'] = format_string('!title  (!path)', array(
          '!title' => $node->title,
          '!path' => $statistic['#statistic']->path,
        ));
      }
    }
  }
}

/**
 * Implements hook_google_analytics_counter_queue_statistics_processed().
 */
function node_google_analytics_counter_queue_statistics_processed($statistics, $source_path) {
  if (module_exists('expire')) {
    if ($node_id = google_analytics_counter_queue_path_is_node_view($source_path)) {
      if ($node = node_load($node_id)) {
        expire_execute_expiration('node', $node, EXPIRE_NODE_CUSTOM);
      }
    }
  }
}

/**
 * Implements hook_google_analytics_counter_queue_api_request_alter().
 */
function node_google_analytics_counter_queue_api_request_alter(array &$parameters, $endpoint, $url) {
  if ('analytics/data' == $endpoint['name'] && !empty($parameters['filters']['ga:pagePath'])) {
    $filters = array();
    foreach ($parameters['filters']['ga:pagePath'] as $filter) {
      if (isset($filter['value'])) {
        if ($paths = _node_google_analytics_counter_queue_generate_node_aliases_for_node_system_path($filter['value'])) {
          foreach ($paths as $path) {
            $filters[] = array(
              'value' => $path,
              'operator' => $filter['operator'],
            );
          }
        }
      }
    }

    if (!empty($filters)) {
      $parameters['filters']['ga:pagePath'] = $filters;
    }
  }
}

/**
 * Helper function for retrieving path aliases for a node system path.
 *
 * @param string $path
 *   A path passed by reference. If the path is a node system path, the returned
 *   value of this variable will be an array containing the node system path and
 *   any URL aliases that might exist for it.
 *
 * @return array
 */
function _node_google_analytics_counter_queue_generate_node_aliases_for_node_system_path($path) {
  $paths = FALSE;
  if ($node_id = google_analytics_counter_queue_path_is_node_view($path)) {
    $original = $path;
    $paths = array($path);
    $query = db_select('url_alias', 'a')
      ->fields('a', array('alias'))
      ->condition('a.source', 'node/' . $node_id)
      ->execute();
    while ($row = $query->fetchObject()) {
      $paths[] = $row->alias;
    }
  }
  return $paths;
}

