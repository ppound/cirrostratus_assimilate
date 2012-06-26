<?php

//create an islsndoraVault api object 

function islandora_vault_service() {
  $iv = IslandoraVault::getInstance();
//echo $iv->getServiceInfo(); 
//echo $iv->listObjectSets();
//echo $iv->getDuracloudAccounts();
//echo $iv->getDuracloudSpaces();
//echo $iv->getObjectSet('4');
//echo $iv->listObjectStores();
//echo $iv->getObjectStore('2');
//CREATE A SET
  /*
    $data['queryType'] = 'iTQL';
    $data['queryText'] = 'select $object from <#ri> where $object <fedora-model:hasModel> <info:fedora/islandora:bookCModel>';
    echo islandora_vault_create_set('list all books by cmodel',$data);
   */

  //CREATE A PIDLIST SET
 // $data = "ilsandora:268\nislandora:269\nislandora:270";
  //echo islandora_vault_create_set('list all books by cmodel4', $data, 'pidList');
  //CREATE A LIST TASK
  /*
    $name = 'list sandbox books9';
    $type = 'list';
    $data['setUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectSets/7';
    $data['storeUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/17';
    $data['state'] = 'Idle';
    echo islandora_vault_create_task($name, $data, $type);
   */

  //CREATE A COPY TASK
  /*
    $name = 'copy sandbox books to Duracloud upei-dev';
    $type = 'copy';
    $data['setUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectSets/7';
    $data['queryStoreUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/17';
    $data['sourceStoreUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/17';
    $data['destStoreUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/3';

    $data['overwrite'] = 'true';
    $data['includeManaged'] = 'true';
    echo islandora_vault_create_task($name,$data,$type);
   */

//RUN A TASK
  //echo islandora_vault_start_task('29');
  //CREATE A STORE
  /*
    $data['username'] = 'fedoraAdmin';
    $data['password'] = 'islandora';
    $data['url'] = 'http://sandbox.islandora.ca:8080/fedora';
    $name = 'islandora Fedora sandbox';
    $type = 'fedora';
    echo islandora_vault_create_object_store($name, $data, $type = 'fedora');
   */

//echo $iv->listTaskLogs();
//echo $iv->listTaskLogs();
//echo $iv->deleteTaskLog('2');
//echo $iv->listTasks();
//echo $iv->getUsers;
  exit();
}

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

function islandora_vault_get_logs_by_task($task){
  module_load_include('inc','islandora_vault', 'includes/islandora_vault.utils');
  $iv = IslandoraVault::getInstance();
  $taskLogs = $iv->listTaskLogs();
  $json_array = json_decode($taskLogs, TRUE);
  $associated_logs = array();
  foreach($json_array['taskLogs'] as $log_entry){    
    if(islandora_vault_identifier_from_uri($log_entry['taskUri']) == $task){
      $associated_logs[] = $log_entry;
    }
  }
  return $associated_logs;   
}

/**
 *
 * @param array $logs 
 */
function islandora_vault_delete_logs($logs){
  module_load_include('inc','islandora_vault', 'includes/islandora_vault.utils');
  $iv = IslandoraVault::getInstance();
  foreach($logs as $log){
    $log_number = islandora_vault_identifier_from_uri($log['uri']);
    $iv->deleteTaskLog($log_number);
  }
}