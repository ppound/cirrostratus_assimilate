<?php


/**
 * @file islandora_vault.admin.inc
 * 
 * islandora_vault admin form
 * @param array $form
 * @param array $form_state 
 * 
 * @return array
 */

function islandora_vault_admin($form, &$form_state) {
  //$form = array();
  $form['islandora_vault_duracloud_space'] = array(
    '#type' => 'textfield',
    '#title' => t('Duracloud space'),
    '#default_value' => variable_get('islandora_vault_duracloud_space', 'myspace'),
    '#description' => t('The Duracloud space that where you will copy items to/from'),
    '#required' => TRUE,
  );
  
  $form['islandora_vault_cloudsync_url'] = array(
    '#type' => 'textfield',
    '#title' => t('CloudSync Url'),
    '#default_value' => variable_get('islandora_vault_cloudsync_url', 'https://ipaddres/cloudsync/api/rest/'),
    '#description' => t('The URL to use when connecting to cloudsync'),
    '#required' => TRUE,
  );
  
  $form['islandora_vault_cloudsync_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Cloudsync User'),
    '#default_value' => variable_get('islandora_vault_cloudsync_user', 'username'),
    '#description' => t('The username to use when connecting to Duracloud'),
    '#required' => TRUE,
  );
 
  $form['islandora_vault_cloudsync_password'] = array(
    '#type' => 'password_confirm',
    '#title' => t('Cloudsync password'),
    '#required' => FALSE,
  );
  
  $form ['islandora_vault_duracloud_store'] = array(
    '#type' => 'textfield',
    '#maxlength' => '255',
    '#size' => '80',
    '#required' => TRUE,
    '#title' => t('Duracloud store url'),
    '#description' => t('The url for the Duracloud store.  This url will be provided by your Cloudsync administrator.'),
    '#default_value' => variable_get('islandora_vault_duracloud_store','http://localhost:8080/cloudsync/api/rest/objectStores/id'),    
  );
  
  $form ['islandora_vault_fedora_store'] = array(
    '#type' => 'textfield',
    '#maxlength' => '255',
    '#size' => '80',
    '#required' => TRUE,
    '#title' => t('Fedora store url'),
    '#description' => t('The url for the Fedora store.  This url will be provided by your Cloudsync administrator.'),
    '#default_value' => variable_get('islandora_vault_fedora_store','http://localhost:8080/cloudsync/api/rest/objectStores/id'),    
  );
  
  $form ['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
 // don't use system settings form so we can encrypt passwords (we need more control over loading and saving the passwords
  return $form;
}

function islandora_vault_admin_validate($form, &$form_state){
  
}
function islandora_vault_admin_submit($form, &$form_state) {
  variable_set('islandora_vault_duracloud_space', $form_state['values']['islandora_vault_duracloud_space']); 
  variable_set('islandora_vault_cloudsync_url', $form_state['values']['islandora_vault_cloudsync_url']); 
  variable_set('islandora_vault_cloudsync_user', $form_state['values']['islandora_vault_cloudsync_user']); 
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $password = $form_state['values']['islandora_vault_cloudsync_password'];
  if(!empty($password)){
    $encrypted_password = islandora_vault_encrypt_password($password);
    variable_set('islandora_vault_cloudsync_password', $encrypted_password); 
  }
  variable_set('islandora_vault_duracloud_store', $form_state['values']['islandora_vault_duracloud_store']); 
  variable_set('islandora_vault_fedora_store', $form_state['values']['islandora_vault_fedora_store']);
}