<?php

  class mod_calendar extends achievoModule
  {
    function getNodes()
    {
      registerNode("calendar.schedule", array("admin", "add", "edit", "delete", "edit_all"));
      registerNode("calendar.schedule_types", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      menuitem("schedule", dispatch_url("calendar.schedule","admin"),"main",true,300);
      menuitem("-","","main",true,320);
      menuitem("schedule_types", dispatch_url("calendar.schedule_types","admin"),"setup");
    }

    function getPimItems()
    {
      include_once(moduleDir("calendar")."pim.inc");
      return array("schedule"=>pimSchedule());
    }

    function getDependencies($mode)
    {
      return array("person");
    }
  }

?>