<?php

  $modifiers[] = "employee.userprefs";

  class mod_calendar extends achievoModule
  {
    function getNodes()
    {
      registerNode("calendar.schedule", array("admin", "add", "edit", "delete", "edit_all"));
      registerNode("calendar.schedule_types", array("admin", "add", "edit", "delete"));
      registerNode("calendar.holidays", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("schedule", dispatch_url("calendar.schedule","admin"),"main",array("calendar.schedule", "admin"),300);
      $this->menuitem("-","","main",array("calendar.schedule", "admin"),320);
      $this->menuitem("schedule_types", dispatch_url("calendar.schedule_types","admin"),"setup", array("calendar.schedule_types", "admin"));
      $this->menuitem("holiday_admin", dispatch_url("calendar.holidays","admin"),"setup", array("calendar.holidays", "admin"));
    }

    function getPimItems()
    {
      return array("schedule"=>"getPimListView");
    }

    function getPimListView()
    {
      if (!(browserInfo::detectPDA()))
      {
        include_once(moduleDir("calendar")."pim.inc");
        return pimSchedule();
      }
    }


    function getDependencies($mode)
    {
      return array("person");
    }

    function userprefs_modifier(&$node)
    {
      useattrib("atklistattribute");
      $usernode = &atkGetNode("employee.employee");
      $node->add(new atkListAttribute("calendar_defaultuserview",
                                      array($usernode->descriptor(getUser()),
                                            atktext("allusers")),
                                      array("",
                                            "*"), AF_LIST_NO_NULL_ITEM|AF_NO_TRANSLATION), "calendar");
    }
  }

?>
