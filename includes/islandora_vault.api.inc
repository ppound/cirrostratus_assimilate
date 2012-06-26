<?php

define('OBJECTSET_JSON', "application/vnd.fcrepo-cloudsync.objectset+json");
define("OBJECTSETS_JSON", "application/vnd.fcrepo-cloudsync.objectsets+json");
define("OBJECTSTORE_JSON", "application/vnd.fcrepo-cloudsync.objectstore+json");
define('OBJECTSTORES_JSON', "application/vnd.fcrepo-cloudsync.objectstores+json");
define('PROVIDERACCOUNTS_JSON', "application/vnd.fcrepo-cloudsync.provideraccounts+json");
define('SERVICEINFO_JSON', "application/vnd.fcrepo-cloudsync.serviceinfo+json");
define('SERVICEINIT_JSON', "application/vnd.fcrepo-cloudsync.serviceinit+json");
define('SPACES_JSON', "application/vnd.fcrepo-cloudsync.spaces+json");
define('TASK_JSON', "application/vnd.fcrepo-cloudsync.task+json");
define('TASKLOG_JSON', "application/vnd.fcrepo-cloudsync.tasklog+json");
define('TASKLOGS_JSON', "application/vnd.fcrepo-cloudsync.tasklogs+json");
define('TASKS_JSON', "application/vnd.fcrepo-cloudsync.tasks+json");
define('USER_JSON', "application/vnd.fcrepo-cloudsync.user+json");
define('USERS_JSON', "application/vnd.fcrepo-cloudsync.users+json");

//do this until we expost these in a module or library
$islandora_module_path = drupal_get_path('module', 'islandora');

@include_once 'sites/all/libraries/tuque/HttpConnection.php';
@include_once 'sites/all/libraries/tuque/RepositoryConnection.php';
@include_once "$islandora_module_path/libraries/tuque/HttpConnection.php";
@include_once "$islandora_module_path/libraries/tuque/RepositoryConnection.php";

/**
 * reuse the RepositoryConnection extend in case we need to modify anything
 */
class VaultConnection extends RepositoryConnection {

  /**
   * We override this function to simplify exception as we get more
   * meaningful errors from Duracloud and cloudsync (i think)
   *
   * @param Exception $e
   *   The exception being parsed
   */
  protected function parseFedoraExceptions($e) {
    //$he = new HttpConnectionException()
    throw new HttpConnectionException('Error ' . $e->getCode() . ' ' . $e->getMessage(), $e->getCode(), $e->getResponse(), $e); //$e;//Exception($e['response']['headers']);
  }

}

class IslandoraVault {

  protected $connection = NULL;
  private static $_instance;
  private function __construct() {
    $this->connection = new VaultConnection(variable_get('islandora_vault_cloudsync_url'), variable_get('islandora_vault_cloudsync_user'),
            variable_get('islandora_vault_cloudsync_password'));
  }
  
  static public function getInstance(){
    if(self::$_instance == NULL){
      self::$_instance = new IslandoraVault();
    }
    return self::$_instance;
  }

  function getServiceInfo() {
    $value = $this->connection->getRequest('service');
    return $value['content'];
  }

//sets
  function getObjectSet($set) {
    $set = '/' . $set;
    $value = $this->connection->getRequest('objectSets' . $set);
    return $value['content'];
  }

  function listObjectSets() {
    $value = $this->connection->getRequest('objectSets');
    return $value['content'];
  }

  function deleteObjectSet($set) {
    $set = '/' . $set;
    $value = $this->connection->deleteRequest('objectSets' . $set);
    return $value['content'];
  }

  function createObjectSet($data) {
    $value = $this->connection->postRequest('objectSets', 'string', $data, OBJECTSET_JSON);
    return $value['content'];
  }

//stores
  function getObjectStore($store) {
    $store = '/' . $store;
    $value = $this->connection->getRequest('objectStores' . $store);
    return $value['content'];
  }

  function listObjectStores() {
    $value = $this->connection->getRequest('objectStores');
    return $value['content'];
  }

  function deleteObjectStore($store) {
    $store = '/' . $store;
    $value = $this->connection->deleteRequest('objectStores');
    return $value['content'];
  }

  function createObjectStore($data) {
    $value = $this->connection->postRequest('objectStores', 'string', $data, OBJECTSTORE_JSON);
    return $value['content'];
  }

//duracloud
  function getDuracloudAccounts($account = NULL) {
    $value = $this->connection->getRequest('duracloud/providerAccounts');
    return $value['content'];
  }

  function getDuracloudSpaces($space = NULL) {
    $value = $this->connection->getRequest('duracloud/spaces');
    return $value['content'];
  }

//task logs
  function listTaskLogs() {
    $value = $this->connection->getRequest('taskLogs');
    return $value['content'];
  }

  function getTaskLog($log) {
    $log = '/' . $log . '/content';
    $value = $this->connection->getRequest('taskLogs' . $log);
    return $value['content'];
  }

  function deleteTaskLog($log) {
    $log = '/' . $log;
    $value = $this->connection->deleteRequest('taskLogs' . $log);
    return $value['content'];
  }

//tasks
  function listTasks() {
    $value = $this->connection->getRequest('tasks');
    return $value['content'];
  }

  function getTask($task) {
    $task = '/' . $task;
    $value = $this->connection->getRequest('tasks' . $task);
    return $value['content'];
  }

  function createTask($data) {
    $value = $this->connection->postRequest('tasks', 'string', $data, TASK_JSON);
    return $value['content'];
  }
  
  function updateTask($task, $data){
    $task = '/' . $task;
    $value = $this->connection->patchRequest('tasks' . $task, 'string', $data, TASK_JSON);
    return $value['content'];
  }

//users
  function getUsers() {
    $value = $this->connection->getRequest('users.json');
    return $value['content'];
  }

}

