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
    var $m_cancontinue = true;
    
    var $m_resultmsg = array("ok" => "Installation completed",
                             "n/a" => "No installation required",
                             "failed" => "Installation failed",
                             "faileddep" => "Not installed (dependencies could not be resolved)",
                             "cyclicdep" => "Not installed (Cyclic dependency detected)",
                             "alreadyinstalled" => "Module was already installed and up to date",
                             "notfound" => "Not installed (Module could not be found)",
                             "patched" => "Patch applied succesfully");
  
    function setup()
    {
      $this->atkNode("setup");
    }
    
    function action_intro(&$handler)
    {
      $this->setupScreen("Introduction", 
                          "I am the Achievo setup script. I can do several things for you:
                          <ul>
                            <li>I can check for new versions of Achievo or one of your installed modules. 
                            <li>I can install an Achievo database if you don't already have one.
                            <li>I can upgrade an existing Achievo database if necessary.                            
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
    
    function action_dbcheck(&$handler)
    {            
      $res = $this->getDbStatus();
      
      if ($res==DB_SUCCESS)
      {
        // Database seems to be ok. Now we go check what version(s) we are running.
        return $this->action_versioncheck($handler);
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
      
      if ($res==DB_SUCCESS && atkconfig("database") == "mysql")
      {        
        // This can't be trusted. Mysql returns DB_SUCCESS even
        // if the user doesn't have access to the database. We only get an
        // error for this after we performed the first query.
        $g_db->table_names();  // this triggers a query
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
    
    function setupScreen($title, $output, $nextaction="", $params=array())
    {
      global $g_layout;
      
      $g_layout->initGui();
      $g_layout->ui_top($title);
      
      $form = '<div align="left"><b>'.$title.'</b><br><br>';
                               
      $form.= '<form action="'.($nextaction=="achievostart"?"index.php":"setup.php").'">'.
               session_form(SESSION_NEW). 
               '<input type="hidden" name="atkaction" value="'.$nextaction.'">'.
               $output;
               
      if (count($params))
      {
        foreach ($params as $key=>$value)
        {
          $form.='<input type="hidden" name="'.$key.'" value="'.$value.'">';
        }
      }
       
      if ($nextaction!="")
      {                        
        if ($nextaction=="achievostart")
        {
          $btn = "Start using Achievo";
        }
        else if ($nextaction==$this->m_action)
        {
          $btn = "Retry";
        }
        else
        {
          $btn = "Continue";
        }
        $form.= '<br><br><input type="submit" value="'.$btn.'"><br><br>';
      }
                             
      $form.= '</form></div>';
      $g_layout->output($form);
      $g_layout->ui_bottom();
      
    }
    
    function getTableNames()
    {
      global $g_db;
      
      $tablenames=array();
      $tables = $g_db->table_names();
      atk_var_dump($tables);
      for ($i=0, $_i=count($tables); $i<$_i; $i++)
      {
        $tablenames[] = $tables[$i]["table_name"];
      }                
      return $tablenames;   
    }
    
    /**
     * Check what version of the Achievo database we are using.
     */
    function action_versioncheck(&$handler)
    {         
      // Check if we have a connection first.
      $res = $this->getDbStatus();
      
      if ($res == DB_SUCCESS)
      {
        $tablenames = $this->getTableNames();                      
    
        if (count($tablenames)==0)
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
                                     continue, but keep this in mind: The installation process may fail if there are conflicting tables
                                     already present in the database, and it might even try to modify or drop existing tables. 
                                     <br><br><b>Continue at your own risk</b>.",
                                     "installdb");
        }
        if (in_array("versioninfo", $tablenames))
        {
          // Post 0.9.1 database, we can perform upgrades according to
          // versioninfo
          return $this->upgradeCheck();
        }
        else
        {
          // Apparently, this is a version from before 0.9.2, when we didn't have the
          // versioninfo table yet.
          // Perform some checks to determine what version this database might be.
          // We do this by looking at some tables that exist, that were created in
          // several Achievo versions.
          $orgversion = "";      
          
          if (in_array("projectactivity", $tablenames) && in_array("templateactivity", $tablenames))
          {
            // If these two tables exist, this is a 0.4 database.
            $orgversion = "0.4";
          }
          else if (in_array("todo_history", $tablenames) && in_array("employee_project", $tablenames))
          {
            $orgversion = "0.9.1";
          }
          else if (in_array("schedule", $tablenames) && in_array("schedule_types", $tablenames))
          {
            $orgversion = "0.9";
          }
          else if (in_array("hours_lock", $tablenames) && in_array("contract", $tablenames)
                  && !in_array("schedule", $tablenames))
          {
            $orgversion = "0.8";
          }
          else if (!in_array("hours_lock", $tablenames) && !in_array("contract", $tablenames)
                  && in_array("tpl_project", $tablenames))
          {
            $orgversion = "0.6";
          }
          
          if ($orgversion!="")
          {          
            return $this->setupScreen("Database version $orgversion detected",
                                      "The database you are currently using ('".atkconfig("databasename")."') appears to be from Achievo version $orgversion.
                                       <br>I can upgrade this database for you. 
                                       <br>Please make sure you backup the database
                                       first, because although I know what I'm doing, I'm just an open source script, and 
                                       I come with no warranty whatsoever. If I screw up the database, I've got nothing to 
                                       offer you but 'oops, sorry!'.
                                       <br>
                                       <br>In other words, backup your database first!
                                       <br>
                                       <br>If you've taken the necessary precautions, click the continue button to upgrade the database.",
                                      "legacyupgrade", array("orgversion"=>$orgversion));
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
        
      }
      else
      {
        return $this->handleDbStatus($res);
      }
    }
    
    function action_installdb(&$handler)
    {
      global $g_modules, $g_layout, $g_modifiers;
      
      // We must clear the modifiers. Modifiers will cause extra fields to be
      // installed, but this should be done by the extension modules
      // themselves. Not clearing the modifiers will lead to 'column already
      // exists' errors.
      $old_modifiers = $g_modifiers;
      $g_modifiers = array();
      
      $g_layout->initGui();                                     
      
      // We loop through the modules to install them. 
      // The setup module contains essential table needed for the installation process,
      // so we install this first.
      $result = array();
      $ok = true;
      
      if ($this->installModule("setup", $result))
      {      
        foreach (array_keys($g_modules) as $modname)
        {
          if ($modname!="setup") // we already installed setup when we started.
          {
            if (!$this->installModule($modname, $result, true))
            {
              atkdebug("Installation failed.");
              $ok = false;
              break;
            }
          }
        }        
      }
      else
      {
        $output = "I was unable to install the system table, so I cannot continue the installation. Please verify your database setup and try again.";
        $ok = false;
      }
      
      $output.=$this->renderResult($result);
      
      // Also show any reports.
      if (!$this->renderReport($output))
      {
        $ok = false;
      }
      
      // Restore modifiers.
      $g_modifiers = $old_modifiers;
      
      if ($ok)
      {
        return $this->setupScreen("Installation result", $output, "achievostart");
      }
      else
      {
        return $this->setupScreen("Installation result", $output);
      }
    }
    
    function renderReport(&$output)
    {  
      $ok = true;
      if (count($this->m_report))
      {
        foreach($this->m_report as $reportmsg)
        {  
          // We only show errors, important msgs and warnings.
          if (in_array($reportmsg["type"], array("error", "important", "warning")))
          {
            $output.="<br><br>";
            if ($reportmsg["type"]=="error")
            {
              $output.='<b>Installation failed!</b><br>';
              $output.= $reportmsg["msg"];
              $ok = false;
              break;
            }
            else if ($reportmsg["type"]=="important")
            {
              // Show this to the user
              $output.="<b>Important:</b><br>";
              $output.= $reportmsg["msg"];
            }
            else if ($reportmsg["type"]=="warning")
            {
               // Show this to the user
              $output.="<b>Warning:</b><br>";
              $output.= $reportmsg["msg"];
            }          
            $output.="<br>";
          }
        }   
      }
      return $ok;
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
        $result[$modname] = "alreadyinstalled";
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
      if ($this->needsInstall($modname))
      {      
        $this->includeInstallFile($modname, $installfile);
        if (!$this->m_cancontinue)
        {  
          //  Something went wrong.
          atkdebug("Detected an error during the installation of $modname");
          $result[$modname] = "failed";
          return false;
        }
        else
        {
          // if we get here, everything should be installed.
          atkdebug("Installed $modname");
          $result[$modname] = "ok";
          return true;
        }

      }

      // If we get here, there was nothing to install for this module.
      $result[$modname] = "n/a";
      return true;
      
    }
    
    function includeInstallFile($modname, $installfile)
    {
      // give included file access to $g_db and $this ("$setup");
      global $g_db;        
      $setup = &$this;
        
      // also set 'current module' to the module were now installing. This will 
      // be used by setup functions called from the install file.
      $this->m_currentmodule = $modname;      
      include($installfile);
    }
    
    function needsInstall($modname)
    {
      $installfile = moduleDir($modname)."install/install.inc";   
      return (file_exists($installfile));
    }
    
    function hasVersionInfo()
    {
      static $s_hasversioninfo=false;
      if (!$s_hasversioninfo) // While we have no versioninfo, we keep trying.
      {
        $tablenames = $this->getTableNames();        
        $s_hasversioninfo = (in_array("versioninfo", $tablenames));
      }      
      return $s_hasversioninfo;
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
      
      if ($this->hasVersionInfo())
      {                
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
      }
      return $result;
    }
    
    function installNode($nodename)
    {
      // Check if previous calls all went ok. If one went wrong, we should stop.
      if (!$this->m_cancontinue) return false;
      
      atkdebug("Installnode call for $nodename");
      
      $node = &getNode($nodename);            
      
      if (is_object($node))
      {
        // We have to check if the table already exists. If it exists, we may need to
        // add columns to it.
        global $g_db;        
        $meta = $g_db->metadata($node->m_table, true);                
        atk_var_dump($meta, 'metadata '.$this->m_table);
        
        if ((!is_array($meta)||count($meta)<=0) && in_array($node->m_table, $this->getTableNames()))
        {
          // No metadata. It is unsafe to continue the installation.
          atkdebug("Table exists, but metadata is not present. I cannot continue installation without metadata");
          $this->report("I could not read metadata from the database, so I 
                         cannot continue the installation. Please verify your 
                         database setup and try again. 
                         <br>If you cannot find the problem, please send a 
                         message to the Achievo mailinglist.", "error");
          return false;
        }
        else
        {
          // Table does not exist.
        }
        
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
          $this->report("Your database is not supported by the automatic 
                        installscript. If you think it should be, please 
                        send a message to the Achievo mailinglist.", "error");
          return false;
        }
      }
      else
      {
        atkdebug("setup::installNode: node $node not found.");
        return false;
      }
      return true;
    }
    
    function columnExists($table, $column)
    {
      global $g_db;
      $meta = $g_db->metadata($table, true);                
      for ($i=0, $_i=count($meta); $i<$_i; $i++)
      {
        if (strtolower($meta[$i]["name"]) == strtolower($column))
        {
          return true;
        }
      }
      return false;
    }
    
    function setVersion($version, $module="")
    {
      // if something went wrong, we should ignore the setVersion call.
      if (!$this->m_cancontinue) return false;   
      
      if ($module=="") $module = $this->m_currentmodule;
      
      if ($module!="" && $this->hasVersionInfo())
      {
        $versioninfo = &getNode("setup.versioninfo");
        $recs = $versioninfo->selectDb("module='".$module."'");
        
        if (count($recs)>0)
        {
          // rec already exists. Update.          
          $recs[0]["version"] = $version;
          $versioninfo->updateDb($recs[0]);
        }
        else
        {
          $newrec = array("module"=>$module, "version"=>$version);
          // rec doesn't exist. Insert.          
          $versioninfo->addDb($newrec);
        }
      }
    }      
    
    /**
     * patch/install scripts can use this method for providing progress 
     * information.
     *
     * There are several types of progress information:
     * info - Normal progress information. 
     * important - Important information that the user running the convert
     *             script *must* read.
     * warning - Indicates a warning during the conversion phase, which is
     *           not critical enough to stop execution, but must still be
     *           reported to the user.
     * error - Indicates an error during conversion. The installer will not
     *         run any further script when an error is encountered.
     */
    function report($progressmsg, $type="info")
    {
      $this->m_report[] = array("msg"=>$progressmsg, "type"=>$type);
      // Regardless of what we output later to the user, everything
      // reported by the scripts is put in the debuglog.
      atkdebug($type.": ".$progressmsg);
      
      // If an error is reported by some subprocess, we should
      // mark this so we don't continue.
      if ($type=="error")
      {
        $this->m_cancontinue=false;
      }
      
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
      if ($default!="") $sql.= " DEFAULT '$default'";
      $g_db->query($sql);        
    }
    
    function alterColumn($table, $col, $newname, $type, $nullable=true, $default="")
    {
      global $g_db;
      $sql = "ALTER TABLE $table CHANGE $col $newname $type";
      if (!$nullable) $sql.=" NOT NULL";
      if ($default!="") $sql.= " DEFAULT '$default'";
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
    
    function action_legacyupgrade(&$handler)
    {          
      global $g_db; 
      
      $orgversion = $this->m_postvars["orgversion"];      
      $history = array("0.4", "0.6", "0.8", "0.9", "0.9.1", "0.9.2");
      
      $startidx = array_search($orgversion, $history, true);
      
      if ($startidx!==false && $startidx!==NULL) // in php 4.2.0 the result switched from null to false
      {
        $ok = true;
        for ($script=$startidx, $_script=count($history); $script<$_script-1 && $ok; $script++)
        {          
          $scriptname = "upgrade/convert-".$history[$script]."-to-".$history[$script+1].".inc";
          $output.="<br><br>Start of conversion from ".$history[$script]." to ".$history[$script+1].".";
          
          // Upgrade scripts might take a while. We'll set the timelimit to 0, 
          // so the script can run as long as it takes.
          set_time_limit(0); 
          
          // Like with install db, we pass the upgrade scripts a pointer
          // to $setup and $g_db.
          $setup = &$this;
          
          // clear the report.
          $this->m_report = array();
          
          include_once($scriptname);
          
          // first check if there are any atk errors.
          global $g_error_msg;
          if (count($g_error_msg))
          {
            $output.='<br><br><b>Conversion failed!</b><br>';
            $output.= implode("<br>", $g_error_msg);
            $ok = false;
          }
          else
          {
            // Also show any reports.
            if (!$this->renderReport($output))
            {
              $ok = false;
            }
          }
          if ($ok)
          {
            $output.= "<br>Conversion of ".$history[$script]." to ".$history[$script+1]." complete.";
          }
          else
          {
            $output.= "<br><br>Conversion of ".$history[$script]." to ".$history[$script+1]." failed, stopping conversion. 
                       <br><br><b>Please restore your backup</b>. Contact the Achievo developers who might help you out in finding the cause of failure.";
          }
        }
      }
      else
      {
      }
      
      if ($ok)
      {
        // finally perform post 0.9.1 installation stuff. 
        // Legacy upgrade will upgrade to 0.9.2; after that, we can execute
        // the auto-upgrade, which will install necessary patches.
        include_once("./version.inc");
        $output.="<br><br>Start of patch from ".$history[count($history)-1]." to ".$achievo_version;
        
        $result = array();
        if ($this->upgradedb($result))
        {
          $output.=$this->renderResult($result);
          $this->renderReport($output);
        }
      } 
      $this->setupscreen("Upgrading from version $orgversion",
                         "Your database is upgraded from version $orgversion to version $achievo_version. The upgrade is performed in several small steps.
                          <br>Below is the result of each of these steps. Please review if there are any errors.
                          <br>Also take note of any important messages.".$output,
                         "achievostart");
      
    }
    
    function renderResult($result)
    {
    	    	
      global $g_layout;
      $output="";
      if (count($result)>0)
      {        
        $output = $g_layout->data_top();
        $output.= $g_layout->tr_top();
        $output.= $g_layout->ret_td_datatitle("Module");
        $output.= $g_layout->ret_td_datatitle("Result");
        $output.= $g_layout->tr_bottom();
        foreach ($result as $modname => $result)
        {
          $output.= $g_layout->tr_top();
          $output.= $g_layout->ret_td($modname);          
          $output.= $g_layout->ret_td((!empty($this->m_resultmsg[$result])?$this->m_resultmsg[$result]:$result));
          $output.= $g_layout->tr_bottom();
        }
        $output.= $g_layout->data_bottom();
      }
      return $output;
    }
    
    function action_upgradedb(&$handler)
    {
      global $g_layout;
      
      $g_layout->initGui();
      
      $result = array();
      
      if ($this->upgradedb($result))
      {
        // things have been installed. Show result.
        $output = $this->renderResult($result);     
        $this->renderReport($output);

        if ($this->m_cancontinue)
        {
          return $this->setupScreen("Upgrade result", $output, "achievostart");
        }
        else
        {
          return $this->setupScreen("Upgrade result", $output);
        }        
      }
      else
      {    
        return $this->setupScreen("Database is up to date", "Your database appears to be up to date. No upgrade is necessary.", "achievostart"); 
      }    
    }
    
    function upgradedb(&$result)
    {
      global $g_modules;            

      $installedmodules = $this->getInstalledModules();
      
      $stufftodo = false;
      
      foreach ($g_modules as $modname=>$moduledir)
      {
        if (!in_array($modname, array_keys($installedmodules)) && $this->needsInstall($modname))
        {
          // module is not installed yet.           
          $stufftodo = true;
          
          if (!$this->installModule($modname, $result, true))
          {
            atkdebug("Installation failed.");
            $this->m_cancontinue = false;
            break;
          }     
        }
        else
        {
          // module is installed. Check if we need to patch.
          $availablepatches = array();
          
          if ($handle=@opendir($moduledir."install"))
          {
            while ($file = readdir($handle))
            {            
              if (substr($file,0,5)=="patch")
              {            
                $patchno = substr($file, 6, -4); // Patchnumber is between 'patch-' and '.inc'
                if ($patchno>$installedmodules[$modname])
                {
                  $availablepatches[] = $patchno;
                }
              }
            }
            closedir($handle);
            
            if (count($availablepatches))
            {
              $stufftodo = true;
              // We need to install patches.
              natcasesort($availablepatches); // We must install patches in the correct order.
             
              for($i=0, $_i=count($availablepatches); $i<$_i; $i++)
              {
                $patchfile = moduleDir($modname)."install/patch-".$availablepatches[$i].".inc";
                $this->includeInstallFile($modname, $patchfile);
                $this->setVersion($availablepatches[$i]);
                $result[$modname] = "patched";
              }
            }
          }
        }
      }
      
      return $stufftodo;
    }
    
    function upgradeCheck()
    {
      global $g_modules, $g_layout;
      
      $g_layout->initGui();

      $installedmodules = $this->getInstalledModules();            
      
      $installs = array();
      $patches = array();
      
      foreach ($g_modules as $modname=>$moduledir)
      {
        if (!in_array($modname, array_keys($installedmodules)) && $this->needsInstall($modname))
        {
          // module is not installed yet.         
          $installs[] = $modname; 
        }
        else
        {
          // module is installed. Check if we need to patch.
          $availablepatches = array();
          if ($handle=@opendir($moduledir."install"))
          {
            while ($file = readdir($handle))
            {            
              if (substr($file,0,5)=="patch")
              {            
                $patchno = substr($file, 6, -4); // Patchnumber is between 'patch-' and '.inc'
                if ($patchno>$installedmodules[$modname])
                {
                  $availablepatches[] = $patchno;
                }
              }
            }
            closedir($handle);
            
            if (count($availablepatches))
            {
              // We need to install patches.
              natcasesort($availablepatches); // We must install patches in the correct order.
              $patches[$modname] = $availablepatches;
            }
          }
        }
      }
      
      if (count($installs)||count($patches))
      {
        // things must be installed or patched
        $output = $g_layout->data_top();
        $output.= $g_layout->tr_top();
        $output.= $g_layout->ret_td_datatitle("Module");
        $output.= $g_layout->ret_td_datatitle("Necessary action");
        $output.= $g_layout->tr_bottom();
        foreach ($installs as $modname)
        {
          $output.= $g_layout->tr_top();
          $output.= $g_layout->ret_td($modname);          
          $output.= $g_layout->ret_td("Install");
          $output.= $g_layout->tr_bottom();
        }
        foreach ($patches as $modname=>$modpatches)
        {
          $output.= $g_layout->tr_top();
          $output.= $g_layout->ret_td($modname);          
          $output.= $g_layout->ret_td(count($modpatches)." patch".(count($modpatches)>1?"es":""));
          $output.= $g_layout->tr_bottom();
        }
        $output.= $g_layout->data_bottom();
        return $this->setupScreen("Database needs to be updated", "The following actions must be taken in order to update the Achievo database:<br><br>".$output."<br>Please click the 'continue' button below to start the update.", "upgradedb");
      }
      else      
      {    
        return $this->setupScreen("Database is up to date", "Your database appears to be up to date. No upgrade is necessary.", "achievostart");          
      }
    }
      
  }
  
?>