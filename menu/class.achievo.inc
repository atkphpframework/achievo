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
        //updateSelectedProjects();
        $projects = $g_sessionManager->getValue("selectedprj","globals");
      }

      $prj  = atktext("project_select").":";
      $prj .="<FORM><SELECT style=\"width: 150px\" name=\"selectedproject\" onchange=\"reloadProjects(this);\">";
      $prj .= "<OPTION value=\"0\">".atktext("project_select_none")."</OPTION>";

      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : null;
      for ($i=0;$i < count($projects); $i++)
      {
        $prj .= "<OPTION value=\"".$projects[$i]['projectid']."\"";
        if ($selectedproject == $projects[$i]['projectid']) $prj .=" selected";
        $prj .= ">".$projects[$i]['projectname']."</OPTION>";
      }
      $prj .="</SELECT></FORM>";
      return $prj;

    }
  }
}

?>