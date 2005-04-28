<?php
  // ATK Configuration file.
  // This configures the atk backend for use with Achievo.
  // Do *NOT* change this file, ever!
  //
  // If you want to configure Achievo, edit the config.inc.php file.

  $config_identifier = "achievo";
  $config_top_frame = 1;
  $config_authorization = "db";
  $config_auth_usertable = "person";
  $config_auth_userfield = "userid";
  $config_auth_accesstable = "accessright";
  $config_auth_passwordfield = "password";
  $config_auth_leveltable  = "person";
  $config_auth_levelfield  = "entity";
  $config_max_loginattempts = 0;
  $config_auth_loginform = true;
  $config_auth_userdescriptor = "[userid]";
  $config_authentication_md5 = true;
  $config_securityscheme = "group";
  $config_debug = 2;
  $config_auth_accountenableexpression = "role='employee' AND status='active'";
  $config_atktempdir = $config_atkroot."achievotmp/";
  $config_tplcompiledir = $config_atktempdir."compiled/tpl/";
  $config_auth_grantall_privilege = "employee.profile.grantall";
  $config_durationformat = 0;
  $config_defaultfavico = "images/achievo.ico";
?>
