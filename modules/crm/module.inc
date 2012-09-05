<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage crm
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */


  /**
   * Module file for the menu items and security
   * 
   * @author Sandy Pleyte <sandy@achievo.org>
   * @package achievo
   * @subpackage crm
   * @since 1.3.0
   */
  class mod_crm extends achievoModule
  {
    function getNodes()
    {
      registerNode("crm.campaign", array("admin", "add","edit","delete"));
      registerNode("crm.campaign_status", array("admin", "add","edit","delete"));
      registerNode("crm.campaign_type", array("admin", "add","edit","delete"));
      registerNode("crm.lead", array("admin", "add","edit","delete","convert"));
    }

    function getMenuItems()
    {
      $this->menuitem("sales",dispatch_url("quotation.quotation", "admin"),"main",true,500);
      $this->menuitem("campaigns",dispatch_url("crm.campaign", "admin"), "sales",array("quotation.quotation", "admin"),100);
      $this->menuitem("leads",dispatch_url("crm.lead", "admin"), "sales",array("quotation.quotation", "admin"),100);

      $this->menuitem("campaign_status",dispatch_url("crm.campaign_status", "admin"), "setup",array("crm.campaign_status", "admin"),200);
      $this->menuitem("campaign_type",dispatch_url("crm.campaign_type", "admin"), "setup",array("crm.campaign_type", "admin"),200);
      $this->menuitem("organization_status",dispatch_url("crm.organization_status", "admin"), "setup",array("organization.organization", "admin"),200);
      $this->menuitem("industry",dispatch_url("crm.industry", "admin"), "setup",array("organization.organization", "admin"),200);
      $this->menuitem("source",dispatch_url("crm.source", "admin"), "setup",array("organization.organization", "admin"),200);
      $this->menuitem("rate",dispatch_url("crm.rate", "admin"), "setup",array("organization.organization", "admin"),200);
      $this->menuitem("relation_type",dispatch_url("crm.relation_type", "admin"), "setup",array("organization.organization", "admin"),200);
    }

  }
   // Add plugins dir for the last viewed plugin
  atkimport("atk.ui.atksmarty");
  atkSmarty::addPluginDir(moduleDir("crm")."plugins/");

?>