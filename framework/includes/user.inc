<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



function user_init() {
  global $user;
  $account = session_get('user');
  if (is_object($account)) {
    $user = $account;
  }
}

class BaseUser extends ORMObject{

  public $is_logged_in = FALSE;
  public $anonymous_user = TRUE;
  public $base_table = 'users';
  protected $properties = array(
    'username' => 'Anonymous User',
    'password' => '',
    'uid' => 0,
  );

  public function __construct() {
    
  }

  public function login($username, $password) {
    
    $query = db_select(self::$base_table);
    $query->fields(self::$base_table, self::$properties);
    $query->conditions(self::$base_table . '.username', $username);
    $query->conditions(self::$base_table . '.password', $password);
    $result = $query->execute();
   
  }
  
}

