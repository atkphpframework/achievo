<?php

  class mod_reports extends achievoModule
  {    
    function getNodes()
    {
      registerNode("reports.weekreport",array("report","view_all","view_managed"));
      registerNode("reports.hoursurvey",array("report","view_all","view_managed","save_for_all_users"));
      registerNode("reports.projectstatus",array("report", "any_user"));
      registerNode("reports.projectsurvey",array("report"));
    }

    function pdaNodes()
    {
      return array(
      "hours_hoursurvey"=>dispatch_url("reports.hoursurvey", "report"));
    }

    function getMenuItems()
    {
      $this->menuitem("reports", "","main",true,370);
      // TODO FIXME: hoursurvey is part of timereg.hours class. This should
      // be in a separate class in this module that delegates to the hours class.
      $this->menuitem("hours_hoursurvey", dispatch_url("reports.hoursurvey", "report"),"reports", array("reports.hoursurvey", "report"));
      $this->menuitem("projectsurvey", dispatch_url("reports.projectsurvey", "report"),"reports", array("reports.projectsurvey", "report"));
      $this->menuitem("weekreport", dispatch_url("reports.weekreport", "report"),"reports",array("reports.weekreport","report"));
      $this->menuitem("projectstatus", dispatch_url("reports.projectstatus", "report"), "reports", array("reports.projectstatus", "report"));
      $this->menuitem("todo_report", dispatch_url("reports.todo_report", "report"), "reports", array("reports.projectstatus", "report"));
    }   
  }

?>