<?php
include_once("atk/atkbrowsertools.inc");
if (browserInfo::detectPDA())
    $GLOBALS["g_theme"]["Name"] = "default";
elseif (atkArrayNvl(atkGetUser(), "theme") != "")
    $GLOBALS["g_theme"]["Name"] = atkArrayNvl(atkGetUser(), "theme");
else
    $GLOBALS["g_theme"]["Name"] = atkConfig("defaulttheme");

$page = &atkinstance("atk.ui.atkpage");
$theme = &atkInstance("atk.ui.atktheme");

$page->register_style($theme->stylePath("style.css"));
?>