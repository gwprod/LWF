<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

abstract class ORMObject
{
  protected $properties = array();
  protected $schema = array();
  protected $base_table = '';
  public function __set($name, $value)
  {
    self::$properties[$name] = $value;
  }
  public function __get($name)
  {
    if(isset(self::$properties[$name]))
      return self::$properties[$name];
  }
  abstract protected function schema();
  
  protected function buildTable()
  {
    $schema = self::schema();
    $base_table = self::$base_table;
    if(Database::table_exists($base_table))
    {
      foreach($schema as $column_name => $column)
      {
        if(!Database::field_exists($base_table, $column_name))
        {
          
        }
      }
    }
  }
}