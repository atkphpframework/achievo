<?php
 /**
   * Module Resource planning
   *
   * $Id$
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision$
   *
   */

  /**
   * Planning module.
   */
  class mod_resource_planning extends achievoModule
  {
    function getNodes()
    {
      registerNode("resource_planning.resource_planning", array("add", "resplanning", "myplanning", "gant"));
    }

    function getMenuItems()
    {
      /* add menuitems */
      menuitem("resource_planning","","main", 1, 250);
      menuitem("-","","main", 1, 251);

      menuitem("resource_planning_planweekview", dispatch_url("resource_planning.resource_planning", "planweekview"), "resource_planning", array("resource_planning.resource_planning", "resplanning"));
      menuitem("resource_planning_planweekviewempl", dispatch_url("resource_planning.resource_planning", "planweekviewempl"), "resource_planning", array("resource_planning.resource_planning", "myplanning"));
      menuitem("resource_planning_gantview", dispatch_url("resource_planning.resource_planning", "gantview"), "resource_planning", array("resource_planning.resource_planning", "gant"));
    }

    function getPimItems()
    {
      include_once(moduleDir("resource_planning")."pim.inc");      
      return array("planning"=>pimPlanning());
    }
  }
?>
