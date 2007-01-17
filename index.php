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

  $pda = browserInfo::detectPDA();  
                    
  if ($pda)
  {
    $config_defaulttheme="default";
    $bodytemplate='body_pda.tpl';
  }
  
  $body = $ui->render($ui->templatePath(($bodytemplate?$bodytemplate:'body.tpl')),getBodyTplVars());
  $output = $ui->render($ui->templatePath('index.tpl'),array('body'=>$body));

  // Send the output to the browser
  $atkoutput->output($output);
  $atkoutput->outputFlush();
  
  /**
   * Get the template variables for the body template.
   * Contains the urls for every part of the application.
   *
   * @return Array The template variables
   */
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
?>
