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
      $this->project("projectselector", NF_READONLY);
      $this->setSecurityAlias("project.project");
      $this->add(new projectFilter());
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
      if ($mode != 'select') return;

      global $g_securityManager;
      if (!$g_securityManager->allowed("timereg.hours", "any_project"))
      {
        $user = getUser();
        $query->addJoin("project_person", "members", "members.projectid = $tablename.id", true);
        $query->addGroupBy("project.id");
        $query->addCondition("(members.personid = ".$user["id"]." AND $tablename.timereg_limit!=".PRJ_TIMEREG_ALL_USERS.") OR $tablename.timereg_limit=".PRJ_TIMEREG_ALL_USERS);
      }
      $query->addCondition("$tablename.status='active'");
    }
  }

?>
