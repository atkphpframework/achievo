<?php

  class mod_organization extends achievoModule
  {
    function getNodes()
    {
      registerNode("organization.customer", array("admin", "add", "edit", "delete")); // niet
      registerNode("organization.contact", array("add","edit","delete"));
      registerNode("organization.contracts", array("admin","add","edit","delete"));
      registerNode("organization.contracttype", array("admin","add","edit","delete"));
    }
    
    function getMenuItems()
    {
      menuitem("customermanagement",dispatch_url("organization.customer", "admin"),"main",array("organization.customer","admin"),500);
      menuitem("customer_admin", dispatch_url("organization.customer", "admin"),"customermanagement", array("organization.customer","admin"));
      menuitem("contracts", dispatch_url("organization.contracts","admin"),"customermanagement", array("organization.contracts","admin"));
      
      menuitem("contracttype", dispatch_url("organization.contracttype","admin"),"setup", array("organization.contracttype","admin"));
      menuitem("-","","main",true,720);         
    }
    
    function search($expression)
    {
      $res = array();
      $custnode = &getNode("organization.customer");
      $res["customers"] = $custnode->searchDb($expression);   
        
      $contactnode = &getNode("organization.contact");
      $res["contacts"] = $contactnode->searchDb($expression);   
      
      return $res;
    }
  }
?>
