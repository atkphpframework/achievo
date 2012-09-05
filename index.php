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

    $page = &atkinstance("atk.ui.atkpage");
    $ui = &atkinstance("atk.ui.atkui");
    $theme = &atkinstance("atk.ui.atktheme");
    $output = &atkinstance("atk.ui.atkoutput");

    $page->register_style($theme->stylePath("style.css"));
    $page->register_script(atkconfig("atkroot")."atk/javascript/launcher.js");

    $content = '<script language="javascript">atkLaunchApp(); </script>';
    $content.= '<br><br><a href="#" onClick="atkLaunchApp()">'.atktext('app_reopen', "atk").'</a> &nbsp; '.
    '<a href="#" onClick="window.close()">'.atktext('app_close', "atk").'</a><br><br>';

    $box = $ui->renderBox(array("title"=>atktext("app_launcher"),
    "content"=>$content));

    $page->addContent($box);
    $output->output($page->render(atktext('app_launcher'), true));

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
      $user = &atkGetUser();
      $indexpage = &atknew('atk.ui.atkindexpage');
      $indexpage->setUsername(getFullUsername());
      $indexpage->setTitle(getAchievoTitle());
      $indexpage->setTopSearchPiece(getSearchPiece());
      $centerpiece="";
      $centerpiecelinks=array();
      getCenterPiece($centerpiece,$centerpiecelinks);
      $indexpage->setTopCenterPieceLinks($centerpiecelinks);
      if($user["name"]=="administrator")
      {
        $destination = array("atknodetype"=>"pim.pim","atkaction"=>"adminpim");
      }
      else 
      {
        $destination = array("atknodetype"=>"pim.pim","atkaction"=>"pim");
      }
      $indexpage->setDefaultDestination($destination);
      $indexpage->generate();
    }
  }
?>