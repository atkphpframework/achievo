<?php

  class mod_reports extends achievoModule
  {
    function getNodes()
    {
      registerNode("reports.weekreport",array("report","view_all"));
      registerNode("reports.hoursurvey",array("report","view_all"));
      registerNode("reports.reportorganization",array("view"));
    }

    function getMenuItems()
    {
      menuitem("reports", "","main",true,370);
      // TODO FIXME: hoursurvey is part of timereg.hours class. This should
      // be in a separate class in this module that delegates to the hours class.
      menuitem("hours_hoursurvey", dispatch_url("reports.hoursurvey", "report"),"reports", array("reports.hoursurvey", "report"));
      menuitem("weekreport", dispatch_url("reports.weekreport", "report"),"reports",array("reports.weekreport","report"));
      $target_organization2 = "dispatch.php?atknodetype=reports.reportorganization&atkaction=view&organizationid=[id]";
      menuitem("organizationreport",  dispatch_url("organization.organization", "select", array("atktarget" => atkurlencode($target_organization2))), "reports", array("reports.reportorganization", "view"));
    }
  }

?>