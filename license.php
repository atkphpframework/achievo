<?php

  $config_atkroot = "./";
  include_once("atk.inc");
  atksession();
  atksecure();
  require "theme.inc";

  $page = &atkinstance("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();

  $page->register_style($theme->stylePath("style.css"));

  $license = file("doc/LICENSE");
  $tmp_output="";
  for ($i=0;$i<count($license);$i++)
  {
    $tmp_output.='<br>'.str_replace("", "", $license[$i]);
  }

  $title = atkText('title_license');
  $box = $ui->renderBox(array("title"=>atkText("title_license"), "content"=>$tmp_output));
  $actionpage = $ui->render("actionpage.tpl", array("blocks"=>array($box), "title"=>$title));
  $page->addContent($actionpage);
  $output->output($page->render($title, true));

  $output->outputFlush();
?>