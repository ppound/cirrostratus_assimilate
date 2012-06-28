<?php

/**
 *
 * @param string $uri
 * @return string 
 *  returns the identifier portion of the uri
 */
function islandora_vault_identifier_from_uri($uri) {
  $start = strrchr($uri, '/');
  return substr($start, 1);
}

/**
 * 
 * @param string $json
 *   
 * @return string
 *  returns a string of rendered html 
 */
function islandora_vault_create_task_output($caption, $json) {
  $decoded = json_decode($json, TRUE);
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
  $variables['caption'] = $caption;
  $variables['colgroups'] = array();
  $variables['sticky'] = FALSE;
  $variables['empty'] = "Could not parse task json";
  return theme_table($variables);
}

/**
 *
 * @param array $logs
 * @return string 
 */
function islandora_vault_print_logs($task, $logs) {
  $items = array();
  if(empty($logs)){
    return t('No logs found for this %s task', array('%s' => $task));
  }
  $rows = array();
  $variables = array();
  $row = 0;
  foreach ($logs as $log) {
    $log_id = islandora_vault_identifier_from_uri($log['uri']);   
    $link = l($log['resultType'],'islandora_vault/'.$log_id.'/manage/vault/logcontent/');    
    $finished_date = (empty($log['finishDate'])) ? 'incomplete' : $log['finishDate'];
    $rows[$row++] = array($link, $log['startDate'], $finished_date);
  }
  $variables['rows'] = $rows;
  $variables['header'] = array('result', 'started', 'finished');
  $variables['attributes'] = array();
  $variables['caption'] = $task;
  $variables['colgroups'] = array();
  $variables['sticky'] = FALSE;
  $variables['empty'] = "Could not parse logs";
  return theme_table($variables);
}

