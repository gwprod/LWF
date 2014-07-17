<?php
require_once 'conf.inc';
require_once 'database/database.inc';
require_once 'database/orm_object.inc';
require_once 'session.inc';
require_once 'user.inc';
function bootstrap_init() {
  conf_init();
  Database::getInstance();
  session_init();
}
/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
function lwf_die($message)
{
  cleanup();
  die($message);
}
function bootstrap_cleanup()
{
  session_cleanup();
}
