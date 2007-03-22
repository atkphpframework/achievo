<?php

  /**
   * @internal includes
   */
  $config_atkroot = "./";
  include_once("atk.inc");
  include_once("atk/atkbrowsertools.inc");
  include_once("achievotools.inc");
  atksession();
  atksecure();
  include "theme.inc";

  $theme = &atkinstance('atk.ui.atktheme');
  if (atkconfig("fullscreen"))
  {
    // Fullscreen mode. Use index.php as launcher, and launch app.php fullscreen.

    $page = &atknew("atk.ui.atkpage");
    $ui = &atkinstance("atk.ui.atkui");
    $theme = &atkTheme::getInstance();
    $output = &atkOutput::getInstance();

    $page->register_style($theme->stylePath("style.css"));
    $page->register_script(atkconfig("atkroot")."atk/javascript/launcher.js");

    $content = '<script language="javascript">atkLaunchApp(); </script>';
    $content.= '<br><br><a href="#" onClick="atkLaunchApp()">'.atktext('app_reopen', "atk").'</a> &nbsp; '.
    '<a href="#" onClick="window.close()">'.atktext('app_close', "atk").'</a><br><br>';

    $box = $ui->renderBox(array("title"=>text("app_launcher"),
    "content"=>$content));

    $page->addContent($box);
    $output->output($page->render(text('app_launcher'), true));

    $output->outputFlush();
  }
  else
  {
    if ($theme->getAttribute('useframes',true))
    {
      // Regular mode. app.php can be included directly.
      include "app.php";
    }
    else
    {
      $indexpage = &atknew('atk.ui.atkindexpage');
      $indexpage->generate();
    }
  }
/*
  $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();
  $atkoutput = &atkOutput::getInstance();

  $pda = browserInfo::detectPDA();

  if ($pda)
  {
    $config_defaulttheme="default";
    $bodytemplate='body_pda.tpl';
  }

  $body = $ui->render($ui->templatePath(($bodytemplate?$bodytemplate:'body.tpl')),getBodyTplVars());
  $version = achievoVersion();
  $state = achievoState();
  $title = atktext("app_title")." v".achievoVersion().($state!='stable'?" ($state)":"");
  $extra_header='  <meta name="achievoversion" content="'.$version.'" />'."\n";
  $head = $page->head($title,$extra_header);
  $output = $ui->render($ui->templatePath('index.tpl'),array('head'=>$head,
                                                             'body'=>$body));

  // Send the output to the browser
  $atkoutput->output($output);
  $atkoutput->outputFlush();


  function getBodyTplVars()
  {
    global $ATK_VARS;
    $user = getUser();
    if(isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"]))
    {
      $params = array();
      if (isset($ATK_VARS["atkselector"])) $params["atkselector"] = $ATK_VARS["atkselector"];
      if (isset($ATK_VARS["atktab"])) $params["atktab"] = $ATK_VARS["atktab"];
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

    return array( 'top_url'     =>$top_url,
                  'default_url' =>$default_url,
                  'menu_url'    =>$menu_url,);
  }
*/
?>