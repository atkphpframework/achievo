<?php
  /* menu items */
  
  menuitem("time_registration", "","main",true,100);
    menuitem("hours_admin", dispatch_url("hours", "admin"),"time_registration",true);
    menuitem("-","","time_registration");
    menuitem("hours_lock", dispatch_url("hours_lock", "admin"),"time_registration",array("hours_lock","admin"));
    
  menuitem("-","","main",true,120);
//  menuitem("costregistration_admin", dispatch_url("costregistration", "admin"),"main",true,200);
    
//  menuitem("-","","main",true,220);
  
//  menuitem("schedule", dispatch_url("schedule","admin"),"main",true,300);  
//  menuitem("-","","main",true,320);
  
  menuitem("reports", "","main",true,370);
    menuitem("hours_hoursurvey", dispatch_url("hours", "hoursurvey"),"reports");    
    menuitem("weekreport", dispatch_url("weekreport", "report"),"reports",array("weekreport","report"));    
  
  menuitem("-", "","main",true,372);
   
 // if ($selectedproject == 0)
 // {
    menuitem("projectmanagement","","main",true,400); // projects submenu

      $target_stats = "dispatch.php?atknodetype=project&atkaction=stats&selectedprojectid=[id]";      
      $target_planning = "dispatch.php?atknodetype=project&atkaction=planning&selectedprojectid=[id]";        
      menuitem("project_admin", dispatch_url("project", "admin"), "projectmanagement", array("project", "admin"));
      menuitem("project_planning",  dispatch_url("project", "select", array("atktarget" => $target_planning)), "projectmanagement", array("project", "planning"));    
      menuitem("-","","projectmanagement");
      menuitem("project_stats", dispatch_url("project", "select", array("atktarget" => $target_stats)), "projectmanagement", array("project", "stats"));
      menuitem("activity_stats", dispatch_url("activity", "stats"), "projectmanagement", array("activity", "stats"));     
      menuitem("employee_stats", dispatch_url("employee", "stats"), "projectmanagement", array("employee", "stats"));     
 //     menuitem("-","","projectmanagement");
  //    menuitem("project_todoview", dispatch_url("project_todoview", "admin"), "projectmanagement", array("project_todoview", "admin"));
    //  menuitem("project_notesview", dispatch_url("project_notesview", "admin"), "projectmanagement", array("project_notesview", "admin"));
 // }
//  else
 // {
   // menuitem("projectmanagement","","main",true,400);     // selected projects submenu   
     // menuitem("project_admin", dispatch_url("project", "edit", array("atkselector" => "project.id='".$selectedproject."'")), "projectmanagement", array("project", "admin"));
     // menuitem("project_planning",  dispatch_url("project", "planning", array("selectedprojectid" => $selectedproject)), "projectmanagement", array("project", "planning"));  
      //menuitem("project_planning",  dispatch_url("project", "planning", array("atkselector" => "project.id=")), "projectmanagement", array("project", "planning"));  
    //  menuitem("-","","projectmanagement");
    //  menuitem("project_stats", dispatch_url("project", "stats", array("selectedprojectid" => $selectedproject)), "projectmanagement", array("project", "stats"));            
    //  menuitem("-","","projectmanagement");
  //    menuitem("project_todoview", dispatch_url("project_todoview", "edit", array("atkselector" => "project.id=".$selectedproject)), "projectmanagement", array("project_todoview", "admin"));
//      menuitem("project_notesview", dispatch_url("project_notesview", "edit", array("atkselector" => "project.id=".$selectedproject)), "projectmanagement", array("project_notesview", "admin"));   
//  }   

//  menuitem("customermanagement","","main",true,500);
//    menuitem("customer_admin", dispatch_url("customer", "admin"),"customermanagement");
    menuitem("customermanagement", dispatch_url("customer", "admin"),"main",true,500);
//    menuitem("contracts", dispatch_url("contracts","admin"),"customermanagement");

  menuitem("employeemanagement","","main",true,600);
     menuitem("profile_admin", dispatch_url("profile", "admin"), "employeemanagement", array("profile", "admin"));
     menuitem("employee_admin", dispatch_url("employee", "admin"), "employeemanagement", array("employee", "admin"));    

  menuitem("-","","main",true,620);

  menuitem("setup","","main",true,700);    
     menuitem("activity_admin", dispatch_url("activity", "admin"), "setup", array("activity", "admin"));
     menuitem("tpl_phase_admin", dispatch_url("tpl_phase", "admin"), "setup", array("tpl_phase", "admin"));
     menuitem("tpl_project_admin", dispatch_url("tpl_project", "admin"), "setup", array("tpl_project", "admin"));    
//     menuitem("holiday_admin", dispatch_url("holidays", "admin"), "setup", array("activity", "admin"));
//     menuitem("contracttype", dispatch_url("contracttype","admin"),"setup");      
//     menuitem("schedule_types", dispatch_url("schedule_types","admin"),"setup");
  menuitem("-","","main",true,720);         
  
  menuitem("support","","main",true,800);
     menuitem("achievo_docs", "manual.php", "support");
     menuitem("achievo_license", "license.php", "support");
     menuitem("achievo_home", "http://www.achievo.com/","support");  

?>
