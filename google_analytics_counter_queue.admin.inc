<?php
/**
 * @file
 * google_analytics_counter_queue.admin.inc
 */

/**
 * Returns a system settings form for the Google Analytics Counter Queue.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 *
 * @return array
 *   Returns a system settings form with Google Analytics Counter Queue settings.
 */
function google_analytics_counter_queue_admin_settings_form($form, $form_state) {
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['settings']['google_analytics_counter_queue_queue_process_admin_paths'] = array(
    '#type' => 'radios',
    '#title' => t('Process admin paths'),
    '#description' => t('Used to determine whether or not the queue items with administrative paths are processed.'),
    '#options' => array(
      1 => t('Yes'),
      0 => t('No'),
    ),
    '#default_value' => (int) google_analytics_counter_queue_queue_process_admin_paths(),
  );
  $form['settings']['google_analytics_counter_queue_queue_ignored_paths'] = array(
    '#type' => 'textarea',
    '#title' => t('Paths to ignore'),
    '#description' => t("Specify paths to ignore during view count processing. Enter one path per line. The '*' character is a wildcard. Example path %node-wildcard-edit would not be used to calculate the total number of views for %node-wildcard.", array('%node-wildcard' => 'node/*', '%node-wildcard-edit' => 'node/*/edit')),
    '#default_value' => google_analytics_counter_queue_queue_ignored_paths(),
  );
  $form['settings']['google_analytics_counter_queue_update_statistics_node_counter'] = array(
    '#type' => 'radios',
    '#title' => t('Update statistics node counter'),
    '#description' => t('Used to determine whether or not the statistics node_counter table should be updated with the total count for node specific paths.'),
    '#options' => array(
      1 => t('Yes'),
      0 => t('No'),
    ),
    '#default_value' => google_analytics_counter_queue_update_statistics_node_counter(),
  );
  $form['settings']['google_analytics_counter_queue_execute_queues_on_cron'] = array(
    '#type' => 'radios',
    '#title' => t('Process queues on cron'),
    '#description' => t('Used to determine whether or not the queues should be processed during the execution of cron jobs (<em>hook_cron()</em>) or not. For large sites, it is recommended that a separate process is created for processing the queues.'),
    '#options' => array(
      1 => t('Yes'),
      0 => t('No'),
    ),
    '#default_value' => google_analytics_counter_queue_execute_queues_on_cron(),
  );
  $form['settings']['google_analytics_counter_queue_google_analytics_queue_item_threshold'] = array(
    '#type' => 'select',
    '#title' => t('Queue item process threshold'),
    '#description' => t('The number of attempts to process a single queue item when the first attempt fails (e.g. request timed out).'),
    '#options' => array(
      '1' => '1',
      '2' => '2',
      '3' => '3',
      '5' => '5',
      '10' => '10',
    ),
    '#default_value' => google_analytics_counter_queue_google_analytics_queue_item_threshold(),
  );
  $form['settings']['google_analytics_counter_queue_queue_process_multiple'] = array(
    '#type' => 'select',
    '#title' => t('Multiple queue item processing'),
    '#description' => t('The number of queue items to process at once. <em>Note: This configuration is only used for the custom drush queue processing command implemented by Google Analytics Counter Queue.</em>'),
    '#options' => array(
      '1' => '1',
      '2' => '2',
      '3' => '3',
      '5' => '5',
      '10' => '10',
    ),
    '#default_value' => google_analytics_counter_queue_queue_process_multiple(),
  );
  $form['settings']['google_analytics_counter_queue_api_data_query_max_results'] = array(
    '#type' => 'select',
    '#title' => t('Maximum number of results'),
    '#description' => t('Used to determine the number of results retrieved in a single query from Google Analytics data query.'),
    '#options' => array(
      '50' => '50',
      '100' => '100',
      '250' => '250',
      '500' => '500',
      '1000' => '1000',
    ),
    '#default_value' => google_analytics_counter_queue_api_data_query_max_results(),
  );
  return system_settings_form($form);
}

/**
 * Returns Google Analytics Counter Queue priority queue overview form.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 *
 * @return array
 *   Returns a Drupal FAPI array to render.
 */
function google_analytics_counter_queue_admin_priority_queue($form, &$form_state) {
  if (isset($form_state['values']['op']) && in_array($form_state['values']['op'], array(t('Update'), t('Delete'))) &&
    isset($form_state['values']['operation']) && 'delete' == $form_state['values']['operation'] && !empty($form_state['values']['queue_items']['select'])) {
    return google_analytics_counter_queue_admin_priority_queue_delete_multiple_confirm($form, $form_state, array_filter($form_state['values']['queue_items']['select']));
  }

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => array(
      'delete' => t('Delete selected items'),
      'process' => t('Process selected items'),
    ),
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $table_rows = array();
  $queue = google_analytics_counter_queue_priority_queue();
  if (0 < count($queue)) {
    foreach ($queue as $item) {
      $operation_href_prefix = 'admin/config/system/google-analytics-counter-queue/queue/item/' . $item->getIdentifier();
      $operations = array(
        'edit' => array(
          'title' => t('edit'),
          'href' => $operation_href_prefix . '/edit',
        ),
        'delete' => array(
          'title' => t('delete'),
          'href' => $operation_href_prefix . '/delete',
          'options' => array(
            'query' => array(
              'destination' => 'admin/config/system/google-analytics-counter-queue/priority-queue',
            )
          ),
        ),
      );

      if ($total_records = $item->getTotalRecords()) {
        $remaining_records = $item->getRemainingRecords();
        $processed_info = t('@remaining of @total', array(
          '@total' => $total_records,
          '@remaining' => $remaining_records ? $remaining_records : t('Unknown'),
        ));
      }
      else {
        $processed_info = t('Not started');
      }

      $table_rows[$item->getIdentifier()] = array(
        'label' => array('data' => array(
          '#markup' => l($item->getPath(), $item->getPath())),
        ),
        'processed' => array(
          '#markup' => $processed_info,
        ),
        'operations' => array('data' => array(
          '#theme' => 'links',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        )),
        'weight' => array('data' => array(
          '#type' => 'textfield',
          '#title' => t('Weight for queue item'),
          '#title_display' => 'invisible',
          '#default_value' => $item->getWeight(),
          '#value' => $item->getWeight(),
          '#size' => 3,
          '#attributes' => array('class' => array('queue-item-weight')),
        )),
        '#attributes' => array('class' => array('draggable')),
      );
    }
  }

  $form['queue_items'] = array(
    '#tree' => TRUE,
    '#type' => 'google_analytics_counter_queue_tableselectdrag',
    '#header' => array(
      'label' => t('URL'),
      'processed' => t('Processed'),
      'operations' => t('Operations'),
      'weight' => t('Weight'),
    ),
    '#rows' => $table_rows,
    '#empty' => t('There are currently no priority queue items.'),
    '#attributes' => array(
      'id' => 'queue-items',
    ),
    '#tableselectdrag_settings' => array(
      'action' => 'order',
      'relationship' => 'self',
      'group' => 'queue-item-weight',
    ),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );

  return $form;
}

/**
 * Validation handler for the priority queue overview management form.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 */
function google_analytics_counter_queue_admin_priority_queue_validate($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Update'):
      if (!isset($form_state['values']['queue_items']['select'])) {
        form_set_error('options][operation', t('You must select at least one queue item to perform operations on.'));
      }
      break;
  }
}

/**
 * Submit handler for the priority queue overview management form.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 */
function google_analytics_counter_queue_admin_priority_queue_submit($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Save changes'):
      if (!empty($form_state['values']['queue_items']['weight'])) {
        foreach ($form_state['values']['queue_items']['weight'] as $item_id => $weight) {
          if ($queue_item = google_analytics_counter_queue_queue_item_load($item_id)) {
            $queue_item->setWeight($weight);
            $queue_item->save();
          }
        }
        drupal_set_message(t('You have successfully saved the priority queue item weights.'));
      }
      break;

    case t('Update'):
    default:
      if ('process' == $form_state['values']['operation']) {
        $queue_items = array_keys($form_state['values']['queue_items']['select']);
        batch_set(google_analytics_counter_queue_process_queue_batch($queue_items));
      }
      // Force the form to rebuild, this will allow for "delete" to send a
      // confirmation form prior to actually deleting.
      $form_state['rebuild'] = TRUE;
      break;
  }
}

/**
 * Mass delete confirmation form for priority queue items.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 * @param array $queue_items
 *   An array of queue item identifiers to delete.
 *
 * @return array
 *   Returns the confirmation form to render.
 */
function google_analytics_counter_queue_admin_priority_queue_delete_multiple_confirm($form, $form_state, $queue_items) {
  $form['queue_items'] = array(
    '#tree' => TRUE,
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
  );
  foreach ($queue_items as $item_id) {
    if ($queue_item = google_analytics_counter_queue_queue_item_load($item_id)) {
      $form['queue_items'][$queue_item->getIdentifier()] = array(
        '#type' => 'hidden',
        '#value' => $queue_item->getIdentifier(),
        '#prefix' => '<li>',
        '#suffix' => t('<em>@url</em>', array('@url' => $queue_item->getData()->path)) . '</li>',
      );
    }
  }
  $form['operation'] = array(
    '#type' => 'hidden',
    '#value' => 'delete',
  );
  $form['#submit'][] = 'google_analytics_counter_queue_admin_priority_queue_delete_multiple_confirm_submit';
  $confirm_question = format_plural(count($queue_items), 'Are you sure you want to delete this item?', 'Are you sure you want to delete these items?');
  return confirm_form($form, $confirm_question, 'admin/config/system/google-analytics-counter-queue/priority-queue', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Mass delete confirmation form submit handler; performs delete operations.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 */
function google_analytics_counter_queue_admin_priority_queue_delete_multiple_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    foreach (array_keys($form_state['values']['queue_items']) as $item_id) {
      if ($queue_item = google_analytics_counter_queue_queue_item_load($item_id)) {
        $queue_item->delete();
      }
    }
    drupal_set_message(t('You have successfully deleted all selected queue items.'));
    $form_state['redirect'] = 'admin/config/system/google-analytics-counter-queue/priority-queue';
  }
}

/**
 * Authentication and analytic profile selection form.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 *
 * @return array
 *   Returns the authentication form to render.
 */
function google_analytics_counter_queue_admin_authentication_form($form, $form_state) {
  $client = google_analytics_counter_queue_api_client();
  if ($client->isAuthenticated()) {
    $webprops_response = google_analytics_counter_queue_api_request_web_properties();
    $profiles_response = google_analytics_counter_queue_api_request_profiles();

    $options = array();

    $current_profile_id = google_analytics_counter_queue_api_profile_id();
    $current_profile = NULL;

    foreach ($profiles_response['data']['items'] as $profile) {
      $webprop = NULL;
      foreach ($webprops_response['data']['items'] as $webprop_value) {
        if ($webprop_value['id'] == $profile['webPropertyId']) {
          $webprop = $webprop_value;
        }
      }

      $options[$webprop['name']][$profile['id']] = theme('google_analytics_counter_queue_google_analytics_profile_label', array('profile' => $profile));
      if ($current_profile_id == $profile['id']) {
        $current_profile = $profile;
      }
    }

    $form['google_analytics'] = array(
      '#type' => 'fieldset',
      '#title' => t('Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['google_analytics']['google_analytics_counter_queue_profile_id'] = array(
      '#type' => 'select',
      '#title' => t('Reports profile'),
      '#options' => $options,
      '#default_value' => $current_profile_id,
      '#description' => t('Choose your Google Analytics profile.') . (!empty($current_profile) ? t('The currently active profile is: %profile.', array('%profile' => theme('google_analytics_counter_queue_google_analytics_profile_label', array('profile' => $current_profile)))) : ''),
      '#required' => TRUE,
    );
    $form['operations'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('container-inline')),
    );
    $form['operations']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
    );
    $form['operations']['revoke_access_token'] = array(
      '#type' => 'submit',
      '#value' => t('Revoke access token'),
    );
  }
  else {
    $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Setup'),
      '#description' => t('When you submit this form, you will be redirected to Google for authentication. Login with the account that has credentials to the Google Analytics profile you would like to use.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['settings']['google_analytics_counter_queue_hd'] = array(
      '#type' => 'textfield',
      '#title' => t('Google Apps for Business (optional)'),
      '#description' => t('Provide the domain name (e.g. google.com) if you domain is registered with Google Apps for Business. Otherwise, leave it blank.'),
      '#default_value' => google_analytics_counter_queue_api_profile_id(),
    );
    $form['settings']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Authorize account'),
    );
  }
  return $form;
}

/**
 * Submit handler for the administrative Authentication form.
 *
 * @param array $form
 *   An array of form elements and properties.
 * @param array $form_state
 *   An array representing the current form's state.
 */
function google_analytics_counter_queue_admin_authentication_form_submit($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  switch ($op) {
    case t('Authorize account'):
      $client = google_analytics_counter_queue_api_client();
      $request_token = $client->getRequestToken();
      $_SESSION['google_analytics_counter_queue']['oauth']['token'] = $request_token;
      $_SESSION['google_analytics_counter_queue']['destination'] = current_path();
      $client->obtainAuthorization();
      break;
    case t('Save settings'):
      variable_set('google_analytics_counter_queue_api_profile_id', $form_state['values']['google_analytics_counter_queue_profile_id']);
      drupal_set_message(t('Google Analytics authentication settings have been successfully saved.'));
      break;
    case t('Revoke access token'):
      if ($client = google_analytics_counter_queue_api_client()) {
        $client->revokeToken();
      }
      drupal_set_message(t('Google Authentication access token has been successfully revoked.'));
      break;
  }
}

function google_analytics_counter_queue_admin_statistics_delete_multiple_confirm($form, &$form_state, $statistics) {
  $form['statistics'] = array(
    '#tree' => TRUE,
    '#prefix' => '<ul>',
    '#suffix' => '</ul>',
  );
  foreach ($statistics as $statistic_id) {
    $statistic = google_analytics_counter_queue_statistic_load($statistic_id);
    $form['statistics'][$statistic_id] = array(
      '#type' => 'hidden',
      '#value' => $statistic_id,
      '#prefix' => '<li>',
      '#suffix' => t('<em>@path</em>', array('@path' => $statistic->path)) . '</li>',
    );
  }

  $form['operation'] = array(
    '#type' => 'hidden',
    '#value' => 'delete',
  );

  $form['#submit'][] = 'google_analytics_counter_queue_admin_statistics_delete_multiple_confirm_submit';
  $confirm_question = format_plural(count($statistics), 'Are you sure you want to delete the statistical information for this path?', 'Are you sure you want to delete the statistical information for these paths?');
  return confirm_form($form, $confirm_question, 'admin/config/system/google-analytics-counter-queue/statistics', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

function google_analytics_counter_queue_admin_statistics_delete_multiple_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    foreach ($form_state['values']['statistics'] as $statistic_id) {
      google_analytics_counter_queue_statistic_delete($statistic_id);
    }

    drupal_set_message(t('You have successfully deleted all selected statistics.'));
    $form_state['redirect'] = 'admin/config/system/google-analytics-counter-queue/statistics';
  }
}

function google_analytics_counter_queue_admin_statistics($form, &$form_state) {
  if (isset($form_state['values']['op']) && in_array($form_state['values']['op'], array(t('Update'), t('Delete'))) && isset($form_state['values']['operation']) && 'delete' == $form_state['values']['operation'] && !empty($form_state['values']['statistics'])) {
    return google_analytics_counter_queue_admin_statistics_delete_multiple_confirm($form, $form_state, array_filter($form_state['values']['statistics']));
  }

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => array(
      'delete' => t('Delete selected items'),
    ),
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $statistic_headers = array(
    'path' => array('data' => t('Path'), 'weight' => 1),
    'totalcount' => array('data' => t('Total count'), 'weight' => 2),
    'updated' => array('data' => t('Last updated'), 'sort' => 'desc', 'weight' => 3),
    'operations' => array('data' => t('Operations'), 'weight' => 4),
  );

  $statistic_rows = array();
  $query = db_select('google_analytics_counter_queue_statistics', 's')->extend('PagerDefault');
  $query->join('google_analytics_counter_queue_statistics_path', 'sp', 's.statistic_id = sp.statistic_id');
  $query = $query->fields('s')
    ->limit(25)
    ->orderBy('s.updated', 'DESC')
    ->orderBy('s.totalcount', 'ASC')
    ->execute();
  while ($row = $query->fetchObject()) {
    $operation_href_prefix = 'admin/config/system/google-analytics-counter-queue/statistics/' . $row->statistic_id;
    $operations = array(
      'view' => array(
        'title' => t('view'),
        'href' => $operation_href_prefix . '/view',
      ),
      'delete' => array(
        'title' => t('delete'),
        'href' => $operation_href_prefix . '/delete',
        'options' => array(
          'query' => array(
            'destination' => current_path(),
          ),
        ),
      ),
    );

    $statistic_rows[$row->statistic_id] = array(
      '#statistic' => $row,
      'path' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $row->path,
          '#href' => $row->path,
        ),
      ),
      'totalcount' => array('data' => $row->totalcount),
      'updated' => array('data' => format_date($row->updated, 'custom', 'D, M. d, Y @ g:i a')),
      'operations' => array('data' => array(
        '#theme' => 'links',
        '#links' => $operations,
        '#attributes' => array('class' => array('links', 'inline')),
      )),
    );
  }

  // Allow for other modules to alter the table related data prior to sorting
  // and rendering. This allows for things like node paths to be translated into
  // the respective node titles.
  google_analytics_counter_queue_module_include();
  drupal_alter('google_analytics_counter_queue_statistics_overview', $statistic_rows, $statistic_headers);
  // Sort the headers for the tableselect as they may have been altered and
  // therefore the weights may have changed.
  uasort($statistic_headers, 'drupal_sort_weight');

  $form['statistics'] = array(
    '#type' => 'tableselect',
    '#header' => $statistic_headers,
    '#options' => $statistic_rows,
    '#empty' => t('There is currently no statistics.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

function google_analytics_counter_queue_admin_statistics_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
