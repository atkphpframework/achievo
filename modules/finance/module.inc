<?php

  /**
   * Example module.
   */

  class mod_finance extends achievoModule
  {
    function getNodes()
    {
      /* register nodes */
      registerNode("finance.rates", array("admin", "add", "edit", "delete"));
      registerNode("finance.finance_customer", array("admin", "add", "edit", "delete"));
      registerNode("finance.finance_project", array("admin", "add", "edit", "delete"));
      registerNode("finance.billing_project", array("admin", "add", "edit", "delete"));
      registerNode("finance.currency", array("admin", "add", "edit", "delete"));
      registerNode("finance.bill", array("admin", "add", "edit", "delete", "generatebill", "viewbill"));
      registerNode("finance.bill_line", array("admin", "add", "edit", "delete", "createbill"));
    }

    function getMenuItems()
    {
      /* add menuitems */
      //$target_bill = "dispatch.php?atknodetype=finance.bill&atkaction=admin&selectedprojectid=[id]";
      menuitem("finance","","main",1,675);
      menuitem("-","","main",1,676);

      menuitem("currency", dispatch_url("finance.currency", "admin"),"finance", 1);
      menuitem("-","","finance");
      menuitem("rates", dispatch_url("finance.rates", "admin"),"finance", 1);
      menuitem("finance_customer", dispatch_url("finance.finance_customer", "admin"),"finance", 1);
      menuitem("finance_project", dispatch_url("finance.finance_project", "admin"),"finance", 1);
      menuitem("-","","finance");
      menuitem("bill", dispatch_url("finance.bill", "admin"),"finance", 1);
    }
  }

?>