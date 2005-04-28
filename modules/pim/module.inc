<?php

  class mod_pim extends achievoModule
  {
    function getNodes()
    {
    }
    
    function getMenuItems()
    {
    }
    
    function pdaNodes()
    {
      return array("pim"=>dispatch_url("pim.pim","pim"));
    }
  }
  
?>