<?php

  /*



     Hello.

     If you are reading this, one of two things is likely the case:

     1) You installed Achievo, pointed your browser to the setup.php
        script, and were expecting the Achievo setup screen to come up.

        If this is the case, then the fact that you are reading this
        indicates that either a) PHP is not installed on your server,
        b) your server is not correctly configured for PHP, or c) PHP
        is not correctly configured on your server.
        
        Please review your server installation and configuration, make
        the appropriate corrections, and then return here. If you cannot
        configure your server to work correctly, consult your system
        administrator or other resource. You may also consider contacting
        a web server support forum for your operating system or
        distribution.
        
        You can also see the Achievo README.md for current Achievo
        support information. If you join the Achievo forum and post a
        question, a community member may be able to help and respond.
        However, please understand there are many potential causes for
        problems, and Achievo community members concentrate primarily on
        Achievo itself - although someone may have expertise in the web
        application server stack and operating system you are using.

        Ignore the rest of what you see below: this is Achievo program
        code that is visible because of your server problems.

     2) You are reading the source file setup.php with an editor.
        If you are a programmer or just interested in the source of this
        script, just read on. :-)
        If you are wondering how you can run this installer, just point
        your browser to this file. For example:
        http://yourserver/achievo/setup.php


     Greetings,
     the Achievo development team





















   */

  /**
   * Achievo setup script
   *
   * This script can be used to install the Achievo database, and/or upgrade
   * an existing database.
   *
   */

  // First we define some functions and constants that we need for
  // validating the PHP configuration.

  // Some defines we need
  define("REQUIRED_PHP", "5.1.2");
  define("ACHIEVO_MIN_MEM","32");

  /**
   * Function for checking php version.
   * Found on http://www.php.net/manual/nl/function.phpversion.php
   * (posted there by akov@relex.ru)
   */
  function check_php_version($version)
  {
    // intval used for version like "4.0.4pl1"
    $testVer=intval(str_replace(".", "",$version));
    $curVer=intval(str_replace(".", "",phpversion()));
    if( $curVer < $testVer )
       return false;
     return true;
  }

  /**
   * Function that displays a notice that the installer will not run, with
   * the reason(s) why.
   */
  function displayErrors($errors, $fixablebyuser=true)
  {
    // Since this function can be called before any ATK stuff has been loaded,
    // we have no rendering enging whatsoever, so we need to display html
    // manually.
    echo '<html>
            <head>
              <title>Achievo error</title>
            </head>
            <body bgcolor="#ffffff">
              <br><b>A problem has occurred</b>
              <br>
              <br>We\'re very sorry, but this server is unable to run Achievo,
                  for the following reason(s):
              <br>
              <ul>';
    for($i=0, $_i=count($errors); $i<$_i; $i++)
    {
      echo '<li>'.$errors[$i].'<br><br>';
    }
    echo '    </ul>';
    if ($fixablebyuser)
    {
      echo '  If you can\'t get Achievo to work after following the above instructions, please visit the <a href="http://www.achievo.org/forum" target="_new">Achievo forums</a> and we might be able to help you.';
    }
    echo '  </body>
          </html>';
  }

  // Begin checks

  $errors = array();

  // Check minimum required PHP version
  if (!check_php_version(REQUIRED_PHP))
  {
    $errors[] = "You are using PHP version ".phpversion().", but you need at least php version ".REQUIRED_PHP." to run Achievo.
                 <br>Please upgrade your installation or consult your system administrator.";
  }

  // Some versions of PHP contain bugs and behave badly when running Achievo.
  // Therefore, we check for known buggy versions.
  $version = phpversion();
  if ($version=="4.1.2")
  {
    $errors[] = "The PHP version you are using ($version) is known to have some
                 bugs that prevent Achievo from running properly.
                 <br>Please use the most recent stable PHP version.";

  }

  // Check memory limit
  $memory_limit = ini_get('memory_limit');
  if(empty($memory_limit))
  {
	  $memory_limit = "-1";
  }
  if( $memory_limit == "" )
  {
      // memory_limit disabled at compile time, no memory limit
  }
  elseif( $memory_limit == "-1" )
  {
    // memory_limit enabled, but set to unlimited
  }
  else
  {
  	$mem_display = $memory_limit;
    rtrim($memory_limit, 'M');
    $memory_limit_int = (int) $memory_limit;
    if( $memory_limit_int < ACHIEVO_MIN_MEM )
    {
      $errors[] = "The minimal memory limit for running Achievo is ".ACHIEVO_MIN_MEM."M.
                  <br>Please change the memory limit in your php.ini (".get_cfg_var("cfg_file_path").").";
    }
  }

  // Check session Save path (only possible if we don't have a open_basedir restriction)
  if (!ini_get("open_basedir"))
  {
    $temp_dir = (isset($_ENV['TEMP'])) ? $_ENV['TEMP'] : "/tmp";
    $session_save_path = (session_save_path() === "") ? $temp_dir : session_save_path();
    if (strpos ($session_save_path, ";") !== FALSE)
    {
      $session_save_path = substr ($session_save_path, strpos ($session_save_path, ";")+1);
    }
    if(is_dir($session_save_path))
    {
      if(!is_writable($session_save_path))
      {
        $errors[]="The session save path ($session_save_path) isn't writable for the webserver.";
      }
    }
    else
    {
      $errors[]="The session save path ($session_save_path) is not a valid directory.";
    }
  }

  // Achievo relies on some PHP settings. In future versions, Achievo should not
  // rely on these, but for now, we need to check the settings to verify if they
  // are correct.

  $restarthint = "<br>Don't forget to restart the webserver after changing this."; // common sentence
  $inilocation = get_cfg_var("cfg_file_path");
/*
  $register_globals = (bool)ini_get("register_globals");
  if (!$register_globals)
  {
    $errors[] = "The <b>register_globals</b> setting in <b>$inilocation</b> is set to Off.
                 <br>Currently, Achievo will only function with this setting turned on.
                 <br>Please alter $inilocation and set this value to on. ".$restarthint;
  }
  */
  $include_path = ini_get("include_path");
  // in unix, includepath elements are separated by :, in windows, by ;
  if (strpos($include_path, ".:")===false && strpos($include_path, ":.")===false
      && strpos($include_path, ".;")===false && strpos($include_path, ";.")===false
      && ($include_path!="."))
  {
    $errors[] = "The <b>include_path</b> setting in <b>$inilocation</b> is not set correctly.
                 <br>This setting should contain at least the 'current dir' ('.'), but it is
                 currently set to '$include_path' only.
                 <br>Please alter $inilocation and correct this value. ".$restarthint ;
  }

  // Achievo 1.1 requires a writable temp dir. We must check if we can write.
  $fp = @fopen("achievotmp/compiled/tpl/setuptest", "w");
  if ($fp==FALSE)
  {
    $errors[] = "The Achievo temporary directory is not writable by the webserver.
                 <br>Please check the permissions of the achievotmp/ directory (and its subdirectories).
                 <br><br>On Linux/Unix you can run the command 'chown -R <i>username</i> achievotmp' to make the directory
                 writable by the webserver. Replace <i>username</i> with the username that you use to run the webserver,
                 for example 'nobody', 'apache' or 'www'.";
  }
  else
  {
    fclose($fp);
    unlink("achievotmp/compiled/tpl/setuptest");
  }

  // If there are any errors when we reach this point, it's no use continuing because
  // nothing will work, not even the installer.
  if (count($errors)>0)
  {
    displayErrors($errors);
    exit;
  }

  /* Setup the system */
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();

  // Watch out. We don't know yet if the database configuration is valid.
  // Therefor, we should force atksecure to use an authenticationmethod
  // that doesn't rely on the db. Since only the administrator user from
  // the configfile may log in, we can set it to "config".
  $old_authentication = $config_authentication;
  $config_authentication = "config";
  atksecure();
  $config_authentication = $old_authentication; // restore to the original,
                                                // for node installations
                                                // may be dependent on
                                                // the value of this setting.
                                                // for example, employee only
                                                // adds a password field if
                                                // authentication is 'db'.

  if ($g_user["name"]!="administrator")
  {
    $errors[] = "You can only run the install script using the 'administrator' account.";
  }

  // Now that we have included the configfile, we can validate the database setup.
  // Note: we don't check the existance of the database or its tables yet, since
  // the installer can install the database if needed. Here, we just check if PHP
  // was compiled with the correct database support.

  $dbs = array("mysqli"=>"mysqli_connect",
               "oci8"=>"OCILogon",
               "oci9"=>"OCILogin",
               "pgsql"=>"pg_connect");
  $dbconfig = atkconfig("db");
  if (!function_exists($dbs[$dbconfig["default"]["driver"]]))
  {
    $errors[] = "Your PHP installation seems to be compiled without <b>" . $dbconfig["default"]["driver"] . "</b>
                 database support.
                 <br>Please recompile PHP with support for " . $dbconfig["default"]["driver"] . ", or, if you
                 installed from rpm, install the php rpm for this database.
                 <br>Alternatively, select a different database in the config.inc.php file (<b>\$config_database</b>).";
  }

  // If there are any errors when we reach this point, it's no use continuing because
  // we don't have a correct database setup, so we can't continue setup.
  if (count($errors)>0)
  {
    displayErrors($errors);
    exit;
  }

  // Create node
  $obj = &atkGetNode("setup.setup");
  if ($ATK_VARS["atkaction"]=="") $ATK_VARS["atkaction"] = "intro";

  if (is_object($obj))
  {
    $obj->dispatch($ATK_VARS);
  }
  else
  {
    $errors[] = "Achievo will not run for a (yet) unkown reason. The install-script will dump
                 some information below.
                 <br>Please e-mail this entire page to bugs@achievo.org for analysis, so we might
                 be able to help you, or fix Achievo to work with your setup.
                 <br><br>PHP version: ".phpversion()."
                 <br><br>Please mention which operating system, which webserver, and
                 which database you are using.
                 <br>If possible, include their version numbers as well.";

    // Something went wrong, but it is uncertain what it is. To investigate these
    // kinds of issues, we force $config_debug to 'on' and ask the user politely
    // to mail us the debugoutput.
    displayErrors($errors, false);
    $config_debug = 1;
  }

  $output = &atkOutput::getInstance();
  $output->outputFlush();

?>
