<?php

  $modifiers[] = "employee.userprefs";

  class mod_scheduler extends achievoModule
  {
    function getNodes()
    {
      registerNode("scheduler.scheduler", array("admin", "add", "edit", "delete"));
      registerNode("scheduler.scheduler_category", array("admin", "add", "edit", "delete"));
      // Disabled, doesn't work yet
      //registerNode("scheduler.scheduler_holidays", array("admin", "add", "edit", "delete","import","export","view_holidays"));
      //registerNode("scheduler.scheduler_email_template", array("admin", "edit"));
    }

    function getMenuItems()
    {
      $this->menuitem("scheduler", dispatch_url("scheduler.scheduler","admin"),"main",true,300);
      $this->menuitem("-","","main",true,320);
      $this->menuitem("scheduler_category", dispatch_url("scheduler.scheduler_category","admin"),"setup");
      $target_holidays = "dispatch.php?atknodetype=project.project&atkaction=admin&selectedcountry=[country]";
      // Disabled, doesn't work yet
      //$this->menuitem("scheduler_holidays", dispatch_url("scheduler.scheduler_holidays","admin"),"setup");
      //$this->menuitem("scheduler_email_templates", dispatch_url("scheduler.scheduler_email_template","admin"),"setup");

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
      userelation("atkOneToManyRelation");
      $node->add(new atkOneToOneRelation("scheduler","scheduler.scheduler_userpreferences","userid",AF_ONETOONE_INTEGRATE|AF_ONETOONE_ERROR),"scheduler");
      $node->add(new atkOneToManyRelation("assistant", "scheduler.scheduler_userassistants", "userid"), "scheduler");
    }

    function search($expression)
    {
      $db = &atkGetDb();
      $res = array();
      $sql = "SELECT scheduler_scheduler.*,scheduler_dates.startdate as realstartdate,scheduler_dates.enddate as realenddate FROM scheduler_scheduler,scheduler_dates WHERE scheduler_scheduler.id=scheduler_dates.scheduler_id AND title LIKE '%$expression%' OR description LIKE '%$expression%' ORDER BY realstartdate";
      $nrows = $db->getrows($sql);
      $res["scheduler"] = $this->recLinks($nrows, "scheduler.scheduler");
      return $res;
    }

    function recLinks($recordset, $nodetype)
    {
      $node = &atkGetNode($nodetype);

      $res = array();

      $node = &atkGetNode($nodetype);
      if ($node->allowed("view"))
      {
        for($i=0, $_i=count($recordset); $i<$_i; $i++)
        {
          $item["title"] = date("d-m-Y H:i",$recordset[$i]["realstartdate"])." ".date("H:i",$recordset[$i]["realenddate"])." ".$recordset[$i]["title"];
          $item["url"] = dispatch_url($nodetype, "view", array("atkselector"=>$node->primaryKey($recordset[$i])));
          $res[] = $item;
        }
      }

      return $res;
    }
  }

?>