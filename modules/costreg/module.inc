<?php

  class mod_costreg extends achievoModule
  {
    function getNodes()
    {
      registerNode("costreg.costregistration", array("admin", "add", "edit", "delete", "costreport", "view_all"));
    }
    
    function getMenuItems()
    {
      menuitem("costregistration_costreport", dispatch_url("costreg.costregistration", "costreport"),"reports");
      menuitem("costregistration_admin", dispatch_url("costreg.costregistration", "admin"),"main",true,200);    
      menuitem("-","","main",true,220);
    }
  }
  
?>