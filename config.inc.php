<?php
  //
  // ACHIEVO CONFIGURATION FILE 
  //
  // Change this file to fit your needs before using Achievo.
  // 

  // The database configuration. Specify the hostname of the database server,
  // the database to use and the user/password.
  //
  $config_databasehost = "localhost";
  $config_databasename = "achievo_0_6";
  $config_databaseuser = "demo";
  $config_databasepassword = "demo";
  
  // In admin pages, Achievo shows you a number of records with previous and
  // next buttons. You can specify the number of records to show on a page.
  //
  $config_recordsperpage=25; 

  // The theme defines the layout of Achievo. You can see which 
  // themes there are in the directory themes. Users can choose their own 
  // theme in the user preferences section.
  //
  $config_defaulttheme = "slash";
  
  // The language of the application. You can use any language for which
  // a language file is present in the languages directory.
  // 
  $config_languagefile="english.lng";
          
  // The Achievo administrator password. The password of all users is read from the 
  // database, except the administrator password. This is useful when 
  // you have an empty or corrupt database. 
  // Note that it has no effect when you use the 'user preferences' screen
  // to change the administrator password, since the administrator password
  // is fixed in this file.
  //
  // Note: As an extra security measure you could disable this once you have
  // set up the entire system and enable it again once you need it.
  // (disable it by putting // in front of it)
  //
  $config_administratorpassword = "demo";  
  
  // The number of projects/phases to show in the 'recent projects/phases'
  // dropdown. The more you specify here, the slower hour administration 
  // gets (due to some javascript issues).
  //
  $config_numberofrecentprojects = 10;

  // The ammount of time that a user can book on a day before it is 
  // considered overtime. 
  $config_overtimethreshold = 480;

  // Leave this line in place, it configures the backend of Achievo.
  // Also, you should not change the atkconf.inc file, since that would 
  // break Achievo.
  include "atkconf.inc";
?>
