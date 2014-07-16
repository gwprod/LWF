<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

function conf_init()
{
  global $conf;
  $conf = conf_defaults();
  $conf_settings =  conf_settings();
  $conf = array_merge($conf, $conf_settings);
}
function conf_settings()
{
  $conf = array();
  if(file_exists(SITE_ROOT . '/settings.php')) {
    require_once SITE_ROOT . '/settings.php';
  }
  return $conf;
}
function conf_get($key)
{
  global $conf;
  return $conf[$key];
}
function conf_set($key, $value)
{
  global $conf;
  $conf[$key] = $value;
}

function conf_defaults()
{
  $defaults = array(
    'user' => array(
      'class' => 'BaseUser',
    ),
  );
  return $defaults;
}