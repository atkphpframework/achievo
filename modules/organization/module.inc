<?php

  class mod_organization extends achievoModule
  {
    function getNodes()
    {
      registerNode("organization.organization", array("admin", "add", "edit", "delete")); // niet
      registerNode("organization.contact", array("add","edit","delete"));
      registerNode("organization.contracts", array("admin","add","edit","delete"));
      registerNode("organization.contracttype", array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      menuitem("organizationmanagement",dispatch_url("organization.organization", "admin"),"main",array("organization.organization","admin"),500);
      menuitem("organization_admin", dispatch_url("organization.organization", "admin"),"organizationmanagement", array("organization.organization","admin"));
      menuitem("contracts", dispatch_url("organization.contracts","admin"),"organizationmanagement", array("organization.contracts","admin"));

      menuitem("contracttype", dispatch_url("organization.contracttype","admin"),"setup", array("organization.contracttype","admin"));
      menuitem("-","","main",true,720);
    }
    
    function getDependencies($mode)
    {
      return array("person");
    }

    function search($expression)
    {
      $res = array();
      $custnode = &getNode("organization.organization");
      $res["customers"] = $custnode->searchDb($expression);

      $contactnode = &getNode("organization.contact");
      $res["contacts"] = $contactnode->searchDb($expression);

      return $res;
    }
  }
?>
