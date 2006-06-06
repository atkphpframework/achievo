<?php

  $config_atkroot = "./";
  include_once("atk.inc");
  include_once("atk/atkbrowsertools.inc");
  atksession();
  atksecure();

  $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();
  $atkoutput = &atkOutput::getInstance();


  $user = getUser();
  $pda = browserInfo::detectPDA();

  if(isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"]))
  {
    $params = array();
    if (isset($ATK_VARS["atkselector"])) $params["atkselector"] = $ATK_VARS["atkselector"];
    $default_url = session_url(dispatch_url($ATK_VARS["atknodetype"], $ATK_VARS["atkaction"], $params), SESSION_NEW);
  }
  else if (strtolower($user["name"]) == "administrator")
  {
    $default_url = session_url(dispatch_url("pim.pim", "adminpim"),SESSION_NEW);
  }
  else
  {
    $default_url = session_url(dispatch_url("pim.pim", "pim"),SESSION_NEW);
  }
  $top_url = session_url("top.php", SESSION_NEW);
  $menu_url = session_url("menu.php", SESSION_NEW);

  $output ='<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">';
  $output.="\n<html>\n  <head>\n";
  $output.='    <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset='.atktext("charset","","atk").'">';
  $output.="\n    <title>".atktext('app_title')."</title>\n  </head>\n";
  if (!$pda)
  {
    $output.='  <frameset rows="80,*" frameborder="0" border="0">
    <frame name="top" scrolling="no" noresize src="'.$top_url.'" marginwidth="0" marginheight="0">
    <frameset cols="190,*" frameborder="0" border="0">
      <frame name="menu" scrolling="no" noresize src="'.$menu_url.'" marginwidth="0" marginheight="0">
      <frame name="main" scrolling="auto" noresize src="'.$default_url.'" marginwidth="0" marginheight="0">
    </frameset>
    <noframes>
      <body bgcolor="#CCCCCC" text="#000000">
        <p>Your browser doesnt support frames, but this is required to run '.atktext("app_title").'</p>
      </body>
    </noframes>
  </frameset>
';
  }
  else
  {
    $config_defaulttheme="default";

    $output.='  <frameset rows="30,*" frameborder="0">
    <frame name="top" src="'.$top_url.'">
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

  // Send the output to the browser
  $atkoutput->output($output);
  $atkoutput->outputFlush();
?>
