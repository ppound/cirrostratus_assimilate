<?php

/**
 * Sends a patch request to change state to Starting
 * which invokes the task
 * @param string $task
 *   the task to start
 * @return type 
 * @throws HttpConnectionExcption
 */
function islandora_vault_start_task($task) {
  $iv = IslandoraVault::getInstance();
  $data['task']['state'] = 'Starting';
  return $iv->updateTask($task, json_encode($data));
}

/**
 *
 * @param string $set_id
 * @param array $data
 * @return string
 */
function islandora_vault_update_set($set_id, $updated_set){
  $iv = IslandoraVault::getInstance();
  $updated_set = json_encode($updated_set);
  return $iv->updateObjectSet($set_id, $updated_set);
}

/**
 * create a json string and send it the api
 * string should look similar to this
 * {"task":{"name":"Copy islandora:268 from Fedora Repository at 
 * 192.168.56.195:8080 to DuraCloud Space upei-dev at dgi.duracloud.org (AMAZON_S3)",
 * "type":"copy","state":"Idle","data":"{\"setUri\":
 * \"http://localhost:8080/fedoracloudsync/api/rest/objectSets/4\",
 * \"queryStoreUri\":\"http://localhost:8080/fedoracloudsync/api/rest/objectStores/2\",
 * \"sourceStoreUri\":\"http://localhost:8080/fedoracloudsync/api/rest/objectStores/2\",
 * \"destStoreUri\":\"http://localhost:8080/fedoracloudsync/api/rest/objectStores/3\",\
 * "overwrite\":\"true\",\"includeManaged\":\"true\"}"}}
 * 
 * We create the string by creating an array and passing it through json_encode
 * @param string $name
 * @param array $data
 *   example:
 *   $data['setUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectSets/6';
 *   $data['storeUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/2';
 * @param string state
 *    valid values for state (Starting, Idle)
 * @param string $type
 *   list or copy
 * @return string
 *   json encode string
 * @throws Exception 
 */
function islandora_vault_create_task($name, $data, $type = 'list', $state = 'Idle') {
  $iv = IslandoraVault::getInstance();
  if ($type == 'list' && (empty($data['setUri']) || empty($data['storeUri']))) {
    throw new Exception('invalid data for list task creation');
  }
  if ($type == 'copy' && (empty($data['queryStoreUri']) || empty($data['sourceStoreUri'])
      || empty($data['destStoreUri']) || empty($data['overwrite']) || empty($data['includeManaged']))) {
    throw new Exception('invalid data for copy task creation');
  }

  $data_string = json_encode($data);
  if (!$data_string) {
    throw new Exception('Could not encode task data as json');
  }
  $task['task']['name'] = $name;
  $task['task']['type'] = $type;
  $task['task']['state'] = $state;
  $task['task']['data'] = $data_string;

  return $iv->createTask(json_encode($task));
}

/**
 *
 * @param string $name
 *    name of the objectStore
 * @param string $type
 *   either "fedora" or "duracloud"
 * @param array $data
 *   $data['username'] = 'ausername'
 *   $data['password'] = 'apassword'
 *   $data['url'] = 'aurl'
 * @return type
 * @throws Exception 
 */
function islandora_vault_create_object_store($name, $data, $type = 'fedora') {
  $iv = IslandoraVault::getInstance();
  $url = $data['url'];
  $username = $data['username'];
  $password = $data['password'];
  if (empty($data) || empty($username) || empty($password) || empty($url)) {
    throw new Exception('invalid data for objectStore creation');
  }
  $data_string = json_encode($data);
  if (!$datastring) {
    throw new Exception('Could not encode store data as json');
  }
  //$data_string = '{"url":"' . $url . '","username":"' . $username . '","password":"' . $password . '"}';
  $objectStore['objectStore']['name'] = $name;
  $objectStore['objectStore']['type'] = $type;
  $objectStore['objectStore']['data'] = $data_string;
  return $iv->createObjectStore(json_encode($objectStore));
}

/**
 * Create a new set in cloudsync.  
 * @param string $name
 *  a name for the object set
 * @param string $type
 *  query, pidList or pid pattern 
 *  this function currently only supports query as a type
 * @param mixed $data
 *  $data['queryType'] = 'iTQL'
 *  $data['queryText'] = 'select $object from <#ri>
  where $object <fedora-model:hasModel> <info:fedora/islandora:bookCModel>'
 *  for pidLists $data should be a string consisting of a list of pids one per line
 * @return type
 * @throws Exception 
 */
function islandora_vault_create_set($name, $data, $type = 'query') {
  $iv = IslandoraVault::getInstance();
  if ($type == 'query' && (empty($data['queryType']) || empty($data['queryText']))) {
    throw new Exception('invalid data for query objectSet creation');
  }
  if (empty($data)) {
    throw new Exception('invalid data for pidlist objectSet creation');
  }
  if (is_string($data)) {
    $data_string = $data;
  }
  else {
    $data_string = json_encode($data);
  }
  if (!$data_string) {
    throw new Exception('Could not encode set data as json');
  }
  //$data_string = '{"queryType":"' . $queryType . '","queryText":"' . $queryText . '"}';
  $objectSet['objectSet']['name'] = $name;
  $objectSet['objectSet']['type'] = $type;
  $objectSet['objectSet']['data'] = $data_string;
  return $iv->createObjectSet(json_encode($objectSet));
}

/**
 * returns the contents of a log
 * @param string $log
 *   log number
 * @return string 
 *   content of the log
 */
function islandora_vault_cloudsync_log_content($log) {
  $iv = IslandoraVault::getInstance();
  try {
    $content = $iv->getTaskLog($log);
  } catch (Exception $e) {
    return 'Error loading log content';
  }
  return '<pre>' . $content . '</pre>';
}

/**
 * gets all log entries associated with a task
 * @param string $task
 * @return array
 */
function islandora_vault_get_logs_by_task($task) {
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $iv = IslandoraVault::getInstance();
  $taskLogs = $iv->listTaskLogs();
  $json_array = json_decode($taskLogs, TRUE);
  $associated_logs = array();
  foreach ($json_array['taskLogs'] as $log_entry) {
    if (islandora_vault_identifier_from_uri($log_entry['taskUri']) == $task) {
      $associated_logs[] = $log_entry;
    }
  }
  return $associated_logs;
}

/**
 * deletes a task and its associated set.
 * @param string $task 
 */
function islandora_vault_delete_task_and_set($task) {
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $iv = IslandoraVault::getInstance();
  $json_task = $iv->getTask($task);
  $json_array = json_decode($json_task, TRUE);
  $data = $json_array['task']['data'];
  $data_array = json_decode($data, TRUE); //data is nested as a string in the original array
  $setUri = $data_array['setUri']; //we need to get the setUri from the task before we can delete it
  $set = islandora_vault_identifier_from_uri($setUri);
  $iv->deleteTask($task);
  $iv->deleteObjectSet($set);
}

/**
 * deletes everything related to a task
 * all logs, the task and associated set
 * @param string $task 
 */
function islandora_vault_delete_all($task) {
  $associated_logs = islandora_vault_get_logs_by_task($task);
  islandora_vault_delete_logs($associated_logs);
  islandora_vault_delete_task_and_set($task);
}

function islandora_vault_create_restore_task($query){
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
}

/**
 *
 * @param array $task
 * @param boolean $data_only
 * @return array
 *   an array containing either the whole set or just the data portion
 *   
 */
function islandora_vault_get_set_by_task($task, $data_only = FALSE){
  $data_string = $task['data'];
  $data_array = json_decode($data_string, TRUE);
  $set_uri = $data_array['setUri'];
  $set_id = islandora_vault_identifier_from_uri($set_uri);
  $iv = $iv = IslandoraVault::getInstance();
  $set = $iv->getObjectSet($set_id);
  $set_array = json_decode($set, TRUE);
  if(!$data_only){
    return $set_array;
  }
  $data = $set_array['objectSet']['data'];
  return json_decode($data, TRUE);
  
  
}

/**
 * deletes log entries
 * @param array $logs 
 *   json decode array
 */
function islandora_vault_delete_logs($logs) {
  module_load_include('inc', 'islandora_vault', 'includes/islandora_vault.utils');
  $iv = IslandoraVault::getInstance();
  foreach ($logs as $log) {
    $log_number = islandora_vault_identifier_from_uri($log['uri']);
    $iv->deleteTaskLog($log_number);
  }
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

