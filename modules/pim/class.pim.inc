<?php

  class pim extends atkNode
  {
    function pim()
    {
      $this->atkNode("pim",NF_NO_SECURITY);
    }

    function action_pim()
    {
      global $g_user;

      $me = $g_user["name"];
      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $page = &$this->getPage();
        $page->head(text("title_pim"));
        $page->body(); 
          
        $res = '<br>';
  
        $pimItems = atkHarvestModules("getPimItems");
  
        foreach ($pimItems as $name => $contents)
        {
          $res.= '<b>'.text('pim_'.$name).'</b><br><br>';
          $res.= $contents;
          $res.= '<br><br>';
        }
  
        $res.='<br>';
  
        $page->addContent($ui->renderBox(array("title"=>text("title_pim"),
                                                 "content"=>$res)));
      }
      else
      {
        atkerror("ui object failure"); 
      }

    }
    
    function action_adminpim()
    {
      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $page = &$this->getPage();
        $page->head(text("title_pim_adminpim"));
        $page->body(); 
        
        
        $content.='<br><div align="left">Welcome to Achievo.  
                   <br>You are currently logged in as administrator. <b>This account 
                   is for setup-purposes only</b>.
                   <br><br>As this is not a real account, do not use features 
                   like timeregistration etc.
                   <br>It is best to use this account only when you run Achievo 
                   for the first time, or when you need to upgrade.
                   
                   <br><br><b>First time usage</b>
                   <br>If this is the first time you login to Achievo, '.
                   href(dispatch_url("employee.employee", "admin"), "click here", SESSION_NESTED).
                   ' to create the first real user account. 
                   <br>Then, relogin as the new user.
                   <br>
                   <br><b>Upgrades and module installations</b>
                   <br>If you need to upgrade or if you installed a new module, use the
                   \'setup\' link in the top frame to update the database.</div><br><br>';      
         $page->addContent($ui->renderBox(array("title"=>text("title_pim_adminpim"),
                                                 "content"=>$content)));
      }
      else
      {
        atkerror("ui object failure"); 
      }
    }
  }

?>
