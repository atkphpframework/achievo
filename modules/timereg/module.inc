<?php

  class mod_timereg extends achievoModule
  {
    function getNodes()
    {
      registerNode("timereg.hours", array("admin","add","edit","delete","lock","unlock")); // niet
      registerNode("timereg.hours_lock", array("admin","add","delete")); // niet
    }
    
    function getMenuItems()
    {
      menuitem("time_registration", dispatch_url("timereg.hours", "admin"), "main",true,100);
      menuitem("hours_admin", dispatch_url("timereg.hours", "admin"),"time_registration",true);
      menuitem("-","","time_registration");
      menuitem("hours_lock", dispatch_url("timereg.hours_lock", "admin"),"time_registration",array("timereg.hours_lock","admin"));
    
      menuitem("-","","main",true,120);
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
  }
  
?>