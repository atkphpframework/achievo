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
                          "setup");
    }
    
    function action_setup()
    {
      global $g_db;
      
      // We don't want the db class to display error messages, because
      // we handle the error ourselves.
      $g_db->m_haltonerror = false;            
      
      $this->handleDbStatus($g_db->connect());            
            
    }
    
    /** 
     * Handles the database status. Because of the way some databases
     * handle connections, DB_SUCCESS needs to be confirmed by 
     * performing a query. If handleDbStatus is not called with
     * confirmed=true, it will perform this query and confirm itself.
     */
    function handleDbStatus($dbstatus, $confirmed=false)
    {    
      switch($dbstatus)
      {
        case DB_SUCCESS:      
          if (!$confirmed)
          {
            global $g_db;
            // This can't be trusted. Mysql returns DB_SUCCESS even
            // if the user doesn't have access to the database. We only get an
            // error for this after we performed the first query.
            $res = $g_db->getrows("show tables");                        
            return $this->handleDbStatus($g_db->_translateError($g_db->m_errno), true);            
          }
          return $this->setupScreen("hmm","successful");
          
          // In the case we don't have a DB_SUCCESS now, we fall through
          // the other cases (we don't break), so we can handle the error.
        case DB_UNKNOWNHOST:
          return $this->setupScreen("Unable to connect to database-server", "host wrong");
          
        case DB_UNKNOWNDATABASE:
          return $this->setupScreen("Database does not exist", "database doesn't exist");
          
        case DB_ACCESSDENIED_USER:
          return $this->setupScreen("Database username or password incorrect", "user wrong");
          
        case DB_ACCESSDENIED_DB:
          return $this->setupScreen("User not allowed to connect to database", 
                                    "user may not connect to db");
              
        default: // catches any errors we don't know of, and also DB_UNKNOWNERROR
          return $this->setupScreen("can't do anything");
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
        $form.= '<br><br> <input type="submit" value="continue"><br><br>';
      }
               
      $form.= '</form>';
      $g_layout->output($form);
      $g_layout->ui_bottom();
      
    }
        
  }
  
?>