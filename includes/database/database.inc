<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
define('DATABASE::BUILD_OBJECT', 000000001);

class Database {

  private static $db;

  /**
   * Database constructor.
   * 
   * @global array $conf
   *   The configuration array.
   * @return PDO
   *   The PHP Data Objects instance.
   */
  public function __construct() {

    global $conf;
    $db_conf = $conf['database'];
    if (!self::$db) {
      try {
        $db = new PDO($db_conf['dsn'], $db_conf['username'], $db_conf['password'], $db_conf['options']);
      }
      catch (PDOException $e) {
        die("PDO CONNECTION ERROR: " . $e->getMessage() . "<br/>");
      }
    }
    return self;
  }

  /**
   * Inner db_query for Database object.
   * 
   * @param string $query_string
   *   The query string.
   * @param array $query_parameters
   *   An array of key => value pairs denoting bind params.
   * 
   * @return object
   *   A resultset object.
   */
  public static function db_query($query_string, $query_parameters = array()) {

    $db = Database::getInstance()->getDB();
    $stmt = $db->prepare($query_string);
    foreach ($query_parameters as $parameter_key => $parameter_value) {

      $stmt->bindParam($parameter_key, $parameter_value);
    }
    $stmt->execute();
    return $stmt;
  }

  public static function table_exists($table_name) {
    return Database::db_query('SELECT 1 FROM information_schema.tables WHERE table_name = :table_name', array(':table_name' => $table_name))->fetchField();
  }

  public static function field_exists($table_name, $field_name) {

    return Database::db_query('SELECT 1 FROM information_schema.columns WHERE table_name = :table_name and column_name = :column_name', array(':table_name' => $table_name, ':column_name' => $field_name))->fetchField();
  }
  public static function field_create($table_name, $field_definition)
  {
    Database::db_query('ALTER table :table ADD ')
  }

  /**
   * Get the instance of the Database object.
   * 
   * @return \Database
   */
  public static function getInstance() {
    return new Database();
  }

  private function getDB() {

    return self::$db;
  }

}

/**
 * Queries the database.
 * 
 * @param string $query
 *   The query string.
 * @param array $query_parameters
 *   An array of key => value pairs denoting bind parameters.
 * @return object
 *   A resultset object.
 */
function db_query($query, array $query_parameters = array()) {
  $stmt = Database::db_query($query, $query_parameters);

  return $stmt;
}

function db_select($base_table) {
  return new SelectQuery($base_table);
}

class SelectQuery {

  private $base_table;
  private $fields = array();
  private $conditions = array();
  private $join_tables = array();

  public function __construct($base_table) {
    self::$base_table = $base_table;
  }

  public function buildQuery() {
    $query = 'select :fields from :base_table :conditions';
    $fields = array();
    if (isset(self::$fields)) {
      foreach (self::$fields as $table => $table_fields) {
        foreach ($table_fields as $table_field) {
          $fields[] = $table . '.' . $table_field;
        }
      }
    }
    if (!empty($fields)) {
      $field_query = implode(',', $fields);
    }
    else {
      $field_query = '*';
    }
    $conditons = array();
    $placeholder_count = 0;
    if (isset(self::$conditions)) {
      foreach (self::$conditions as $key => $condition) {
        if (is_object($condition) && get_class($condition) == 'QueryCondition') {
          $query_condition = $condition->buildQuery($placeholder_count);
          $conditions[$key] = '(' . $query_condition['query'] . ')';
          $params += $query_condition['params'];
        }
        else {
          $conditions[$key] = $condition['key'] . ' ' . $condition['type'] . ' :placeholder_' . $placeholder_count;
          $params[':placeholder_' . $placeholder_count];
        }
        $placeholder_count++;
      }
    }
    if (!empty($conditions)) {
      $condition_query = 'WHERE ' . implode(' AND ', $conditions);
    }
    else {
      $condition_query = '';
    }
    $query = str_replace(':fields', $field_query, $query);
    $query = str_replace(':base_table', self::$base_table, $query);
    $query = str_replace(':conditions', $condition_query, $query);
    return array('query' => $query, 'params' => $params);
  }

  public function execute($options = null) {
    $query_data = self::buildQuery();
    $db = Database::getInstance();
    $stmt = $db->prepare($query_data['query']);
    foreach ($query_data['params'] as $key => $condition) {
      $stmt->bindValue($key, $condition['value']);
    }
    $stmt->execute();
    $result = $stmt->fetchAll()
    return $result;
  }

  public function condition($conditon, $value = null, $type = null) {
    if (is_string($condition) && isset($value)) {
      self::$conditions[] = array(
        'key' => $condition,
        'value' => $value,
        'type' => isset($type) ? $type : '='
      );
    }
    elseif (is_object($condition) && get_class($condition) == 'QueryCondition') {
      self::$conditions[] = $condition;
    }
    return self;
  }

  public function fields($table, $fields = array()) {
    if (self::$base_table == $table) {
      self::$fields[$base_table][] += $fields;
    }
    elseif (isset(self::$join_tables[$table])) {
      self::$fields[$table][] += $fields;
    }
    return self;
  }

}

function db_and() {
  return new QueryCondition('AND');
}

function db_or() {
  return new QueryCondition('OR');
}

class QueryCondition {

  private $type = 'AND';
  private $conditions = array();

  public function __construct($type) {
    self::$type = $type;
  }

  public function condition($conditon, $value = null, $type = null) {
    if (is_string($condition) && isset($value)) {
      self::$conditions[] = array(
        'key' => $condition,
        'value' => $value,
        'type' => isset($type) ? $type : '='
      );
    }
    elseif (is_object($condition) && get_class($condition) == 'QueryCondition') {
      self::$conditions[] = $condition;
    }
  }

  public function buildQuery(&$placeholder_count) {
    $conditions = array();
    $params = array();
    foreach (self::$conditions as $key => $condition) {
      if (is_object($condition) && get_class($condition) == 'QueryCondition') {
        $data = $condition->buildQuery($placeholder_count);
        $conditions[] = '(' . $data['query'] . ')';
        $params += $data['params'];
      }
      else {
        $conditions[] = $condition['key'] . ' ' . $condition['type'] . ' :placeholder_' . $placeholder_count;
        $params[':placeholder_' . $placeholder_count] = $condition['value'];
      }
      $placeholder_count++;
    }
    $query = implode(' ' . self::$type . ' ', $conditions);
    return array(
      'query' => $query,
      'params' => $params,
    );
  }

}
