<?php

  require_once("atk/modules/class.atkmodule.inc");

  /**
   * Example module.
   */

  class finance extends atkModule
  {
    function finance()
    {
      /* register nodes */
      $this->registerNode("rates", array("admin", "add", "edit", "delete"));
      $this->registerNode("finance_customer", array("admin", "add", "edit", "delete"));
      $this->registerNode("finance_project", array("admin", "add", "edit", "delete"));
      $this->registerNode("billing_project", array("admin", "add", "edit", "delete"));
      $this->registerNode("currency", array("admin", "add", "edit", "delete"));

			
      
      /* add menuitems */
      $this->menuitem("-");      
      $this->menuitem("finance");

      $this->menuitem("currency", dispatch_url("finance.currency", "admin"),"finance", 1);
      $this->menuitem("-","","finance");      
      $this->menuitem("rates", dispatch_url("finance.rates", "admin"),"finance", 1);
      $this->menuitem("finance_customer", dispatch_url("finance.finance_customer", "admin"),"finance", 1);
      $this->menuitem("finance_project", dispatch_url("finance.finance_project", "admin"),"finance", 1);
      $this->menuitem("-","","finance");      
      $this->menuitem("billing_project", dispatch_url("finance.billing_project", "admin"),"finance", 1);

    }
  }

?>