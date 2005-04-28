<?php

  /** 
   * Extension of the project node, with added capabilities of filtering
   * projects. (members only, etc.)
   */
   
  include_once(moduleDir("project")."class.project.inc");
   
  class projectselector extends project
  {
    function projectselector()
    {
      global $g_securityManager;
      $this->project("projectselector", NF_READONLY);
      $this->setSecurityAlias("project.project");
      
      // Add filter. 
      if (!$g_securityManager->allowed("timereg.hours", "any_project"))
      {
        $this->add(new projectFilter());
      }
    }
        
  }
  
  class projectFilter extends atkAttribute 
  {
    function projectFilter()
    {
      $this->atkAttribute("projectfilter", AF_HIDE|AF_FORCE_LOAD);
      
    }
    
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      $user = getUser();
      $query->addJoin("project_person", "members", "members.projectid = project.id", true);      
      $query->addCondition("(members.personid = ".$user["id"]." OR project.timereg_limit=".PRJ_TIMEREG_ALL_USERS.")");
    }
  }

?>