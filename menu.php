<?php
/**
 * Builds the Achievo menu, please note that this file is a slightly modified
 * version of the menu.php file in the ATK skel directory. This version
 * contains an extra line that includes the theme.inc file!
 */

  include_once("atk.inc");
  
  atksession();  
  atksecure();
  
  include_once("./theme.inc");
  include_once("achievotools.inc");
  
  /* get main menuitems */
  include_once($config_atkroot."config.menu.inc");  

  /* first add module menuitems */
  for ($i = 0; $i < count($g_modules); $i++)
  {
    $module = new $g_modules[$i]();
    menuitems($module->getMenuItems());
  }

  if (!isset($atkmenutop)||$atkmenutop=="") $atkmenutop = "main";

  /* output html */
  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);

  $g_layout->body();
  $g_layout->output("<div align='center'>"); 
  $g_layout->ui_top(text("menu_".$atkmenutop));
  $g_layout->output("<br>");

  
?>
    <script language="JavaScript">
    
    function reloadProjects(el)
    {
      var id = el.options[el.selectedIndex].value;      
      window.location= "menu.php?atkmenutop=projectmanagement&selectedproject="+id;                
    }
    </script>
<?
  /*drop down in projectmanagement */
  if ($atkmenutop == "projectmanagement")
  { 
    $projects = $g_sessionManager->getValue("recentprj");  
    if (count($projects) == 0)
    {
      updateSelectedProjects();
      $projects = $g_sessionManager->getValue("recentprj");
    }
    $prj .= text("project_select").":";
    $prj .="<FORM><SELECT name=\"selectedproject\" onchange=\"reloadProjects(this);\">";
    $prj .= "<OPTION value=\"0\">".text("project_select_none")."</OPTION>";
    
    for ($i=0;$i < count($projects); $i++)
    {
      $prj .= "<OPTION value=\"".$projects[$i]['projectid']."\"";
      if ($selectedproject == $projects[$i]['projectid']) $prj .=" selected";
      $prj .= ">".$projects[$i]['projectname']."</OPTION>";
    }
    $prj .="</SELECT></FORM>";
    $g_layout->output($prj);
    
  }
  

  /* build menu */
  $menu = "";  
  for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
  {
    $name = $g_menu[$atkmenutop][$i]["name"];
    $url = $g_menu[$atkmenutop][$i]["url"];
    //if ($select != "") $url .= "&atkselectedprojectid=".$select;
    //if ($select != "") $url .= rawurlencode("'".$select."'");
    //if ($select != "") $url .= atkUrlEncode("'".$select."'");
    
    $enable = $g_menu[$atkmenutop][$i]["enable"];

    /* delimiter ? */
    if ($g_menu[$atkmenutop][$i] == "-") $menu .= "<br>";
    
    /* submenu ? */
    else if (empty($url) && $enable) $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name")).$config_menu_delimiter;
    else if (empty($url) && !$enable) $menu .=text("menu_$name").$config_menu_delimiter;
      
    /* normal menu item */
    else if ($enable) $menu .= href($url,text("menu_$name"),SESSION_NEW,false,'target="main"').$config_menu_delimiter;
    else $menu .= text("menu_$name").$config_menu_delimiter;    
  }
  
  /* previous */
  if ($atkmenutop != "main")
  {
    $parent = $g_menu_parent[$atkmenutop];
    $menu .= $config_menu_delimiter;
    $menu .= href('menu.php?atkmenutop='.$parent,text("back_to").' '.text("menu_$parent")).'<br>';  
  }

  /* bottom */
  $g_layout->output($menu);
  $g_layout->output("<br><br>");
  $g_layout->ui_bottom();
  $g_layout->output("</div></html>"); 
  $g_layout->outputFlush();
?>
