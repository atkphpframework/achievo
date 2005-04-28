<?php

  class mod_reports extends achievoModule
  {
    function getNodes()
    {
      registerNode("reports.weekreport",array("report","view_all"));
      registerNode("reports.hoursurvey",array("report","view_all"));
    }
    
    function pdaNodes()
    {
      return array(
      "hours_hoursurvey"=>dispatch_url("reports.hoursurvey", "report"));
    }

    function getMenuItems()
    {
      menuitem("reports", "","main",true,370);
      // TODO FIXME: hoursurvey is part of timereg.hours class. This should
      // be in a separate class in this module that delegates to the hours class.
      menuitem("hours_hoursurvey", dispatch_url("reports.hoursurvey", "report"),"reports", array("reports.hoursurvey", "report"));
      menuitem("weekreport", dispatch_url("reports.weekreport", "report"),"reports",array("reports.weekreport","report"));
    }
  }

?>