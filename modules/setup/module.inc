<?php

  class mod_setup extends achievoModule
  {
    
    function getMenuItems()
    {
     menuitem("setup","","main",true,700);    
   
     menuitem("support","","main",true,800);
     menuitem("achievo_docs", "http://www.achievo.org/support/manual", "support");
     menuitem("-", "", "support");
     menuitem("achievo_license", "license.php", "support");
     menuitem("-", "", "support");
     menuitem("online_support", "http://www.achievo.org/support", "support");
     menuitem("sponsorship", "http://www.achievo.org/support/sponsor", "support");
     menuitem("-", "", "support");
     menuitem("achievo_home", "http://www.achievo.org/","support");  
 
    }
  }
  
?>