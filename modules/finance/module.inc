<?php

  /**
   * Example module.
   */

  class finance extends atkModule
  {
    function getNodes()
    {
      /* register nodes */
      registerNode("finance.rates", array("admin", "add", "edit", "delete"));
      registerNode("finance.finance_customer", array("admin", "add", "edit", "delete"));
      registerNode("finance.finance_project", array("admin", "add", "edit", "delete"));
      registerNode("finance.billing_project", array("admin", "add", "edit", "delete"));
      registerNode("finance.currency", array("admin", "add", "edit", "delete"));
    }
    
    function getMenuItems()
    {			      
      /* add menuitems */
      menuitem("finance","","main",1,675);
      menuitem("-","","main",1,676);

      menuitem("currency", dispatch_url("finance.currency", "admin"),"finance", 1);
      menuitem("-","","finance");      
      menuitem("rates", dispatch_url("finance.rates", "admin"),"finance", 1);
      menuitem("finance_customer", dispatch_url("finance.finance_customer", "admin"),"finance", 1);
      menuitem("finance_project", dispatch_url("finance.finance_project", "admin"),"finance", 1);
      menuitem("-","","finance");      
      menuitem("billing_project", dispatch_url("finance.billing_project", "admin"),"finance", 1);

    }
  }

?>
