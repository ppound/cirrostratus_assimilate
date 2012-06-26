<?php

/**
 *
 * @param string $uri
 * @return string 
 *  returns the identifier portion of the uri
 */
function islandora_vault_identifier_from_uri($uri){
  $start = strrchr($uri,'/');
  return substr($start,1);
  }

/**
 * 
 * @param string $json
 *   
 * @return string
 *  returns a string of rendered html 
 */
function islandora_vault_create_task_output($json) {
  $decoded = json_decode($json, TRUE);
  /* data available
    $name = $decoded['task']['name'];
    $data = $decoded['task']['data'];
    $state = $decoded['task']['state'];
    $type = $decoded['task']['type'];
    $uri = $decoded['task']['uri'];
    $decoded_data = json_decode($data);
    $queryType = $decoded_data['queryType'];
    $queryText = $decoded_data['queryText'];
    $overwrite = $decoded_data['overwrite'];
    $sourceStore = $decoded_data['sourceStoreUri'];
    $includeManaged = $decoded_data['includeManaged'];
    $setUri = $decoded_data['setUri'];
    $destStore = $decoded_data['destStoreUri'];
    $queryStore = $decoded_data['queryStoreUri']; */
  $rows = array();
  $variables = array();
  $row = 0;
  foreach ($decoded['task'] as $key => $value) {
    if ($key == 'data') {
      $decoded_data = json_decode($value);
      foreach ($decoded_data as $k => $v) {
        $rows[$row++] = array($k, $v);
      }
    }
    else {
      $rows[$row++] = array($key, $value);
    }
  }
  $variables['rows'] = $rows;
  $variables['header'] = array('key', 'value');
  $variables['attributes'] = array();
  $variables['caption'] = 'Task Data';
  $variables['colgroups'] = array();
  $variables['sticky'] = FALSE;
  $variables['empty'] = "Could not parse task json";
  return theme_table($variables);
}


