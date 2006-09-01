<?php

  /**
   * @internal modifiers
   */
  $modifiers[] = "timereg.hours";

  class mod_project extends achievoModule
  {
    function getNodes()
    {
      /* register nodes */
      registerNode("project.project", array("admin", "add", "edit", "delete", "stats","planning", "initialplanning", "any_project", "changeabbreviation"));
      registerNode("project.phase", array("add", "edit", "delete"));
      registerNode("project.deliverable", array("add", "edit", "delete"));
      registerNode("project.activity", array("admin", "add", "edit", "delete", "stats"));
      registerNode("project.tpl_phase", array("admin", "add", "edit", "delete"));
      registerNode("project.tpl_project", array("admin", "add", "edit", "delete"));
      registerNode("project.role", array("admin", "add", "edit", "delete"));
      registerNode("project.mastergantt_colorconfig", array("admin", "edit"));
    }

    function getMenuItems()
    {
      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : 0;
      if ($selectedproject == 0)
      {
        $this->menuitem("projectmanagement",dispatch_url("project.project", "admin"),"main",array("project.project", "admin"),400); // projects submenu
        $target_planning = "dispatch.php?atknodetype=project.project&atkaction=planning&selectedprojectid=[id]";
        $this->menuitem("project_admin", dispatch_url("project.project", "admin"), "projectmanagement", array("project.project", "admin"));
        $this->menuitem("project_planning",  dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_planning))), "projectmanagement", array("project.project", "planning"));
        $this->menuitem("-","","projectmanagement");
        $this->menuitem("project_stats", dispatch_url("project.projectstats", "stats"), "projectmanagement", array("project.project", "stats"));
        $this->menuitem("activity_stats", dispatch_url("project.activity", "stats"), "projectmanagement", array("project.activity", "stats"));
      }
      else
      {
        $this->menuitem("projectmanagement",dispatch_url("project.project", "admin"),"main",array("project.project", "admin"),400); // projects submenu
        $this->menuitem("project_edit", dispatch_url("project.project", "edit", array("atkselector" => "project.id='".$selectedproject."'")), "projectmanagement", array("project.project", "edit"));
        $this->menuitem("project_view", dispatch_url("project.project", "view", array("atkselector" => "project.id='".$selectedproject."'")), "projectmanagement", array("project.project", "admin"));
        $this->menuitem("project_planning",  dispatch_url("project.project", "planning", array("selectedprojectid" => $selectedproject)), "projectmanagement", array("project.project", "planning"));
        $this->menuitem("-","","projectmanagement");
        $this->menuitem("project_stats", dispatch_url("project.projectstats", "stats", array("projectid" => $selectedproject)), "projectmanagement", array("project.project", "stats"));
      }
      $this->menuitem("mastergantt", dispatch_url("project.mastergantt", "planning"), "projectmanagement", array("project.project", "planning"));
      $this->menuitem("activity_admin", dispatch_url("project.activity", "admin"), "setup", array("project.activity", "admin"));
      $this->menuitem("tpl_phase_admin", dispatch_url("project.tpl_phase", "admin"), "setup", array("project.tpl_phase", "admin"));
      $this->menuitem("tpl_project_admin", dispatch_url("project.tpl_project", "admin"), "setup", array("project.tpl_project", "admin"));
      $this->menuitem("role_admin", dispatch_url("project.role", "admin"), "setup", array("project.role", "admin"));
      $this->menuitem("mastergantt_colorconfig", dispatch_url("project.mastergantt_colorconfig", "admin"), "setup", array("project.mastergantt_colorconfig", "admin"));
    }

    function getDependencies($mode)
    {
      return array("employee", "organization");
    }

    function search($expression)
    {
      $prjnode = &atkGetNode("project.project");
      return array("projects"=>$this->recLinks($prjnode->searchDb($expression), "project.project"));
    }

    /**
     * Adds a listener to the timereg.hours node
     *
     * @param hours $atknode hours node instance
     */
    function timereg_hours_modifier(&$node)
    {
      atkimport("module.project.hourslistener");
      $node->addListener(new hoursListener(array("save", "update")));
    }   
  
  }

?>