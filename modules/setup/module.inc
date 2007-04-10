<?php

  class mod_setup extends achievoModule
  {

    function getMenuItems()
    {
     $this->menuitem("setup","","main",true,700);

     $this->menuitem("support","","main",true,800);
     $this->menuitem("achievo_docs", "http://www.achievo.org/wiki/Achievo", "support");
     $this->menuitem("-", "", "support");
     $this->menuitem("achievo_license", dispatch_url("setup.license","view"),"support");
     $this->menuitem("-", "", "support");
     $this->menuitem("online_support", "http://www.achievo.org/support", "support");
     $this->menuitem("sponsorship", "http://www.achievo.org/support/sponsor", "support");
     $this->menuitem("-", "", "support");
     $this->menuitem("achievo_home", "http://www.achievo.org/","support");

    }
  }

?>