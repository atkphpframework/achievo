<?php
  /* menu items */
      
  menuitem("hours_admin",  dispatch_url("hours", "admin"));
  
  menuitem("hours_hoursurvey", dispatch_url("hours", "hoursurvey"));

  menuitem("schedule", dispatch_url("schedule","admin"));
  menuitem("schedule_types", dispatch_url("schedule_types","admin"));
  
  menuitem("-");
  menuitem("projectmanagement"); // projects submenu
     $target_stats = "dispatch.php?atknodetype=project&atkaction=stats&selectedprojectid=[id]";      
     $target_planning = "dispatch.php?atknodetype=project&atkaction=planning&selectedprojectid=[id]";        
     menuitem("project_admin", dispatch_url("project", "admin"), "projectmanagement", is_allowed("project", "admin"));
     menuitem("project_planning",  dispatch_url("project", "select", array("atktarget" => $target_planning)), "projectmanagement", is_allowed("project", "planning"));  
     menuitem("-","","projectmanagement");
     menuitem("project_stats", dispatch_url("project", "select", array("atktarget" => $target_stats)), "projectmanagement", is_allowed("project", "stats"));          
     menuitem("activity_stats", dispatch_url("activity", "stats"), "projectmanagement", is_allowed("activity", "stats"));     
     menuitem("employee_stats", dispatch_url("employee", "stats"), "projectmanagement", is_allowed("employee", "stats"));
     menuitem("-","","projectmanagement");
     menuitem("project_todoview", dispatch_url("project_todoview", "admin"), "projectmanagement", is_allowed("project_todoview", "admin"));
     menuitem("project_notesview", dispatch_url("project_notesview", "admin"), "projectmanagement", is_allowed("project_notesview", "admin"));
     
  menuitem("-");
  menuitem("customer_admin", dispatch_url("customer", "admin"));
  
  menuitem("-");
  menuitem("setup");    
     menuitem("activity_admin", dispatch_url("activity", "admin"), "setup", is_allowed("activity", "admin"));
     menuitem("tpl_phase_admin", dispatch_url("tpl_phase", "admin"), "setup", is_allowed("tpl_phase", "admin"));
     menuitem("tpl_project_admin", dispatch_url("tpl_project", "admin"), "setup", is_allowed("tpl_project", "admin"));    
  
  menuitem("-");
  menuitem("employeemanagement");
     menuitem("profile_admin", dispatch_url("profile", "admin"), "employeemanagement", is_allowed("profile", "admin"));
     menuitem("-","","employeemanagement");
     menuitem("employee_admin", dispatch_url("employee", "admin"), "employeemanagement", is_allowed("employee", "admin"));    
  
  menuitem("-");          
  menuitem("support");
     menuitem("achievo_docs", "manual.php", "support");
     menuitem("achievo_license", "license.php", "support");
     menuitem("achievo_home", "http://www.achievo.com","support");
    
?>
