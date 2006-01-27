<?php

  class mod_organization extends achievoModule
  {
    function getNodes()
    {
      registerNode("organization.organization", array("admin", "add", "edit", "delete", "document"));
      registerNode("organization.contact", array("admin", "add","edit","delete"));
      registerNode("organization.contracts", array("admin","add","edit","delete"));
      registerNode("organization.contracttype", array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("organizationmanagement",dispatch_url("organization.organization", "admin"),"main",array("organization.organization","admin"),500);
      $this->menuitem("organization_admin", dispatch_url("organization.organization", "admin"),"organizationmanagement", array("organization.organization","admin"));      
      $this->menuitem("contracts", dispatch_url("organization.contracts","admin"),"organizationmanagement", array("organization.contracts","admin"));

      $this->menuitem("contracttype", dispatch_url("organization.contracttype","admin"),"setup", array("organization.contracttype","admin"));
      $this->menuitem("-","","main",true,720);
      $this->menuitem("-","","organizationmanagement",true);
      $this->menuitem("contact_admin", dispatch_url("organization.contact", "admin"),"organizationmanagement", array("organization.contact","admin"));
    }
    
    function getDependencies($mode)
    {
      return array("person");
    }

    function search($expression)
    {
      $res = array();
      $custnode = &getNode("organization.organization");
      $recs = $custnode->searchDb($expression);
            
      $res["organizations"] = $this->recLinks($recs, "organization.organization");

      $contactnode = &getNode("organization.contact");
      $res["contacts"] = $this->recLinks($contactnode->searchDb($expression), "organization.contact");

      $contractnode = &getNode("organization.contracts");
      $res["contracts"] = $this->recLinks($contractnode->searchDb($expression), "organization.contracts");

      return $res;
    }
  }
?>
