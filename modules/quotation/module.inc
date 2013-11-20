<?php
  $modifiers[] = "organization.organization";

  class mod_quotation extends achievoModule
  {

    function getNodes()
    {
      registerNode("quotation.quotation",array("admin","add","edit","delete"));
      registerNode("quotation.payment",array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("sales",dispatch_url("quotation.quotation", "admin"),"main",true,500);
      $this->menuitem("quotation", dispatch_url("quotation.quotation", "admin"), "sales",array("quotation.quotation", "admin"),685);
      $this->menuitem("payment", dispatch_url("quotation.payment", "admin"), "setup", array("quotation.payment", "admin"));
    }

    function getDescription()
    {
      return "Administration of quotations for customers.";
    }

    function getPimItems()
    {
      return array("quotations"=>"getQuotations",
                   "mypipeline"=>"getPipeline");
    }

    function getQuotations()
    {
      include_once(moduleDir("quotation")."pim.inc");
      return pimQuotation();
    }

    function getPipeline()
    {
      include_once(moduleDir("quotation")."pim.inc");
      return pimMyPipeline();
    }

  /**
     * Modifier on  Organization node to show the quotations of an organization
     * @param atkNode $node Node
     */
    function organization_organization_modifier($node)
    {
      userelation('atkonetomanyrelation');
      $node->add(new atkOneToManyRelation("quotations","quotation.quotation","organization_id",AF_HIDE_LIST),"extra");
    }

  }

?>
