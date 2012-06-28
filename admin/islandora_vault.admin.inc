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
    '#description' => t('The URL to use when connecting to Duracloud spaces'),
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
    '#required' => TRUE,
  );
 
  
  
  return system_settings_form($form);
}
