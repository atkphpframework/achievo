<?php

  /**
   * You might have noticed, that the module.inc file for the setup module
   * does not contain any nodes. This means, that you can not grant anyone
   * any rights to use the setup node. 
   * This ensures, that only the administrator user can make use of the 
   * stuff presented here
   */
   
  class setup extends atkNode
  {
    function setup()
    {
      $this->atkNode("setup");
    }
    
    function action_setup()
    {
      global $g_layout;
      $g_layout->initGui();
      $g_layout->ui_top($g_layout->title("setup", "setup", "setup"));
      $g_layout->output("Hello World");
      $g_layout->ui_bottom();
    }
  }
?>