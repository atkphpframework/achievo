<?php

atkimport("atk.menu.atkplainmenu");

class achievo extends atkPlainMenu 
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
      
      $prj .= text("project_select").":";
      $prj .="<FORM><SELECT name=\"selectedproject\" onchange=\"reloadProjects(this);\">";
      $prj .= "<OPTION value=\"0\">".text("project_select_none")."</OPTION>";
  
      for ($i=0;$i < count($projects); $i++)
      {
        $prj .= "<OPTION value=\"".$projects[$i]['projectid']."\"";
        if ($_REQUEST["selectedproject"] == $projects[$i]['projectid']) $prj .=" selected";
        $prj .= ">".$projects[$i]['projectname']."</OPTION>";
      }
      $prj .="</SELECT></FORM>";
      return $prj;
  
    }
  }
}

?>