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

$tmp_output = '
<br>

<br>You can find a manual on the wiki <a href="http://www.achievo.org/wiki/Achievo/Manual">http://www.achievo.org/wiki/Achievo/Manual</a>,
or see the wiki on GitHub <a href="https://github.com/atkphpframework/achievo/wiki/Achievo-guided-tour">https://github.com/atkphpframework/achievo/wiki/Achievo-guided-tour</a>
<br>

 ';

$box = $ui->renderBox(array("title" => atkText("app_title"),
    "content" => $tmp_output));

$page->addContent($box);
$output->output($page->render(atkText('app_title'), true));

$output->outputFlush();
?>