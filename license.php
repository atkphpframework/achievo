<?php

  $config_atkroot = "./";
  include_once("atk.inc");
  atksession();
  atksecure();
  require "theme.inc";

  $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();

  $page->register_style($theme->stylePath("style.css"));

  $license = file("doc/LICENSE");
  $tmp_output="";
  for ($i=0;$i<count($license);$i++)
  {
    $tmp_output.='<br>'.$license[$i];
  }


  $box = $ui->renderBox(array("title"=>atkText("title_licence"),
                                            "content"=>$tmp_output));

  $page->addContent($box);
  $output->output($page->render(atkText('title_licence'), true));

  $output->outputFlush();
?>