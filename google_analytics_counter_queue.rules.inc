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
    )
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
  $items['google_analytics_counter_queue_queue_item_processed'] = array(
    'label' => t('After queue item processed'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => array(
      'queue_item' => array(
        'type' => 'google_analytics_counter_queue_queue_item',
        'label' => t('processed queue item'),
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
    'configurable' => FALSE,
    'group' => t('Google Analytics Counter Queue'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => FALSE,
      ),
    ),
  );
  $items['google_analytics_counter_queue_rules_action_remove_queue_item_for_node'] = array(
    'label' => t('Remove node from queue'),
    'configurable' => FALSE,
    'group' => t('Google Analytics Counter Queue'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => FALSE,
      ),
    ),
  );
  $items['google_analytics_counter_queue_rules_action_create_priority_queue_item_for_node'] = array(
    'label' => t('Add node to priority queue'),
    'configurable' => FALSE,
    'group' => t('Google Analytics Counter Queue'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => FALSE,
      ),
    ),
  );
  $items['google_analytics_counter_queue_rules_action_remove_priority_queue_item_for_node'] = array(
    'label' => t('Remove node from priority queue'),
    'configurable' => FALSE,
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
 * @param object $node
 *   The node object to create a queue item for.
 *
 * @param array $context
 *   Rules action context.
 */
function google_analytics_counter_queue_rules_action_create_queue_item_for_node($node, array $context = array()) {
  _google_analytics_counter_queue_rules_action_create_queue_item_for_node(google_analytics_counter_queue_queue(), $node, $context);
}

function google_analytics_counter_queue_rules_action_create_priority_queue_item_for_node($node, array $context = array()) {
  _google_analytics_counter_queue_rules_action_create_queue_item_for_node(google_analytics_counter_queue_priority_queue(), $node, $context);
}

function _google_analytics_counter_queue_rules_action_create_queue_item_for_node($queue, $node, array $context) {
  // Only create a queue item should it not already exist within the queue.
  if (!google_analytics_counter_queue_queue_exists_with_path($queue, 'node/' . $node->nid)) {
    $queue_item = new GoogleAnalyticsCounterQueueQueueItem($queue, NULL, 'node/' . $node->nid);
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
  _google_analytics_counter_queue_rules_action_remove_queue_item_for_node(google_analytics_counter_queue_queue(), $node, $context);
}

function google_analytics_counter_queue_rules_action_remove_priority_queue_item_for_node($node, array $context = array()) {
  _google_analytics_counter_queue_rules_action_remove_queue_item_for_node(google_analytics_counter_queue_priority_queue(), $node, $context);
}

function _google_analytics_counter_queue_rules_action_remove_queue_item_for_node($queue, $node, array $context = array()) {
  // Select all aliases for this particular node, assuming that the node's
  // source path is that of the node system path - node/%node.
  $query = db_select('url_alias', 'a')
    ->fields('a', array('alias'))
    ->condition('a.source', 'node/' . $node->nid)
    ->execute();
  while ($row = $query->fetchObject()) {
    // Foreach alias found, attempt to load any queue items which contains
    // the alias as the core path for processing. If it exists, delete it!
    if ($queue_item = google_analytics_counter_queue_queue_item_load_by_path($row->alias)) {
      $queue_item->delete(TRUE);
    }
  }
  // Delete the queue item, should it exists, where the queue item's path is
  // the node system default (node/%).
  if ($queue_item = google_analytics_counter_queue_queue_item_load_by_path('node/' . $node->nid)) {
    $queue_item->delete(TRUE);
  }
}
