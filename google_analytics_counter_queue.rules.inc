<?php
/**
 * @file
 * google_analytics_counter_queue.rules.inc
 */

/**
 * Implements hook_rules_data_info().
 */
function google_analytics_counter_queue_rules_data_info() {
  return array(
    'google_analytics_counter_queue_queue_item' => array(
      'label' => t('google analytics counter queue item'),
    ),
    'google_analytics_counter_queue_statistics' => array(
      'label' => t('google analytics counter queue statistics record'),
    ),
  );
}

/**
 * Implements hook_rules_event_info().
 */
function google_analytics_counter_queue_rules_event_info() {
  $items = array();
  $items['google_analytics_counter_queue_queue_item_insert'] = array(
    'label' => t('After queue item inserted'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => array(
      'queue_item' => array(
        'type' => 'google_analytics_counter_queue_queue_item',
        'label' => t('created queue item'),
      ),
    ),
  );
  $items['google_analytics_counter_queue_queue_item_update'] = array(
    'label' => t('After queue item updated'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => array(
      'queue_item' => array(
        'type' => 'google_analytics_counter_queue_queue_item',
        'label' => t('updated queue item'),
      ),
    ),
  );
  $items['google_analytics_counter_queue_queue_item_delete'] = array(
    'label' => t('After queue item deleted'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => array(
      'queue_item' => array(
        'type' => 'google_analytics_counter_queue_queue_item',
        'label' => t('deleted queue item'),
      ),
    ),
  );
  $items['google_analytics_counter_queue_statistics_processed'] = array(
    'label' => t('After statistics for a source path have been processed'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => array(
      'statistics' => array(
        'type' => 'google_analytics_counter_queue_statistics',
        'label' => t('processed statistics record'),
      ),
      'source_path' => array(
        'type' => 'data',
        'label' => t('source path (as a string)'),
      ),
    ),
  );
  return $items;
}

/**
 * Implements hook_rules_action_info().
 */
function google_analytics_counter_queue_rules_action_info() {
  $items = array();
  $items['google_analytics_counter_queue_rules_action_create_queue_item_for_node'] = array(
    'label' => t('Add node to queue'),
    'group' => t('Google Analytics Counter Queue'),
    'parameter' => array(
      'position' => array(
        'type' => 'text',
        'label' => t('Position'),
        'options list' => 'google_analytics_counter_queue_rules_queue_item_position_list',
        'description' => t('Select the default position of the queue item.'),
      ),
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => FALSE,
      ),
    ),
  );
  $items['google_analytics_counter_queue_rules_action_remove_queue_item_for_node'] = array(
    'label' => t('Remove node from queue'),
    'group' => t('Google Analytics Counter Queue'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => FALSE,
      ),
    ),
  );
  return $items;
}

/**
 * Callback for creating a new queue item for a node.
 *
 * This implementation creates a new queue item with node system path only if
 * the path does not already exist within the queue. It is important to note
 * that the item is created for the general queue and not the priority queue.
 *
 * @param string $position
 *   The position identifier (top or bottom).
 * @param object $node
 *   The node object to create a queue item for.
 *
 * @param array $context
 *   Rules action context.
 */
function google_analytics_counter_queue_rules_action_create_queue_item_for_node($position, $node, array $context = array()) {
  if (!google_analytics_counter_queue_queue_exists_with_path('node/' . $node->nid)) {
    $queue_item = new GoogleAnalyticsCounterQueueQueueItem(NULL, 'node/' . $node->nid);
    $queue_item->setWeight(google_analytics_counter_queue_queue_item_next_weight($position));
    $queue_item->save();
  }
}

/**
 * Callback for deleting a queue item based on the node's path (node/[nid]).
 *
 * @param object $node
 *   The node object to delete the queue item for if it exists.
 *
 * @param array $context
 *   Rules action context.
 */
function google_analytics_counter_queue_rules_action_remove_queue_item_for_node($node, array $context = array()) {
  $paths = array('node/' . $node->nid);
  // Select all aliases for this particular node, assuming that the node's
  // source path is that of the node system path - node/%node.
  $query = db_select('url_alias', 'a')
    ->fields('a', array('alias'))
    ->condition('a.source', 'node/' . $node->nid)
    ->execute();
  while ($row = $query->fetchObject()) {
    $paths[] = $row->alias;
  }

  db_delete('google_analytics_counter_queue_queue')
    ->condition('path', $paths, 'IN')
    ->execute();
}

/**
 * Returns an array of default positions for queue items created.
 *
 * @return array
 *   An associative array of position => label for queue items.
 */
function google_analytics_counter_queue_rules_queue_item_position_list() {
  return array('bottom' => t('Default (bottom of queue)'), 'top' => t('Top of queue'));
}
