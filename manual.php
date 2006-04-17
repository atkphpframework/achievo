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

  $tmp_output='

<br><br><br><br><br>

At some point, in the very, very distant future, a complete annotated reference manual will be present on this page.

<br><br>For now, you can use <a href="manual/guide.html">this Achievo Guide</a>, graciously contributed by Greg Louis.
<br><br><br><br><br>

 ';

 $box = $ui->renderBox(array("title"=>atkText("app_title"),
                                            "content"=>$tmp_output));

  $page->addContent($box);
  $output->output($page->render(atkText('app_title'), true));

  $output->outputFlush();
?>