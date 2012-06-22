<?php

//create an islsndoraVault api object 

function islandora_vault_service() {
  $iv = new IslandoraVault();
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
  //CREATE A LIST TASK
/*
  $name = 'list sandbox books3';
  $type = 'list';
  $data['setUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectSets/7';
  $data['storeUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/17';
  $data['state'] = 'Idle';
  echo islandora_vault_create_task($name, $data, $type);
*/

  //CREATE A COPY TASK
  
    $name = 'copy sandbox books to Duracloud upei-dev';
    $type = 'copy';
    $data['setUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectSets/7';
    $data['queryStoreUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/17';
    $data['sourceStoreUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/17';
    $data['destStoreUri'] = 'http://localhost:8080/fedoracloudsync/api/rest/objectStores/3';

    $data['overwrite'] = 'true';
    $data['includeManaged'] = 'true';
    echo islandora_vault_create_task($name,$data,$type);
   

//RUN A TASK
   //echo islandora_vault_start_task('15');
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
//echo $iv->getTasks();
//echo $iv->getUsers;
  exit();
}

/**
 * Sends a patch request to change state to Starting
 * which invokes the task
 * @param string $task
 *   the task to start
 * @return type 
 */
function islandora_vault_start_task($task) {
  $iv = new IslandoraVault();
  $data['task']['state'] = 'Starting';
  return $iv->updateTask($task, json_encode($data));
}

/**
 * create a json string and send it the api
 * string should look like this
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
  $iv = new IslandoraVault();

  if (empty($data['setUri']) || (empty($data['storeUri']) && $type == 'list') || empty($state)) {
    throw new Exception('invalid data for list task creation');
  }
  if ($type == 'copy' && (empty($data['queryStoreUri']) || empty($data['sourceStoreUri'])
      || empty($data['destStoreUri']) || empty($data['overwrite']) || empty($data['includeManaged']))) {
    throw new Exception('invalid data for copy task creation');
  }
  if ($type == 'list') {
    $data_string = '{"setUri":"' . $data['setUri'] . '","storeUri":"' . $data['storeUri'] . '"}';
  }
  else if ($type == 'copy') {
    $data_string = '{"setUri":"' . $data['setUri'] . '","queryStoreUri":"' . $data['queryStoreUri'] . '","sourceStoreUrl":"' .
        $data['sourceStoreUri'] . '","destStoreUri":"' . $data['destStoreUri'] . '","overwrite":"' . $data['overwrite'] .
        '","includeManaged":"' . $data['includeManaged'] . '"}';
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
  $iv = new IslandoraVault();
  $url = $data['url'];
  $username = $data['username'];
  $password = $data['password'];
  if (empty($data) || empty($username) || empty($password) || empty($url)) {
    throw new Exception('invalid data for objectStore creation');
  }
  $data_string = '{"url":"' . $url . '","username":"' . $username . '","password":"' . $password . '"}';
  $objectStore['objectStore']['name'] = $name;
  $objectStore['objectStore']['type'] = $type;
  $objectStore['objectStore']['data'] = $data_string;
  return $iv->createObjectStore(json_encode($objectStore));
}

/**
 *
 * @param string $name
 *  a name for the object set
 * @param string $type
 *  query, pidlist or pid pattern 
 *  this function currently only supports query as a type
 * @param array $data
 *  $data['queryType'] = 'iTQL'
 *  $data['queryText'] = 'select $object from <#ri>
  where $object <fedora-model:hasModel> <info:fedora/islandora:bookCModel>'
 * @return type
 * @throws Exception 
 */
function islandora_vault_create_set($name, $data, $type = 'query') {
  $iv = new IslandoraVault();
  $queryType = $data['queryType'];
  $queryText = $data['queryText'];
  if (empty($queryType) || empty($queryText)) {
    throw new Exception('invalid data for objectSet creation');
  }
  $data_string = '{"queryType":"' . $queryType . '","queryText":"' . $queryText . '"}';
  $objectSet['objectSet']['name'] = $name;
  $objectSet['objectSet']['type'] = $type;
  $objectSet['objectSet']['data'] = $data_string;
  return $iv->createObjectSet(json_encode($objectSet));
}