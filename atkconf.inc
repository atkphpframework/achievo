<?php
  // ATK Configuration file. 
  // This configures the atk backend for use with Achievo.
  // Do *NOT* change this file, ever!
  //
  // If you want to configure Achievo, edit the config.inc.php3 file.
  
  $config_top_frame = 1;
  $config_authentication = "db";  
  $config_auth_usertable = "employee";
  $config_auth_userfield = "userid";
  $config_auth_accesstable = "accessright";
  $config_auth_passwordfield = "password";
  $config_auth_leveltable  = "employee";
  $config_auth_levelfield  = "entity";
  $config_authentication_md5 = true;
  $config_securityscheme = "group";
  $config_debug = 2;
?>
