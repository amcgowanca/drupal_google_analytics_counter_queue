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
    'variables' => google_analytics_counter_queue_rules_event_queue_item_variables(t('created queue item')),
  );
  $items['google_analytics_counter_queue_queue_item_update'] = array(
    'label' => t('After queue item updated'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => google_analytics_counter_queue_rules_event_queue_item_variables(t('updated queue item')),
  );
  $items['google_analytics_counter_queue_queue_item_delete'] = array(
    'label' => t('After queue item deleted'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => google_analytics_counter_queue_rules_event_queue_item_variables(t('deleted queue item')),
  );
  $items['google_analytics_counter_queue_queue_item_processed'] = array(
    'label' => t('After queue item processed'),
    'group' => t('Google Analytics Counter Queue'),
    'variables' => google_analytics_counter_queue_rules_event_queue_item_variables(t('processed queue item')),
  );
  return $items;
}

function google_analytics_counter_queue_rules_event_queue_item_variables($label) {
  $args = array(
    'queue_item' => array(
      'type' => 'google_analytics_counter_queue_queue_item',
      'label' => $label,
    ),
  );
  return $args;
}
