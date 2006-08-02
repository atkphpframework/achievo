<?php

  $modifiers[] = "employee.userprefs";

  class mod_scheduler extends achievoModule
  {
    function getNodes()
    {
      registerNode("scheduler.scheduler", array("admin", "add", "edit", "delete", "edit_all"));
      registerNode("scheduler.scheduler_category", array("admin", "add", "edit", "delete"));
      registerNode("scheduler.scheduler_holidays", array("admin", "add", "edit", "delete","import","export","view_holidays"));
    }

    function getMenuItems()
    {
      $this->menuitem("scheduler", dispatch_url("scheduler.scheduler","admin"),"main",true,300);
      $this->menuitem("-","","main",true,320);
      $this->menuitem("scheduler_category", dispatch_url("scheduler.scheduler_category","admin"),"setup");
      $target_holidays = "dispatch.php?atknodetype=project.project&atkaction=admin&selectedcountry=[country]";
      $this->menuitem("scheduler_holidays", dispatch_url("scheduler.scheduler_holidays","admin"),"setup");
    }

    function getPimItems()
    {
      return array("schedule"=>"getPimListView");
    }

    function getPimListView()
    {
      include_once(moduleDir("scheduler")."pim.inc");
      return pimScheduler();
    }

    function getDependencies($mode)
    {
      return array("person");
    }

    function userprefs_modifier(&$node)
    {
      userelation("atkOneToOneRelation");
      $node->add(new atkOneToOneRelation("scheduler","scheduler.scheduler_userpreferences","userid",AF_ONETOONE_INTEGRATE|AF_ONETOONE_ERROR),"scheduler");
      $node->add(new atkOneToManyRelation("assistant", "scheduler.scheduler_userassistants", "userid"), "scheduler");
    }
  }

?>