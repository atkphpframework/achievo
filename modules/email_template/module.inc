<?php

  class mod_email_template extends achievoModule
  {
    function getNodes()
    {
    }

    function getMenuItems()
    {
    }
  }
  // Add plugins dir
  atkimport("atk.ui.atksmarty");
  atkSmarty::addPluginDir(moduleDir("email_template")."plugins/");
?>
