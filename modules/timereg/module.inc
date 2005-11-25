<?php

  class mod_timereg extends achievoModule
  {
    function getNodes()
    {
      registerNode("timereg.hours", array("admin","add","edit","delete","lock","unlock", "any_user", "any_project")); 
      registerNode("timereg.hours_approve", array("admin","add","delete","approve","disapprove")); // niet
      registerNode("timereg.hours_lock", array("admin","add","delete")); // niet
      registerNode("timereg.workperiod", array("admin", "add", "edit", "delete"));
      registerNode("timereg.overtime_balance", array("edit", "delete"));
    }
    
    function getMenuItems()
    {
      $this->menuitem("time_registration", dispatch_url("timereg.hours", "admin"), "main",true,100);
      $this->menuitem("hours_admin", dispatch_url("timereg.hours", "admin"),"time_registration",true);
      $this->menuitem("-","","time_registration");
      $this->menuitem("hours_lock", dispatch_url("timereg.hours_lock", "admin"),"time_registration",array("timereg.hours_lock","admin"));
      if (is_allowed("timereg.hours_approve", "admin"))
        $this->menuitem("hours_approve", dispatch_url("timereg.hours_approve", "admin"),"time_registration",array("timereg.hours_approve","admin"));
      $this->menuitem("workperiod_admin", dispatch_url("timereg.workperiod", "admin"), "setup", array("timereg.workperiod", "admin"));
      //menuitem("overtimebalance_admin", dispatch_url("timereg.overtime_balance", "admin"), "setup", array("timereg.overtime_balance", "admin"));
    
      $this->menuitem("-","","main",true,120);
    }
    
    function getPimItems()
    {
      include_once(moduleDir("timereg")."pim.inc");
      return array("timeregistration"=>pimTimeReg());
    }  

    function getDependencies($mode)
    {
      return array("project", "employee");
    }

    function pdaNodes()
    {
      return array("time_registration_add"=>dispatch_url("timereg.hourspda", "add"),"time_registration_admin"=>dispatch_url("timereg.hourspda", "admin"));
    }
  }
  
?>