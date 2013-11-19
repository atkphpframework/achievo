<?php

atkimport("atk.menu.atkframesmenu");

class achievo extends atkFramesMenu
{

  function getHeader($atkmenutop)
  {
    if ($atkmenutop == "projectmanagement")
    {
      global $g_sessionManager;
      include_once "achievotools.inc";
      $projects = $g_sessionManager->getValue("selectedprj","globals");

      if (count($projects) == 0)
      {
        updateSelectedProjects();
        $projects = $g_sessionManager->getValue("selectedprj","globals");
      }

      $prj  = atktext("project_select") . ':';
      $prj .= '<FORM name="projectselectorform" action="menu.php" method="get">';
      $prj .= session_form(SESSION_DEFAULT);
      $prj .= '<INPUT type="hidden" name="atkmenutop" value="projectmanagement">';
      $prj .= '<SELECT style="width: 150px" name="selectedproject" onchange="document.projectselectorform.submit();">';
      $prj .= '<OPTION value="0">' . atktext("project_select_none") . '</OPTION>';
      $selectedproject = atkArrayNvl($_REQUEST, "selectedproject");
      foreach($projects as $project)
      {
        $selected = ($selectedproject == $project['projectid']) ? ' selected' : '';
        $prj .= sprintf('<OPTION value="%d" %s>%s</OPTION>', $project['projectid'], $selected, $project['projectname']);
      }
      $prj .= '</SELECT></FORM>';
      return $prj;
    }
  }
}

?>