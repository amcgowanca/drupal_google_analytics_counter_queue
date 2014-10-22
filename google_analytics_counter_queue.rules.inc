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
    'group' => t('Google Analytics Counter Queue'),
    'parameter' => array(
      'queue_type' => array(
        'type' => 'text',
        'label' => t('Queue'),
        'options list' => 'google_analytics_counter_queue_rules_queue_type_list',
        'description' => t('Select the queue which the queue item will be created in.'),
      ),
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
      'queue_type' => array(
        'type' => 'text',
        'label' => t('Queue'),
        'options list' => 'google_analytics_counter_queue_rules_queue_type_list',
        'description' => t('Select the queue which the queue item will be created in.'),
      ),
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
 * @param string $queue_type
 *   The queue's machine name.
 * @param string $position
 *   The position identifier (top or bottom).
 * @param object $node
 *   The node object to create a queue item for.
 *
 * @param array $context
 *   Rules action context.
 */
function google_analytics_counter_queue_rules_action_create_queue_item_for_node($queue_type, $position, $node, array $context = array()) {
  if ($queue = DrupalQueue::get($queue_type)) {
    // Only create a queue item should it not already exist within the queue.
    if (!google_analytics_counter_queue_queue_exists_with_path($queue, 'node/' . $node->nid)) {
      $queue_item = new GoogleAnalyticsCounterQueueQueueItem($queue, NULL, 'node/' . $node->nid);
      $queue_item->setWeight(google_analytics_counter_queue_queue_item_next_weight($position, $queue));
      $queue_item->save();
    }
  }
}

/**
 * Callback for deleting a queue item based on the node's path (node/[nid]).
 *
 * @param string $queue_type
 *   The queue's machine name.
 * @param object $node
 *   The node object to delete the queue item for if it exists.
 *
 * @param array $context
 *   Rules action context.
 */
function google_analytics_counter_queue_rules_action_remove_queue_item_for_node($queue_type, $node, array $context = array()) {
  if ($queue = DrupalQueue::get($queue_type)) {
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
}

/**
 * Returns an array of option containing queue type information for rules.
 *
 * @return array
 *   An associative array of machine_name => label for queue types.
 */
function google_analytics_counter_queue_rules_queue_type_list() {
  return array(
    'google_analytics_counter_queue' => t('General queue'),
    'google_analytics_counter_priority_queue' => t('Priority queue'),
  );
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
