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
function islandora_vault_manage_object_form($form, &$form_state, $object) {
  module_load_include('inc', 'islandora_basic_collection', 'includes/collection_manager_table');
  module_load_include('inc', 'islandora_basic_collection', 'includes/collection_management');
  $potential_collections = islandora_vault_get_datastores_as_option_array();
  $table = islandora_collection_table($object);
  if (!$table) {
    $form['no_objects'] = array(
      '#type' => 'item',
      '#title' => t('No objects found in this collection'),
    );
    return $form;
  }
  $form = array();

  $form['migrate']['titlebox'] = array(
    '#type' => 'item',
    '#title' => t("Manage objects from @collection_pid", array('@collection_pid' => $object->id)),
  );  

  $form['migrate']['datastore'] = array(
    '#title' => t('Choose a datastore'),
    '#description' => t("All chosen content will be replicated to the selected datastore"),
    '#type' => 'select',
    '#options' => $potential_collections,
  );

  $form['migrate']['table'] = $table;

  $form['current'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $form['migrate']['message'] = array(
    '#type' => 'item',
    '#value' => t(""),
  );

  $form['migrate']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Migrate selected objects'),
  );
  return $form;
}

/**
 * Submit handler for object migration form
 * 
 * @param array $form
 * @param array $form_state 
 */
function islandora_vault_manage_object_form_submit($form, &$form_state) {
  return 'success';
}

function islandora_vault_get_datastores_as_option_array(){
  module_load_include('inc','islandora_vault','/includes/islandora_vault.api.inc');
  $iv = new IslandoraVault();
  $json = $iv->listObjectStores();
  $stores = json_decode($json, TRUE);
  $options = array();
  foreach($stores['objectStores'] as $store){
    if($store['type'] == 'duracloud'){
      $options[$store['uri']] = $store['name'];
    }
  }
  return $options;
}