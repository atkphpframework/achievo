<?php
  $modifiers[] = "employee.userprefs";
  
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
    
    function userprefs_modifier(&$node)
    {
      userelation("atkOneToManyRelation");

      $node->add(new atkOneToManyRelation("pimitems","pim.userpimitems","userid",AF_FORCE_LOAD),"pim");
    }

  }
  
?>