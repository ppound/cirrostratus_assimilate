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

  //module_load_include('inc', 'islandora_basic_collection', 'includes/collection_manager_table');
  module_load_include('inc', 'islandora_basic_collection', 'includes/collection_management');
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.api');
  $duracloud_stores = islandora_vault_get_datastores_as_option_array();
  $fedora_stores = islandora_vault_get_datastores_as_option_array('fedora');
  //$table = islandora_collection_table($object);
  //if (!$table) {
  //  $form['no_objects'] = array(
  //    '#type' => 'item',
  //    '#title' => t('No objects found in this collection'),
  //  );
  //  return $form;
  //}
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  $task_output = "";
  $cloudsync_json = NULL;
  if (!empty($json)) {
    $decode = json_decode($json->content, TRUE);
    $task = islandora_vault_identifier_from_uri($decode['task']['uri']);
    module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.api');
    try {
      $iv = IslandoraVault::getInstance();
      $cloudsync_json = $iv->getTask($task);
    } catch (Exception $e) {
      drupal_set_message(t('Error loading task, %s CLOUDSYNC datastream and cloudsync task maybe out of sync %t ', array('%s' => $object->id, '%t' => $e->getMessage())), 'error');
      return $form;
    }
    $task_output = islandora_vault_create_task_output($cloudsync_json);
  }

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

  $form['vault_manager']['run']['form'] = drupal_get_form('islandora_vault_run_cloudsync_task_form', $object, $task_output, $cloudsync_json);


  $form['vault_manager']['create'] = array(
    '#title' => t('Create Cloudsync Task'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['vault_manager']['create']['form'] = drupal_get_form('islandora_vault_create_cloudsync_task_form', $object, $task_output, $fedora_stores, $duracloud_stores);


  $form['vault_manager']['delete'] = array(
    '#title' => t('Delete Cloudsync Task'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['vault_manager']['delete']['form'] = drupal_get_form('islandora_vault_delete_cloudsync_task_form', $object, $task_output,
      $cloudsync_json);



  $form['vault_manager']['cloudsync_reports'] = array(
    '#title' => t('Cloudsync Reports'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['vault_manager']['cloudsync_reports']['form'] = drupal_get_form('islandora_vault_cs_report_form', $object, $task_output);


  $form['vault_manager']['duracloud_reports'] = array(
    '#title' => t('Duracloud Reports'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );


  return $form;
}

function islandora_vault_cs_report_form($form, &$form_state, $object, $task_output){
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
}

function islandora_vault_delete_cloudsync_task_form($form, &$form_state, $object, $task_output, $cloudsync_json) {
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  else {
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
    '#description' => t("<strong>This will delete this task, the corresponding set and all logs associated with this task.</strong><br />"
        . $task_output),
  );

  $form['delete']['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );
  $decoded_cloudsync = json_decode($cloudsync_json, TRUE);
  $disabled = FALSE;
  if ($decoded_cloudsync['task']['state'] == 'Starting') {
    $disabled = TRUE;
  }

  $form['delete']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete this task'),
    '#suffix' => t('<br /><em>This will delete this task, the corresponding set and all cloudsync logs associated with this task.  This will not delete
      any fedora data or any items already copied to Duracloud</em>'),
    '#disabled' => $disabled,
  );

  return $form;
  
}

function islandora_vault_delete_cloudsync_task_form_submit($form, &$form_state){
  $object = islandora_object_load($form_state['values']['pid']);
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  else {
    drupal_set_message(t('Error loading task for %s', array('%s' => $object->id)),'error');
    return $form;
  }
  $decode = json_decode($json->content, TRUE);
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.service');
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $task = islandora_vault_identifier_from_uri($decode['task']['uri']);
  $associated_logs = islandora_vault_get_logs_by_task($task);
  islandora_vault_delete_logs($associated_logs);
  
}

function islandora_vault_run_cloudsync_task_form($form, &$form_state, $object, $task_output, $cloudsync_json) {
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $form = array();
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

  $form['run']['titlebox'] = array(
    '#type' => 'item',
    '#title' => t("Cloudsync Task for @collection_pid", array('@collection_pid' => $object->id)),
    '#description' => t("<strong>This task will run in the background.  When this task is completed it's state will be reported as idle.</strong><br />"
        . $task_output),
  );

  $form['run']['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );
  $decoded_cloudsync = json_decode($cloudsync_json, TRUE);
  $disabled = FALSE;
  if ($decoded_cloudsync['task']['state'] == 'Starting') {
    $disabled = TRUE;
  }

  $form['run']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Run task'),
    '#disabled' => $disabled,
  );

  return $form;
}

function islandora_vault_run_cloudsync_task_form_submit($form, &$form_state) {
  
  $object = islandora_object_load($form_state['values']['pid']);
  if (isset($object['CLOUDSYNC'])) {
    $json = $object->getDatastream('CLOUDSYNC');
  }
  else {
    form_set_error(NULL, t('Error loading task for %s', array('%s' => $object->id)));
    return $form;
  }
  $decode = json_decode($json->content, TRUE);
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.service');
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $task = islandora_vault_identifier_from_uri($decode['task']['uri']);
  islandora_vault_start_task($task);
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
or \$object <fedora-rels-ext:isMemberOf> <info:fedora/$object->id>)            
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
    '#title' => t("Create a Cloudsync task for @collection_pid", array('@collection_pid' => $object->id)),
  );

  $form['migrate']['toDatastore'] = array(
    '#title' => t('Choose a Duracloud datastore (copy to)'),
    '#description' => t("All chosen content will be replicated to the selected datastore"),
    '#type' => 'select',
    '#options' => $duracloud_stores,
  );

  $form['migrate']['fromDatastore'] = array(
    '#title' => t('Choose a Fedora datastore (copy from)'),
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
    '#value' => t('Create task'),
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
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.service');
  //$pids = array_filter($form_state['values']['table']);
  $current = $form_state['values']['current'];
  
  try {
    $object = islandora_object_load($current);
  } catch (Exception $e) {
    drupal_set_message(t('error getting object for %s', array('%s' => $current)), 'error');
    return "";
  }
  $iv = IslandoraVault::getInstance();
  $label = t('ISLANDORA MANAGED Set for object - !t - !p', array('!t' => $object->label, '!p' => $current));
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
  $label = t('ISLANDORA MANAGED Task for object - !t - !p', array('!t' => $object->label, '!p' => $current));
  //CREATE A COPY TASK
  $type = 'copy';
  $data['setUri'] = $set['objectSet']['uri'];
  $data['queryStoreUri'] = $form_state['values']['fromDatastore'];
  $data['sourceStoreUri'] = $form_state['values']['fromDatastore'];
  $data['destStoreUri'] = $form_state['values']['toDatastore'];
  $data['overwrite'] = 'true';
  $data['includeManaged'] = 'true';
  try {
    $json = islandora_vault_create_task($label, $data, $type);
  } catch (Exception $e) {
    drupal_set_message(t('error creating cloudsync task for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  drupal_set_message(t('Created ') . $label);
  try {
    $object->repository->api->m->addDatastream($current, 'CLOUDSYNC', 'string', $json, array('controlGroup' => 'M', 'dsLabel' => 'CLOUDSYNC'));
  } catch (Exception $e) {
    drupal_set_message(t('error updating CLOUDSYNC datastream for %s %message', array('%s' => $current, '%message' => $e->getMessage())), 'error');
    return '';
  }
  drupal_set_message(t('Added Cloudsync datastream to %s ', array('%s' => $current)));
}

/**
 * return a list of duracloud stores
 * @return type 
 */
function islandora_vault_get_datastores_as_option_array($type = 'duracloud') {
  module_load_include('inc', 'islandora_vault', '/includes/islandora_vault.api.inc');
  $iv = IslandoraVault::getInstance();
  $json = $iv->listObjectStores();
  $stores = json_decode($json, TRUE);
  $options = array();
  foreach ($stores['objectStores'] as $store) {
    if ($store['type'] == $type) {
      $options[$store['uri']] = $store['name'];
    }
  }
  return $options;
}