<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

function session_init() {
  session_start();
}
function session_set($key, $value) {
  $_SESSION[$key] = $value;
}
function session_get($key)
{
  if(isset($_SESSION[$key]))
    return $_SESSION[$key];
  return FALSE;
}