<?php   

  /*



     Hello. 
    
     If you can read this text, one of two things might be the case:
    
     1) You installed Achievo, and pointed your browser to the setup.php 
        script, and were expecting the Achievo setup screen to come up.
        
        If this is the case, then the fact that you are reading this indicates
        that either you don't have PHP installed, or it is not configured 
        correctly. 
        Please make sure your PHP-installation works, and come back
        when you got it running. You might want to consult your system 
        administrator. If you can't get it to work, subscribe to the Achievo
        mailinglist and we will try to help you out.
    
        You can ignore the rest of what you see below: this is programmers' 
        code, that you shouldn't be seeing in the first place.
        
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
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$
   *
   */
  
  // First we define some functions and constants that we need for
  // validating the PHP configuration.
  
  // Some defines we need
  define("REQUIRED_PHP", "4.1.0");
  
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
      echo '  If you can\'t get Achievo to work after following the above instructions, subscribe to the Achievo mailinglist en we might be able to help you.';
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
      && strpos($include_path, ".;")===false && strpos($include_path, ";.")===false)
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
  
  $dbs = array("mysql"=>"mysql_pconnect",
               "oci8"=>"OCILogon",
               "oci9"=>"OCILogin",
               "pgsql"=>"pf_connect");
  if (!function_exists($dbs[atkconfig("database")]))
  {
    $errors[] = "Your PHP installation seems to be compiled without <b>".atkconfig("database")."</b> 
                 database support.
                 <br>Please recompile PHP with support for ".atkconfig("database").", or, if you 
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
  $obj = getNode("setup.setup"); 
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

  $g_layout->outputFlush();
  
?>
