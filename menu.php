<?php
/**
 * Builds the Achievo menu, please note that this file is a slightly modified
 * version of the menu.php file in the ATK skel directory. This version
 * contains an extra line that includes the theme.inc file!
 */

  include_once("atk.inc");  

  atksession("atkmenu");
  atksecure();

  include_once("./theme.inc");
  include_once($config_atkroot."atk/atkmenutools.inc");
  include_once("achievotools.inc");

  /* get main menuitems */
  include_once($config_atkroot."config.menu.inc");

  /* first add module menuitems */
  foreach ($g_modules as $modname => $modpath)
  {
    $mod = getModule($modname);
    if (method_exists($mod,"getMenuItems")) $mod->getMenuItems();
  }

  $atkmenutop = $HTTP_GET_VARS["atkmenutop"];

  if (!isset($atkmenutop)||$atkmenutop=="") $atkmenutop = "main";

  $g_layout->initGUI();
  $g_layout->register_script("javascript/menuload.js");

  /* output html */
  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);


  $g_layout->body();
  $g_layout->output("<div align='center'>");
  $g_layout->ui_top(text("menu_".$atkmenutop));
  $g_layout->output("<br>");

  /* build menu */
  $menu = "";

  function menu_cmp($a,$b)
  {
    if ($a["order"] == $b["order"]) return 0;
    return ($a["order"] < $b["order"]) ? -1 : 1;
  }

  usort($g_menu[$atkmenutop],"menu_cmp");

/*
  $g_layout->output('<script language="JavaScript">

    function reloadProjects(el)
    {
      var id = el.options[el.selectedIndex].value;
      window.location= "menu.php?atkmenutop=projectmanagement&selectedproject="+id;
    }
    </script>');
*/

  /* Drop down in projectmanagement */

  if ($atkmenutop == "projectmanagement")
  {
    $projects = $g_sessionManager->getValue("selectedprj","globals");
    if (count($projects) == 0)
    {
      updateSelectedProjects();
      $projects = $g_sessionManager->getValue("selectedprj","globals");
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

  for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
  {
    $name = $g_menu[$atkmenutop][$i]["name"];
    $url = $g_menu[$atkmenutop][$i]["url"];
    //if ($select != "") $url .= "&atkselectedprojectid=".$select;
    //if ($select != "") $url .= rawurlencode("'".$select."'");
    //if ($select != "") $url .= atkUrlEncode("'".$select."'");

    $enable = $g_menu[$atkmenutop][$i]["enable"];

    if (is_array($enable))
    {
      $enabled = false;
      for ($j=0;$j<(count($enable)/2);$j++)
      {
        $enabled |= is_allowed($enable[(2*$j)],$enable[(2*$j)+1]);
      }
      $enable = $enabled;
    }

    /* delimiter ? */

    if ($g_menu[$atkmenutop][$i]["name"] == "-") $menu .= "<br>";

    else if ($enable) // don't show menu items we don't have access to.
    {

      $hassub = (is_array($g_menu[$g_menu[$atkmenutop][$i]["name"]]));

      /* submenu ? */
      if ($hassub)
      {
        if (empty($url)) // normal submenu
        {
          $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name")).$config_menu_delimiter;
        }
        else // submenu AND a default url.
        {
          $menuurl = session_url('menu.php?atkmenutop='.$name);
          $mainurl = session_url($url,SESSION_NEW);
          $menu.= '<a href="javascript:menuload(\''.$menuurl.'\', \''.$mainurl.'\');">'.text("menu_$name").'</a>'.$config_menu_delimiter;
        }
      }
      else // normal menuitem
      {
        $menu .= href($url,text("menu_$name"),SESSION_NEW,false,'target="main"').$config_menu_delimiter;
      }
    }
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
