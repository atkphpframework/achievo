<?
  /* menu items */
  $target_stats = "dispatch.php?atknodetype=project&atkaction=stats&selectedprojectid=[id]";      
  $target_planning = "dispatch.php?atknodetype=project&atkaction=planning&selectedprojectid=[id]";        
      
  menuitem("hours_admin",  dispatch_url("hours", "admin"));
  menuitem("hours_hoursurvey", dispatch_url("hours", "hoursurvey"));
  menuitem("-");
  menuitem("customer_admin", dispatch_url("customer", "admin"));
  menuitem("-");
  menuitem("project_admin", dispatch_url("project", "admin"), "main", is_allowed("project", "admin"));
  menuitem("project_stats", dispatch_url("project", "select", array("atktarget" => $target_stats)), "main", is_allowed("project", "stats"));
  menuitem("project_planning",  dispatch_url("project", "select", array("atktarget" => $target_planning)), "main", is_allowed("project", "planning"));  
  menuitem("-");
  menuitem("activity_admin", dispatch_url("activity", "admin"), "main", is_allowed("activity", "admin"));
  menuitem("activity_stats", dispatch_url("activity", "stats"), "main", is_allowed("activity", "stats"));
  menuitem("-");  
  menuitem("employee_admin", dispatch_url("employee", "admin"), "main", is_allowed("employee", "admin"));
  menuitem("employee_stats", dispatch_url("employee", "stats"), "main", is_allowed("employee", "stats"));
  menuitem("-");
  menuitem("profile_admin", dispatch_url("profile", "admin"), "main", is_allowed("profile", "admin"));
  menuitem("-");
  menuitem("tpl_phase_admin", dispatch_url("tpl_phase", "admin"), "main", is_allowed("tpl_phase", "admin"));
  menuitem("tpl_project_admin", dispatch_url("tpl_project", "admin"), "main", is_allowed("tpl_project", "admin"));  
  menuitem("-");
  menuitem("achievo_docs", "manual.php");  
  menuitem("achievo_license", "license.php");  
  menuitem("achievo_home", "http://www.achievo.com");   
?>