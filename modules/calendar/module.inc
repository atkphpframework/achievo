<?php

  class mod_calendar extends achievoModule
  {
    function getNodes()
    {
      registerNode("calendar.schedule", array("admin", "add", "edit", "delete", "edit_attendee"));
      registerNode("calendar.schedule_types", array("admin", "add", "edit", "delete"));
      registerNode("calendar.schedule_notes", array("admin", "add", "edit", "delete", "view"));
      registerNode("calendar.schedule_attendees", array("admin", "add", "edit", "delete", "view"));
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
  }

?>