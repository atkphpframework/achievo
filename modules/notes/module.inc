<?php

  class mod_notes extends achievoModule
  {
    function getNodes()
    {
      registerNode("notes.project_notes", array("admin", "add", "edit", "delete"));     
    }
    
    function getMenuItems()
    {          
      menuitem("-","","projectmanagement");        
      $selectedproject = $_REQUEST["selectedproject"];
      if ($selectedproject == 0)
      {
        menuitem("project_notesview", dispatch_url("notes.project_notesview", "admin"), "projectmanagement", array("notes.project_notes", "admin"));
      } 
      else
      {        
        menuitem("project_notesview", dispatch_url("notes.project_notesview", "edit", array("atkselector" => "project.id=".$selectedproject)), "projectmanagement", array("notes.project_notesview", "admin"));   
      }   
    }
  }
  
?>
