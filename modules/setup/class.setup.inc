<?php

  /**
   * You might have noticed, that the module.inc file for the setup module
   * does not contain any nodes. This means, that you can not grant anyone
   * any rights to use the setup node. 
   * This ensures, that only the administrator user can make use of the 
   * stuff presented here
   */
   
  class setup extends atkNode
  {
    var $m_report = array(); 
  
    function setup()
    {
      $this->atkNode("setup");
    }
    
    function action_intro()
    {
      $this->setupScreen("Introduction", 
                          "I am the Achievo setup script. I can do several things for you:
                          <ul>
                            <li>I can check for new versions of Achievo or one of your installed modules. 
                            <li>I can install an Achievo database if you don't already have one.
                            <li>I can upgrade an existing Achievo database if necessary.
                            <li>I can do a sanity check on the Achievo database (not yet implemented).                            
                          </ul>
                          <br>I will try to do my job with as little input as possible. I will make
                          educated guesses about what I need to do, based on your current database
                          (if any), and the Achievo config.inc.php file. 
                          <br>If you haven't done so, please make sure your config.inc.php is 
                          correct, before clicking the 'continue' button. In config.inc.php, you
                          have to specify the database I should use and a valid username and password.
                          ",
                          "dbcheck");
    }
    
    function action_dbcheck()
    {            
      $res = $this->getDbStatus();
      
      if ($res==DB_SUCCESS)
      {
        // Database seems to be ok. Now we go check what version(s) we are running.
        return $this->action_versioncheck();
      }
      else
      {
        // something went wrong.      
        return $this->handleDbStatus($res); 
      }          
    }
    
    function getDbStatus()
    {
      global $g_db;
      
      // We don't want the db class to display error messages, because
      // we handle the error ourselves.
      $g_db->m_haltonerror = false;            
      
      $res = $g_db->connect();
      
      if ($res==DB_SUCCESS)
      {        
        // This can't be trusted. Mysql returns DB_SUCCESS even
        // if the user doesn't have access to the database. We only get an
        // error for this after we performed the first query.
        $tmp = $g_db->getrows("show tables");  // TODO FIXME: 'show tables' is NOT database independent
        $res = $g_db->_translateError($g_db->m_errno);
      }
          
      return $res;
    }
    
    /** 
     * Handles the database status. Because of the way some databases
     * handle connections, DB_SUCCESS needs to be confirmed by 
     * performing a query. If handleDbStatus is not called with
     * confirmed=true, it will perform this query and confirm itself.
     */
    function handleDbStatus($dbstatus)
    {    
      switch($dbstatus)
      {
        case DB_SUCCESS: // We're not supposed to handle a db status if the db is ok. But, in the case some 
                         // programmer decides to call handleDbStatus anyway, we'll just return a screen with an 'ok' message.
          return $this->setupScreen("Connection succesful",
                                    "Your database connection appears to be OK");
          
          // In the case we don't have a DB_SUCCESS now, we fall through
          // the other cases (we don't break), so we can handle the error.
        case DB_UNKNOWNHOST:
          return $this->setupScreen("Unable to connect to database-server", 
                                    "In config.inc.php, you have set \$config_databasehost to '<b>".atkconfig("databasehost")."</b>'.
                                     However, I am not able to connect to that server. 
                                     This could mean:
                                     <ul>
                                       <li>The hostname is incorrect. In this case, please correct the error in the config.inc.php file and click 'Retry'.
                                       <li>The host is down. You might want to retry later or contact the servers' administrator.
                                       <li>The host is up, but there's no databaseserver running. You might want to select a different server, or contact your system-administrator.                                       
                                     </ul>", "dbcheck");
          
        case DB_UNKNOWNDATABASE:
          return $this->setupScreen("Database does not exist", 
                                    "The database specified in config.inc.php ('".atkconfig("databasename")."') does not exist.
                                     You have to do one of the following:
                                     <ul>
                                       <li>Correct the config.inc.php file by setting \$config_databasename to the correct database.
                                       <li>Create the database, and make sure you grant the correct rights to the user you specified in config.inc.php.
                                     </ul>
                                     Once the configuration is ok, you can click 'retry' and the setup process can continue.",
                                     "dbcheck");
          
        case DB_ACCESSDENIED_USER:
          return $this->setupScreen("Database username or password incorrect", 
                                    "I've tried to connect to the database with username '".atkconfig("databaseuser")."' and the password you specified in config.inc.php, but was
                                     unable to login. 
                                     <br>This could mean:
                                     <ul>
                                       <li>The username and or password in config.inc.php are not correct. In this case, please correct the error and click 'Retry'.
                                       <li>The user has not been created yet in the databaseserver. In this case, create the useraccount or contact your system-administrator. When the user is created, click 'Retry'.".
                                       (atkconfig("database")!="mysql"?"":"<li>A common problem when using MySQL is that the user was created, but the privileges were not flushed. You might want to try reloading the database (or issuing a 'FLUSH PRIVILEGES' statement), and clicking 'Retry'.").
                                    "</ul>",
                                    "dbcheck");
          
        case DB_ACCESSDENIED_DB:
          return $this->setupScreen("User not allowed to connect to database", 
                                    "I've tried to connect to the database with username '".atkconfig("databaseuser")."' and the password you specified in config.inc.php, but was 
                                     unable to connect. The username and password appear to be ok, but the user is not allowed to use the database '".atkconfig("databasename")."'.
                                     <br>This could mean:
                                     <ul>
                                       <li>The username has not been granted access to the database. Please correct the error and click 'Retry'".
                                       (atkconfig("database")!="mysql"?"":"<li>A common problem when using MySQL is that access was granted, but the privileges were not flushed. You might want to try reloading the database (or issuing a 'FLUSH PRIVILEGES' statement), and clicking 'Retry'.").
                                    "</ul>",
                                    "dbcheck");
              
        default: // catches any errors we don't know of, and also DB_UNKNOWNERROR
          return $this->setupScreen("Unknown connection problem",
                                    "There was an unknown problem when I tried connecting to the database. Please verify your database setup and
                                     the database configuration in config.inc.php. If you think you have fixed the problem, you may click 'Retry'.",
                                    "dbcheck");
      }
    }
    
    function setupScreen($title, $output, $nextaction="")
    {
      global $g_layout;
      
      $g_layout->initGui();
      $g_layout->ui_top($title);
                  
      $form = '<b>'.$title.'</b><br><br>
               <form action="setup.php">'.
               session_form(SESSION_NESTED).
               '<input type="hidden" name="atkaction" value="'.$nextaction.'">'.
               $output;
       
      if ($nextaction!="")
      {                        
        $form.= '<br><br><input type="submit" value="'.($nextaction!=$this->m_action?"Continue":"Retry").'"><br><br>';
      }
               
      $form.= '</form>';
      $g_layout->output($form);
      $g_layout->ui_bottom();
      
    }
    
    /**
     * Check what version of the Achievo database we are using.
     */
    function action_versioncheck()
    {         
      // Check if we have a connection first.
      $res = $this->getDbStatus();
      
      if ($res == DB_SUCCESS)
      {
        global $g_db;
        $tablenames=array();
        $tables = $g_db->table_names();
        for ($i=0, $_i=count($tables); $i<$_i; $i++)
        {
          $tablenames[] = $tables[$i]["table_name"];
        }                
    
        if (count($tables)==0)
        {
          return $this->setupScreen("Database appears to be empty",
                                    "Your database connection is working. However, the specified database ('".atkconfig("databasename")."') seems to be empty.
                                     <br>This means I need to install a new database. 
                                     <br><br><i>If you think this is not correct, please verify your database setup, and rerun this script.</i>
                                     <br><br>To continue with the installation of a new Achievo database, please click 'Continue'.",
                                    "installdb");
        }
        else if (!in_array("hours", $tablenames))
        {
          // The 'hours' table has been used since the very beginning of Achievo.
          // If this table is not present, and the database is not empty, then
          // the database is not an Achievo database and we will refuse to 
          // install or upgrade here.
          return $this->setupScreen("Database is not an Achievo database",
                                    "The database configured in config.inc.php ('".atkconfig("databasename")."') is not empty, and it does not
                                     contain an Achievo database. This database probably belongs to a different application.
                                     <br>It is strongly recommended to install Achievo to a new, empty database.
                                     <br><br>If you are intentionally installing Achievo into a non-empty database, you can 
                                     continue, but keep this in mind: The installation process may fail if there are conflicting tables                                      already present in the database, and it might even try to modify or drop existing tables. 
                                     <br><br><b>Continue at your own risk</b>.",
                                     "installdb");
        }
        if (in_array("versioninfo", $tablenames))
        {
          return $this->setupScreen(">=0.9.2", "Hi, you have a db that's 0.9.2 or higher");
          // TODO: read versioninformation and perform upgrades
        }
        else
        {
          // Apparently, this is a version from before 0.9.2, when we didn't have the
          // versioninfo table yet.
          // Perform some checks to determine what version this database might be.
          // We do this by looking at some tables that exist, that were created in
          // several Achievo versions.
          $version = "";
          
          if (in_array("projectactivity", $tablenames) && in_array("templateactivity"))
          {
            // If these two tables exist, this is a 0.4 database.
            $version = "0.4.x";
            // TODO FIXME: the 0.4 -> 0.6 conversion script works completely different
            // from the others, in that it reads from db 1 and inserts in another database.
            // We'll have to rewrite this script.
          }
          else if (in_array("todo_history", $tablenames) && in_array("employee_project", $tablenames))
          {
            $version = "0.9.1";
          }
          else if (in_array("schedule", $tablenames) && in_array("schedule_types"))
          {
            $version = "0.9";
          }
          else if (in_array("hours_lock", $tablenames) && in_array("contract", $tablenames)
                  && !in_array("schedule", $tablenames))
          {
            $version = "0.8.x";
          }
          
          if ($version!="")
          {          
            return $this->setupScreen("Database version $version detected",
                                      "The database you are currently using ('".atkconfig("databasename")."') appears to be from Achievo version $version.
                                       <br>I can upgrade this database for you. Please make sure you backup the database
                                       first, because although I know what I'm doing, I'm just an open source script, and 
                                       I come with no warranty whatsoever. If I screw up the database, I've got nothing to 
                                       offer you but 'oops, sorry!'.
                                       <br>
                                       <br>In other words, backup your database first!
                                       <br>
                                       <br>If you've taken the necessary precautions, click the continue button to upgrade the database.",
                                      "upgrade$version");
          }
          else
          {
            return $this->setupScreen("Unknown Achievo version",
                                      "I was unable to determine what version of Achievo your current database ('".atkconfig("databasename")."') is.
                                       <br>
                                       <br>Please verify if the database configuration you specified in config.inc.php is correct, and try again.
                                       <br>
                                       <br>If you are sure the configuration is correct, please send a message to the Achievo mailinglist so we can 
                                       try to determine why the setup script does not recognize your Achievo database.",
                                      "dbcheck");
          }
        }
          /*return $this->setupScreen("Database exists and is not empty",
                                    "In a future release, I will be able to upgrade this database automatically.
                                    <br>But I can't do that yet.                                     
                                    <br>I'm also not yet able to determine what version you are currently running.
                                    <br>I can't even see if this database is up to date or not. Poor me.
                                    <br>
                                    <br>But I'll give you some pointers to help you upgrade the database manually.
                                    <br>(Be sure to backup your database beforehand!)
                                    <br>
                                    <br>If you were using a database from version <b>0.9.1</b>, you can use <b><a href=\"upgrade/convert-0.9.1-to-0.9.2.php\">this upgrade-script</a></b> to upgrade to 0.9.2.
                                    <br><br>If you are running a different version, please check Achievo's upgrade directory and run all
                                        necessary patches. The .sql files need to be executed in mysql. The .php files can be run through your browser.
                                        Please be sure to apply the upgrade scripts in the correct order.");*/
        
      }
      else
      {
        return $this->handleDbStatus($res);
      }
    }
    
    function action_installdb()
    {
      global $g_modules, $g_layout;
      
      $resultmsg = array("ok" => "Installation completed",
                         "failed" => "Installation failed",
                         "faileddep" => "Not installed (dependencies could not be resolved)",
                         "cyclicdep" => "Not installed (Cyclic dependency detected)",
                         "alreadyinstalled" => "No installation necessary, module was already installed",
                         "notfound" => "Not installed (Module could not be found)");
                         
      
      // We loop through the modules to install them. 
      // The setup module contains essential table needed for the installation process,
      // so we install this first.
      $result = array();
      
      if ($this->installModule("setup", $result))
      {
      
        foreach ($g_modules as $modname=>$moduledir)
        {
          if ($modname!="setup") // we already installed setup when we started.
          {
            if (!$this->installModule($modname, $result, true))
            {
              atkdebug("Installation failed.");
              break;
            }
          }
        }        
      }
      else
      {
        $output = "I was unable to install the system table, so I cannot continue with the installation. Please verify your database setup and try again.";
      }
      
      if (count($result)>0)
      {
        $g_layout->initGui();
        $output = $g_layout->data_top();
        $output.= $g_layout->tr_top();
        $output.= $g_layout->ret_td_datatitle("Module");
        $output.= $g_layout->ret_td_datatitle("Result");
        $output.= $g_layout->tr_bottom();
        foreach ($result as $modname => $result)
        {
          $output.= $g_layout->tr_top();
          $output.= $g_layout->ret_td($modname);          
          $output.= $g_layout->ret_td((!empty($resultmsg[$result])?$resultmsg[$result]:$result));
          $output.= $g_layout->tr_bottom();
        }
        $output.= $g_layout->data_bottom();
      }
      return $this->setupScreen("Installation result", $output);
    }
    
    /**
     * Install a module, and if dependencies on other modules exist, install
     * those modules as well or fail with an error message.
     *
     * @param $modname The name of the module to install.
     * @param $result  This is an array in which the method will store the
     *                 result of the installation, using the modulename as
     *                 array key.
     * @param $resolvedeps If set to false, the installModule function will
     *                     break if dependencies are not met. If set to true
     *                     It will try to install the modules the requested
     *                     module depends on.
     */
    function installModule($modname, &$result, $resolvedeps=false)
    {
      atkdebug("installModule call for $modname");
      // Cycle protection.
      if ($result[$modname]=="ok")
      {
        // Module is already installed. 
        atkdebug("Module ".$modname." already installed in this run.");
        return true;
      }
      else if ($result[$modname]=="faileddep")
      {
        // Module failed dependencies before, and now apparently, during the 
        // resolvement of its dependencies, some other module requested it
        // to be installed again. This will lead to a loop, so we must
        // stop the installation.
        $result[$modname] = "cyclicdep";
        atkdebug("Module ".$modname." has cyclical dependency.");
        return false;
      }
    
      // Retrieve already installed modules. We are currently only interested
      // in the names of the installed modules, not the versions.
      $installed_mods = array_keys($this->getInstalledModules());
      
      // Yet another check. The requested module may already have been
      // installed before. 
      if (in_array($modname, $installed_mods))
      {
        $result[$modname] = "allreadyinstalled";
        atkdebug("Module ".$modname." already installed in previous run.");
        return true;
      }
      
      $module = getModule($modname); 
      if (!is_object($module))
      {
        $result[$modname] = "notfound";
        atkdebug("Module ".$modname." could not be found.");
        return false;
      }
      
      $deps = array();
      
      // if method doesn't exist, assume that there are no dependencies.
      // Note that everything derived from achievoModule should have this 
      // method. Therefor, absence indicates that this module might not be
      // an achievo module.
      if (method_exists($module, "getDependencies"))       
      {
        $deps = $module->getDependencies(DEP_INSTALL);
      }
      else
      {
        atkdebug("Warning: $modname might not be an Achievo module.");
      }
      
      for ($i=0, $_i=count($deps); $i<$_i; $i++)
      {
        if (!in_array($deps[$i], $installed_mods))
        {
          // Dependency not met.
          atkdebug("Resolving dependency ".$deps[$i]." for $modname");
          $result[$modname] = "faileddep";
          if ($resolvedeps)
          {
            if (!$this->installModule($deps[$i], $result, true))
            {
              atkdebug("Stopping install of $modname. Resolving dependencies failed");
              return false; // dependencies coult not be resolved. Give up.
            }
          }
          else
          {
            atkdebug("Stopping install of $modname. Dependencies not met");
            return false;
          }
        }
      }
      
      // if we get here, all dependencies are either resolved, or we have given up already
      // and returned false. Next thing to do is perform the database installation.
      $installfile = moduleDir($modname)."install/install.inc";   
      if (file_exists($installfile))
      {
        // give included file access to $db and $this ("$setup");
        global $g_db;
        $db = &$g_db;
        $setup = &$this;
        
        // also set 'current module' to the module were now installing. This will 
        // be used by setup functions called from the install file.
        $this->m_currentmodule = $modname;
        include($installfile);
      }
      
      // if we get here, everything should be installed.
      atkdebug("Installed $modname");
      $result[$modname] = "ok";
      return true;
    }
    
    /**
     * This function retrieves the list of installed modules. 
     *
     * @return An array, where the modulename is the key, and the current
     *         installed version of the module is the value.
     */
    function getInstalledModules()
    {
      // (Ivo) Mental note: I originally had in mind to cache the result of the 
      // query, so subsequent calls would not need to perform a query. 
      // But I didn't implement this, since modules may be installed or
      // deinstalled between 2 calls.
      $result = array();
            
      $versioninfo = getNode("setup.versioninfo");
      
      if (is_object($versioninfo)) // If this object doesn't exist, we don't have
                                   // a correct installer, so we can only return 
                                   // an empty  result.      
      {       
        // for fresh installations the versioninfo doesn't exist. we set
        // errorreporting to silent, because we don't want to know this.
        global $g_db;
        $curhaltval = $g_db->m_haltonerror;
        $g_db->m_haltonerror = false;
        $rows = $versioninfo->selectDb();
        $g_db->m_haltonerror = $curhaltval; // reset to original state.
        
        for ($i=0, $_i=count($rows); $i<$_i; $i++)
        {
          $result[$rows[$i]["module"]] = $rows[$i]["version"];
        }        
      }
      return $result;
    }
    
    function installNode($nodename)
    {
      $node = &getNode($nodename);            
      
      if (is_object($node))
      {
        // We have to check if the table already exists. If it exists, we may need to
        // add columns to it.
        global $g_db;
        $meta = $g_db->metadata($node->m_table, true);                
        
        include_once(atkconfig("atkroot")."atk/db/class.atkddl.inc");        
        $ddl = atkDDL::create();
        
        if (is_object($ddl))
        {
          if (count($meta)==0 || $meta["num_fields"]==0)
          {
            atkdebug("Table ".$node->m_table." does not exist; creating...");      
                
            $ddl->setTable($node->m_table);
          
            foreach ($node->m_attribList as $attribname => $attrib)
            {          
              $flags = 0;
              if ($attrib->hasFlag(AF_PRIMARY)) $flags|=DDL_PRIMARY;
              if ($attrib->hasFlag(AF_UNIQUE)) $flags|=DDL_UNIQUE;
              if ($attrib->hasFlag(AF_OBLIGATORY)) $flags|=DDL_NOTNULL;
              
              // if ($attrib->hasFlag(AF_AUTO_INCREMENT)) $flags|=DDL_AUTO_INCREMENT; not yet implemented                    
            
              $ddl->addField($attribname, 
                             $attrib->dbFieldType(), 
                             $attrib->dbFieldSize(), 
                             $flags);
            }                  
            
            $ddl->executeCreate();
          }
          else
          {
            atkdebug("Table ".$node->m_table." already exists; performing diff...");
            
            // Create lookup table of fields.
            for ($i=0, $_i=count($meta); $i<$_i; $i++)
            {
              $fields[$meta[$i]["name"]] = $meta[$i];
            }
            
            $ddl->setTable($node->m_table);
          
            foreach ($node->m_attribList as $attribname => $attrib)
            {          
              if (in_array($attribname, array_keys($fields)))
              {
                // field exists. assume it's ok.
                // TODO FIXME: verify size and type and if necessary, alter the column.
                atkdebug("Field $attribname already exists");
              }
              else
              {
                atkdebug("Field $attribname does not exist. Adding to alter table queue...");
                $flags = 0;
                if ($attrib->hasFlag(AF_PRIMARY)) $flags|=DDL_PRIMARY;
                if ($attrib->hasFlag(AF_UNIQUE)) $flags|=DDL_UNIQUE;
                if ($attrib->hasFlag(AF_OBLIGATORY)) $flags|=DDL_NOTNULL;
              
                // if ($attrib->hasFlag(AF_AUTO_INCREMENT)) $flags|=DDL_AUTO_INCREMENT; not yet implemented                    
            
                $ddl->addField($attribname, 
                               $attrib->dbFieldType(), 
                               $attrib->dbFieldSize(), 
                               $flags);
              }   
            }                  
            
            if ($ddl->executeAlter())
            {
              atkdebug("Table ".$node->m_table." altered...");
            }
          }
        }
        else
        {
          atkdebug("setup::installNode: ddl class not found");
        }
      }
      else
      {
        atkdebug("setup::installNode: node $node not found.");
        return false;
      }
    }
    
    function setVersion($version, $module="")
    {
      if ($module=="") $module = $this->m_currentmodule;
      
      if ($module!="")
      {
        $versioninfo = &getNode("setup.versioninfo");
        $recs = $versioninfo->selectDb("module='".$module."'");
        if (count($recs)>0)
        {
          // rec already exists. Update.
          $versioninfo->updateDb(array("module"=>$module, "version"=>$version));
        }
        else
        {
          // rec doesn't exist. Insert.
          $rec = array("module"=>$module, "version"=>$version);
          $versioninfo->addDb($rec);
        }
      }
    }      
    
    function report($progressmsg)
    {
      $this->m_report[] = $progressmsg;
    }
    
    // TODO FIXME: functions that follow are *NOT* yet database
    // independent. They should use the abstractionlayer. 
    
    function renameSequence($seq_name,$new_name)
    {
      global $g_db;
      $sql = "UPDATE db_sequence SET seq_name='".$new_name."'
                WHERE seq_name='".$seq_name."'";
      $res = $g_db->query($sql);  
    }
    
    function renameTable($old, $new)
    {
      global $g_db;    
      $sql = "ALTER TABLE $old RENAME $new";
      $res = $g_db->query($sql);    
      
      //change db_sequence name
      $this->renameSequence($old, $new);  
    }
    
    function addColumn($table, $col, $type, $nullable=true, $default="")
    {
      global $g_db;
    
      $sql = "ALTER TABLE $table ADD $col $type";
      if (!$nullable) $sql.=" NOT NULL";
      if ($default!="") $sql.= " DEFAULT $default";
      $g_db->query($sql);        
    }
    
    function alterColumn($table, $col, $newname, $type, $nullable=true, $default="")
    {
      global $g_db;
      $sql = "ALTER TABLE $table CHANGE $col $newname $type";
      if (!$nullable) $sql.=" NOT NULL";
      if ($default!="") $sql.= " DEFAULT $default";
      $g_db->query($sql);
    }
    
    function dropColumn($table, $col)
    {
      global $g_db;
    
      $sql = "ALTER TABLE $table DROP $col";      
      $res = $g_db->query($sql);        
    }        
    
    function dropTable($table)
    { 
      global $g_db;
      $g_db->query("DROP TABLE $table");
    }
      
  }
  
?>