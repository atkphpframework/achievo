<?php

  $config_atkroot = "./";
  include_once("atk.inc");
  include_once("atk/atkbrowsertools.inc");
  atksession();
  atksecure();

  $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();
  //$output = &atkOutput::getInstance();
  $output = "";

  $output='<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">';
  $output.="\n<html>\n <head>\n";
  $output.='  <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset='.atktext("charset","","atk").'">';
  $output.="\n  <title>".atktext('app_title')."</title>\n </head>\n";

  $user = getUser();
  $pda = browserInfo::detectPDA();
  atkimport("atk.utils.atkframeset");

  if(isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"]))
  {
    $default_url = "dispatch.php?atknodetype=".$ATK_VARS["atknodetype"]."&atkaction=".$ATK_VARS["atkaction"];
    if (isset($ATK_VARS["atkselector"])) $default_url.="&atkselector=".$ATK_VARS["atkselector"];
  }
  else if (strtolower($user["name"]) == "administrator")
  {
    $default_url = session_url("dispatch.php?atknodetype=pim.pim&atkaction=adminpim",SESSION_NEW);
  }
  else if ($pda)
  {
    $default_url = session_url("dispatch.php?atknodetype=pim.pim&atkaction=pim",SESSION_NEW);
  }
  else
  {
    $default_url = session_url("dispatch.php?atknodetype=pim.pim&atkaction=pim",SESSION_NEW);
  }

  //$root = &new atkRootFrameset();
  if (!$pda)
  {
  $output.='
        <frameset rows="80,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ';

  $output.='
      <frameset cols="190,*" frameborder="0" border="0">
        <frame name="menu" scrolling="no" noresize src="menu.php" marginwidth="0" marginheight="0">
        <frame name="main" scrolling="auto" noresize src="'.$default_url.'" marginwidth="0" marginheight="0">
    ';
  $output.='</frameset>';

  $output.='
        <noframes>
          <body bgcolor="#CCCCCC" text="#000000">
            <p>Your browser doesnt support frames, but this is required to run '.atktext("app_title").'</p>
          </body>
        </noframes>
      </frameset>
      </html>
       ';
  }
  else
  {
    $config_defaulttheme="default";

    $output.='
    <frameset rows="30,*" frameborder="0">
      <frame name="top" src="top.php">
      <frame name="main" src="'.$default_url.'">
      <noframes>
        <body bgcolor="#CCCCCC" text="#000000">
          <p>Your PDA doesnt support frames, but this is required to run '.atktext("app_title").'</p>
        </body>
      </noframes>
    </frameset>
  ';
  }


  $output.= "</html>";
  echo $output;
?>
