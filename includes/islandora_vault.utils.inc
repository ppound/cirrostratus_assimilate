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
 * @param string $query
 * @return string 
 *   line seperated list of pids
 */
function islandora_vault_get_pid_list(&$object, $query, $type = 'itql') {
  $results = $object->repository->ri->query($query, $type, 'unlimited', '0');
  $pid_list = "";
  foreach ($results as $result) {
    $pid_list .= $result['object']['value'] . "\n";
  }
  return $pid_list;
}

function islandora_vault_get_duracloud_item_properties($duracloud_item) {
  $dc = DuracloudVault::getInstance();
  $output = $dc->getItemProperties($duracloud_item);
  return islandora_vault_http_parse_headers($output);
}

/**
 * replacement for pecl parse headers 
 * taken from php.net
 * @param string $header
 * @return array 
 */
function islandora_vault_http_parse_headers( $header )
    {
        $retVal = array();
        $fields = explode("\r\n", preg_replace('/\x0D\x0A[\x09\x20]+/', ' ', $header));
        foreach( $fields as $field ) {
            if( preg_match('/([^:]+): (.+)/m', $field, $match) ) {
                $match[1] = preg_replace('/(?<=^|[\x09\x20\x2D])./e', 'strtoupper("\0")', strtolower(trim($match[1])));
                if( isset($retVal[$match[1]]) ) {
                    $retVal[$match[1]] = array($retVal[$match[1]], $match[2]);
                } else {
                    $retVal[$match[1]] = trim($match[2]);
                }
            }
        }
        return $retVal;
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
  if (empty($logs)) {
    return t('No logs found for this %s task', array('%s' => $task));
  }
  $rows = array();
  $variables = array();
  $row = 0;
  foreach ($logs as $log) {
    $log_id = islandora_vault_identifier_from_uri($log['uri']);
    $link = l($log['resultType'], 'islandora_vault/' . $log_id . '/manage/vault/logcontent/');
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

