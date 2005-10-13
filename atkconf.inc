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
  $config_corporate_module_base = "modules.achievomodule";
  $config_menu_layout = "achievo";

  // -------------------------------------------
  //        TEST FRAMEWORK CONFIGURATION
  // -------------------------------------------

  // The test framework can automatically run tests on achievos
  // functionallity. In order to do this, the framework needs a valid
  // login (username and password) with all access rights. Because the
  // tests may take a long time to complete, it is also wise to set a
  // high time-out value on connections and total test duration.
  //
  // NOTE: the test framework is intended to provide achievo and achievo
  // module developers with an automatic unit and webtesting feature. If
  // you just want to use achievo, then it won't be nescecerry for you to
  // configure these settings.

  // The url used by the testing framework in order to reach your achievo
  // installation through the webserver's http interface. A trailing slash
  // *must* be present in this url!
  //
  $config_test_achievourl = "http://yourhost/yourachievopath/";

  // The username used for automated testing. Be sure to specify a user
  // having sufficient rights to access all areas you want to test.
  //
  $config_test_username = "username";

  // The password belonging to the username specified above. Please mind
  // that password's are case sensitive.
  //
  $config_test_password = "password";

  // The maximum duration of all tests in seconds. This overrides the
  // webserver's default value in order to provide the tests with enough
  // time to complete all tests. Setting this value too low will cause
  // the webserver to stop the testscript before completing all tests.
  // On the other side, setting this value too high will increase the risk
  // of hanged tests being active for a much too long time. To find the
  // right value, you may need to experiment a little bit. It mostly
  // depends on how fast your webserver is and how many tests you run
  //
  $config_test_maxexecutiontime = 600;

  // The maximum duration of a page-request by the webtester, measured in
  // seconds. Set this timeout as high as you want your page requests to
  // last at most. But be carefull about setting this value too low, it
  // may cause tests to fail, without giving you any clue it was caused
  // by a time-out when requesting a page.
  //
  $config_test_connectiontimeout = 60;

?>
