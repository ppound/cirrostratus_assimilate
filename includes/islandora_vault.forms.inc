<?php

/**
 * @file
 * islandora_vault.forms.inc
 */

/**
 * Create object management form for the vault
 * 
 * @param array $form_state
 * @param object $pid
 *
 * @return string
 */
function islandora_vault_manage_object($object) {
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.api');
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.service');
  try {
    $duracloud_stores = islandora_vault_get_datastores_as_option_array();
    $fedora_stores = islandora_vault_get_datastores_as_option_array('fedora');
  } catch (Exception $e) {
    drupal_set_message(t('Could not load cloudsync datastores.  Please make sure cloudsync is running and you have
      the Islandora Vault configured correctly'), 'error');
    return "";
  }

  if (isset($object['CLOUDSYNC'])) {
    $cloudsync_ds = $object->getDatastream('CLOUDSYNC');
  }
  $task_output = "";
  $cloudsync_json = NULL;
  $backup_task = NULL;
  $restore_task = NULL;
  if (!empty($cloudsync_ds)) {
    $decode = json_decode($cloudsync_ds->content, TRUE);
    $backup_task_id = islandora_vault_identifier_from_uri($decode['backup']['task']['uri']);
    $restore_task_id = islandora_vault_identifier_from_uri($decode['restore']['task']['uri']);
    module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.api');
    try {
      $iv = IslandoraVault::getInstance();
      $backup_task = $iv->getTask($backup_task_id);
      $restore_task = $iv->getTask($restore_task_id);
    } catch (Exception $e) {
      drupal_set_message(t('Error loading task, %s CLOUDSYNC datastream and cloudsync task maybe out of sync %t ', array('%s' => $object->id, '%t' => $e->getMessage())), 'error');
      return "";
    }
    $task_output = islandora_vault_create_task_output('Backup Task Data', $backup_task);
    $task_output .= '<p>' . islandora_vault_create_task_output('Restore Task Data', $restore_task);
  }
  $tasks = array();
  $tasks['backup'] = json_decode($backup_task, TRUE);
  $tasks['restore'] = json_decode($restore_task, TRUE);
  $form = array();
  $form['vault_manager'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 0,
    '#prefix' => '',
  );

  $form['vault_manager']['run'] = array(
    '#title' => t('Run Cloudsync Task'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['vault_manager']['run']['form'] = drupal_get_form('islandora_vault_run_cloudsync_task_form', $object, $task_output, $tasks);


  $form['vault_manager']['create'] = array(
    '#title' => t('Create Cloudsync Tasks'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['vault_manager']['create']['form'] = drupal_get_form('islandora_vault_create_cloudsync_task_form', $object, $task_output, $fedora_stores, $duracloud_stores);


  $form['vault_manager']['delete'] = array(
    '#title' => t('Delete Cloudsync Tasks'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['vault_manager']['delete']['form'] = drupal_get_form('islandora_vault_delete_cloudsync_task_form', $object, $task_output, $tasks);



  $form['vault_manager']['cloudsync_reports'] = array(
    '#title' => t('Cloudsync Reports'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['vault_manager']['cloudsync_reports']['form'] = drupal_get_form('islandora_vault_cs_report_form', $object, $task_output, $tasks);


  $form['vault_manager']['duracloud_reports'] = array(
    '#title' => t('Duracloud Reports'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );


  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @param type $object
 * @param type $task_output
 * @param type $cloudsync_json
 * @return string 
 */
function islandora_vault_cs_report_form($form, &$form_state, $object, $task_output, $tasks) {
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  else {
    $form['run']['titlebox'] = array(
      '#type' => 'item',
      '#title' => t("Cloudsync Task not found for @collection_pid", array('@collection_pid' => $object->id)),
      '#description' => t("<strong>Please Create a task.</strong><br />"),
    );
    return $form;
  }

  $backup_task_id = islandora_vault_identifier_from_uri($tasks['backup']['task']['uri']);
  $restore_task_id = islandora_vault_identifier_from_uri($tasks['restore']['task']['uri']);
  $backup_array = islandora_vault_get_logs_by_task($backup_task_id);
  $restore_array = islandora_vault_get_logs_by_task($restore_task_id);
  $output = islandora_vault_print_logs('Backup Logs', $backup_array);
  $output .= '<p>' . islandora_vault_print_logs('Restore Logs', $restore_array);
  $form['run']['titlebox'] = array(
    '#type' => 'item',
    '#title' => t("Cloudsync Logs for @collection_pid", array('@collection_pid' => $object->id)),
    '#description' => $output,
  );
  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @param type $object
 * @param type $task_output
 * @param type $cloudsync_json
 * @return string 
 */
function islandora_vault_delete_cloudsync_task_form($form, &$form_state, $object, $task_output, $tasks) {
  if (!isset($object['CLOUDSYNC'])) {
    $form['delete']['titlebox'] = array(
      '#type' => 'item',
      '#title' => t("Cloudsync Task not found for @collection_pid", array('@collection_pid' => $object->id)),
      '#description' => t("<strong>Please Create a task.</strong><br />"),
    );
    return $form;
  }

  $form['delete']['titlebox'] = array(
    '#type' => 'item',
    '#title' => t("Cloudsync Task for @collection_pid", array('@collection_pid' => $object->id)),
    '#description' => t("<strong>This will delete both the backup and restore tasks, the corresponding set and all logs associated with these tasks.</strong><br />"
        . $task_output),
  );

  $form['delete']['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $disabled = FALSE;
  if ($tasks['backup']['task']['state'] != 'Idle' ||
      $tasks['restore']['task']['state'] != 'Idle') {
    $disabled = TRUE;
  }

  $form['delete']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete tasks'),
    '#suffix' => t('<br /><em>This will delete both the restore and backup tasks, the corresponding set and all cloudsync logs associated with these tasks.  This will not delete
      any fedora data or any items already copied to Duracloud</em>'),
    '#disabled' => $disabled,
  );

  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @return type 
 */
function islandora_vault_delete_cloudsync_task_form_submit($form, &$form_state) {
  $object = islandora_object_load($form_state['values']['pid']);
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  else {
    drupal_set_message(t('Error loading task for %s', array('%s' => $object->id)), 'error');
    return $form;
  }

  $decode = json_decode($json->content, TRUE);

  try {
    $task = islandora_vault_identifier_from_uri($decode['backup']['task']['uri']);
    $associated_logs = islandora_vault_get_logs_by_task($task);
    islandora_vault_delete_logs($associated_logs);
    islandora_vault_delete_task_and_set($task);
  } catch (Exception $e) {
    drupal_set_message(t('Error Deleting backup task. %s', array('%s' => $e->getMessage())));
    return "";
  }
  drupal_set_message(t('Task and logs deleted for %s backup task', array('%s' => $object->id)));
  try {
    $task = islandora_vault_identifier_from_uri($decode['restore']['task']['uri']);
    $associated_logs = islandora_vault_get_logs_by_task($task);
    islandora_vault_delete_logs($associated_logs);
    islandora_vault_delete_task_and_set($task);
  } catch (Exception $e) {
    drupal_set_message(t('Error Deleting task. %s', array('%s' => $e->getMessage())));
    return "";
  }
  drupal_set_message(t('Task, set and logs deleted for %s restore task', array('%s' => $object->id)));
  try {
    $object->repository->api->m->purgeDatastream($object->id, 'CLOUDSYNC');
  } catch (Exception $e) {
    drupal_set_message(t('Error deleting CLOUDSYNC datastream deleted for %s', array('%s' => $object->id)), 'error');
  }
  drupal_set_message(t('CLOUDSYNC datastream deleted for %s', array('%s' => $object->id)));
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @param type $object
 * @param type $task_output
 * @param type $cloudsync_json
 * @return string 
 */
function islandora_vault_run_cloudsync_task_form($form, &$form_state, $object, $task_output, $tasks) {
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $form = array();
  if (!isset($object['CLOUDSYNC'])) {
    $form['run']['titlebox'] = array(
      '#type' => 'item',
      '#title' => t("Cloudsync Task not found for @collection_pid", array('@collection_pid' => $object->id)),
      '#description' => t("<strong>Please Create a task.</strong><br />"),
    );
    return $form;
  }

  $form['run']['titlebox'] = array(
    '#type' => 'item',
    '#title' => t("Cloudsync Task for @collection_pid", array('@collection_pid' => $object->id)),
    '#description' => t("<strong>This task will run in the background.  When this task is completed it's 
      state will be reported as idle.</strong><br />"),
  );

  $form['run']['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $disabled = FALSE;
  if ($tasks['backup']['task']['state'] != 'Idle' ||
      $tasks['restore']['task']['state'] != 'Idle') {
    $disabled = TRUE;
  }

  $form['run']['submit'] = array(
    '#prefix' => t('Current state is %s <p>', array('%s' => $tasks['backup']['task']['state'])),
    '#type' => 'submit',
    '#value' => t('Copy from Fedora to Duracloud'),
    '#disabled' => $disabled,
  );

  $form['run']['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => 'Advanced',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['run']['advanced']['titlebox'] = array(
    '#type' => 'item',
    '#title' => 'Run the restore task',
    '#description' => 'This will copy items from Duracloud to Fedora,  Make sure you want to restore Fedora objects
      before you run this task.  This task will run in the background.',
  );
  $form['run']['advanced']['restore_button'] = array(
    '#prefix' => t('Current state is %s <p>', array('%s' => $tasks['restore']['task']['state'])),
    '#type' => 'submit',
    '#value' => t('Copy from Duracloud to Fedora'),
    '#disabled' => $disabled,
  );

  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @return type 
 */
function islandora_vault_run_cloudsync_task_form_submit($form, &$form_state) {
  //TODO compare uris in case some one has pointed us at a different cloudsync with
  //existing tasks (we could run the wrong task)
  $object = islandora_object_load($form_state['values']['pid']);
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  else {
    form_set_error(NULL, t('Error loading task for %s', array('%s' => $object->id)));
    return $form;
  }
  $decode = json_decode($json->content, TRUE);
  $task_type = 'Backup';
  if ($form_state['values']['op'] == 'Copy from Duracloud to Fedora') {
    $task_id = islandora_vault_identifier_from_uri($decode['restore']['task']['uri']);
    $task_type = 'Restore';
  }
  else {
    $task_id = islandora_vault_identifier_from_uri($decode['backup']['task']['uri']);
  }
  try {
    islandora_vault_start_task($task_id);
  } catch (Exception $e) {
    drupal_set_message(t('Error running %t task. %s', array('%s' => $e->getMessage(), '%t' => $task_type)));
  }
  drupal_set_message(t('%s Task started.  Please check back later.  When the task is complete
    the state will be reported as idle and you will be able to view the log in the Cloudsync Reports section.  
    You will not be able to run this task again until it is completed.', array('%s' => $task_type)));
}

/**
 *
 * @param array $form
 * @param array $form_state
 * @param FedoraObject $object
 * @param array $fedora_stores
 * @param array $duracloud_stores
 * @return array|string 
 */
function islandora_vault_create_cloudsync_task_form($form, &$form_state, $object, $task_output, $fedora_stores, $duracloud_stores) {
  $set_query = <<<EOD
select \$object from <#ri>
where (\$object <fedora-rels-ext:isMemberOfCollection> <info:fedora/$object->id>
or \$object <fedora-rels-ext:isMemberOf> <info:fedora/$object->id>
or \$object <mulgara:is> <info:fedora/$object->id>)            
order by \$object
EOD;

  $form = array();
  if (isset($object['CLOUDSYNC'])) {

    $form['migrate']['titlebox'] = array(
      '#type' => 'item',
      '#title' => t("Cloudsync Task already Created and configured for @collection_pid", array('@collection_pid' => $object->id)),
      '#description' => t("<strong>To recreate this task you must first delete it.</strong><br />" . $task_output),
    );
    return $form;
  }
  $form['migrate']['titlebox'] = array(
    '#type' => 'item',
    '#title' => t("Create a Cloudsync tasks for @collection_pid", array('@collection_pid' => $object->id)),
    '#description' => t('This will create two tasks, one for copying objects to Duracloud and one for restoring objects from Duracloud')
  );

  $form['migrate']['toDatastore'] = array(
    '#title' => t('Choose a Duracloud datastore'),
    '#description' => t("All chosen content will be replicated to the selected datastore"),
    '#type' => 'select',
    '#options' => $duracloud_stores,
  );

  $form['migrate']['fromDatastore'] = array(
    '#title' => t('Choose a Fedora datastore'),
    '#description' => t("All chosen content will be replicated to the selected datastore"),
    '#type' => 'select',
    '#options' => $fedora_stores,
  );

  //$form['migrate']['table'] = $table;
  $form['migrate']['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => 'Advanced',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['migrate']['advanced']['set_query'] = array(
    '#title' => t('Query to get a list of pids for this collection'),
    '#description' => t("This query will be used to generate a cloudsync set to be used by a cloudsync task.  In most cases you can use
    the default"),
    '#type' => 'textarea',
    '#default_value' => $set_query,
  );

  $form['migrate']['advanced']['query_type'] = array(
    '#title' => t('Query type'),
    '#type' => 'radios',
    '#default_value' => 'iTQL',
    '#options' => drupal_map_assoc(array(t('iTQL'), t('SPARQL'))),
  );

  $form['migrate']['current'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $form['migrate']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create tasks'),
  );
  return $form;
}

/**
 * Submit handler for object migration form
 * 
 * @param array $form
 * @param array $form_state 
 */
function islandora_vault_create_cloudsync_task_form_submit($form, &$form_state) {
  //module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.service');
  //$pids = array_filter($form_state['values']['table']);
  $current = $form_state['values']['current'];

  try {
    $object = islandora_object_load($current);
  } catch (Exception $e) {
    drupal_set_message(t('error getting object for %s', array('%s' => $current)), 'error');
    return "";
  }
  $iv = IslandoraVault::getInstance();
  $connection = Database::getConnection();
  $options_array = $connection->getConnectionOptions();
  $db = $options_array['database'];
  $ip = getHostbyName(php_uname('n'));
  $label = t('ISLANDORA MANAGED Set for object - !t - !d - !i', array('!t' => $object->id, '!d' => $db, '!i' => $ip));
  $data['queryType'] = $form_state['values']['query_type'];
  $data['queryText'] = $form_state['values']['set_query'];
  try {
    $json = islandora_vault_create_set($label, $data, 'query');
  } catch (Exception $e) {
    drupal_set_message(t('error creating cloudsync list for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  drupal_set_message(t('Created ') . $label);
  $set = json_decode($json, TRUE);
  $backup_task_label = t('ISLANDORA MANAGED Task for object - !t - !d - !i - backup', array('!t' => $object->id, '!d' => $db, '!i' => $ip));
  //CREATE A COPY TASK
  $type = 'copy';
  $data['setUri'] = $set['objectSet']['uri'];
  $data['queryStoreUri'] = $form_state['values']['fromDatastore'];
  $data['sourceStoreUri'] = $form_state['values']['fromDatastore'];
  $data['destStoreUri'] = $form_state['values']['toDatastore'];
  $data['overwrite'] = 'true';
  $data['includeManaged'] = 'true';

  try {
    $backup_json = islandora_vault_create_task($backup_task_label, $data, $type);
  } catch (Exception $e) {
    drupal_set_message(t('error creating cloudsync task for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  drupal_set_message(t('Created ') . $backup_task_label);
  $tasks = array();
  $tasks['backup'] = json_decode($backup_json, TRUE);
  //we need to create a pidlist set for restore duracloud can't do itql
  //we execute the itql query to get the pidlist we will update this set before we run a restore
  $results = $object->repository->ri->itqlQuery($form_state['values']['set_query'], 'unlimited', '0');
  $pid_list = "";
  foreach ($results as $result) {
    $pid_list .= $result['object']['value'] . "\n";
  }
  $pid_list_label = t('ISLANDORA MANAGED PidList Set for object - !t - !d - !i', array('!t' => $object->id, '!d' => $db, '!i' => $ip));
  try {
    $pid_list_json = islandora_vault_create_set($pid_list_label, $pid_list, 'pidList');
  } catch (Exception $e) {
    drupal_set_message(t('error creating cloudsync pidlist set for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  $pid_list_array = json_decode($pid_list_json, TRUE);
  $data['setUri'] = $pid_list_array['objectSet']['uri'];
  $data['queryStoreUri'] = $form_state['values']['toDatastore'];
  $data['sourceStoreUri'] = $form_state['values']['fromDatastore'];
  $data['destStoreUri'] = $form_state['values']['fromDatastore'];
  $restore_task_label = t('ISLANDORA MANAGED Task for object - !t - !d - !i - restore', array('!t' => $object->id, '!d' => $db, '!i' => $ip));
  try {
    $restore_json = islandora_vault_create_task($restore_task_label, $data, $type);
  } catch (Exception $e) {
    drupal_set_message(t('error creating cloudsync task for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  drupal_set_message(t('Created ') . $restore_task_label);
  $tasks['restore'] = json_decode($restore_json, TRUE);
  $ds_string = json_encode($tasks);
  try {
    $object->repository->api->m->addDatastream($current, 'CLOUDSYNC', 'string', $ds_string, array('controlGroup' => 'M', 'dsLabel' => 'CLOUDSYNC'));
  } catch (Exception $e) {
    drupal_set_message(t('error updating CLOUDSYNC datastream for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  drupal_set_message(t('Added CLOUDSYNC datastream to %s ', array('%s' => $current)));
}

