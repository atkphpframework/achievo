<?php

  class mod_project extends achievoModule
  {  
    function getNodes()
    {
      /* register nodes */  
      registerNode("project.project", array("admin", "add", "edit", "delete", "stats","planning"));
      registerNode("project.phase", array("add", "edit", "delete"));
      registerNode("project.activity", array("admin", "add", "edit", "delete", "stats"));
      registerNode("project.tpl_phase", array("admin", "add", "edit", "delete"));
      registerNode("project.tpl_project", array("admin", "add", "edit", "delete"));
      registerNode("project.employee_project", array("admin", "add", "edit", "delete"));
    }
    
    function getMenuItems()
    {
      global $selectedproject;
      if ($selectedproject == 0)
      {    
        menuitem("projectmanagement","","main",true,400); // projects submenu
        $target_stats = "dispatch.php?atknodetype=project.project&atkaction=stats&selectedprojectid=[id]";      
        $target_planning = "dispatch.php?atknodetype=project.project&atkaction=planning&selectedprojectid=[id]";        
        menuitem("project_admin", dispatch_url("project.project", "admin"), "projectmanagement", array("project.project", "admin"));
        menuitem("project_planning",  dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_planning))), "projectmanagement", array("project.project", "planning"));
        menuitem("-","","projectmanagement");
        menuitem("project_stats", dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_stats))), "projectmanagement", array("project.project", "stats"));
        menuitem("activity_stats", dispatch_url("project.activity", "stats"), "projectmanagement", array("project.activity", "stats"));     
      }
      else
      {
        menuitem("projectmanagement","","main",true,400);     // selected projects submenu   
        menuitem("project_admin", dispatch_url("project.project", "edit", array("atkselector" => "project.id='".$selectedproject."'")), "projectmanagement", array("project.project", "admin"));
        menuitem("project_planning",  dispatch_url("project.project", "planning", array("selectedprojectid" => $selectedproject)), "projectmanagement", array("project.project", "planning"));  
        menuitem("project_planning",  dispatch_url("project.project", "planning", array("atkselector" => "project.id=")), "projectmanagement", array("project.project", "planning"));  
        menuitem("-","","projectmanagement");
        menuitem("project_stats", dispatch_url("project.project", "stats", array("selectedprojectid" => $selectedproject)), "projectmanagement", array("project.project", "stats"));            
      }
      
      menuitem("activity_admin", dispatch_url("project.activity", "admin"), "setup", array("project.activity", "admin"));
      menuitem("tpl_phase_admin", dispatch_url("project.tpl_phase", "admin"), "setup", array("project.tpl_phase", "admin"));
      menuitem("tpl_project_admin", dispatch_url("project.tpl_project", "admin"), "setup", array("project.tpl_project", "admin"));    
  
    }
  }
?>