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

  $modifiers[] = "project";

  /**
   * Planning module.
   */
  class mod_resource_planning extends atkModule
  {
    function getNodes()
    {
      registerNode("resource_planning.resource_planning", array("add", "resplanning", "myplanning"));
      registerNode("resource_planning.employee_project", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      /* add menuitems */
      menuitem("resource_planning","","main", 1, 250);
      menuitem("-","","main", 1, 251);

      menuitem("resource_planning_planweekview", dispatch_url("resource_planning.resource_planning", "planweekview"), "resource_planning", 1);
      menuitem("resource_planning_planweekviewempl", dispatch_url("resource_planning.resource_planning", "planweekviewempl"), "resource_planning", 1);
    }

    function project_modifier(&$node)
    {
      $node->add(new atkOneToManyRelation("employees","resource_planning.employee_project", "projectid", AF_HIDE_LIST));
    }
  }
?>
