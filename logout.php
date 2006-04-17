<?php
  $config_atkroot = "./";
  include_once("atk.inc");
  include_once("./theme.inc");

  $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();

  $page->register_style($theme->stylePath("style.css"));

  $tmp_output = "<font size=+2>Thanks for using Achievo</font>";
  $tmp_output.= "<hr>";
  $tmp_output.= '<center><a href="index.php">Login</a></center>';

  $box = $ui->renderBox(array("title"=>atkText("app_title"),
                                            "content"=>$tmp_output));

  $page->addContent($box);
  $output->output($page->render(atkText('app_title'), true));

  $output->outputFlush();

?>