<?php
  /**
   * ATK Include file.. If you strip out stuff you don't need, you 
   * might have some performance improvement.
   * Somethings can't be stripped out.
   */
   
   // This variable indicates where to get all the includes. Usually, this is 
   // the current dir.
   if (!isset($config_atkroot)||$config_atkroot=="")
   {
     $config_atkroot = "./";      
   }
  
  // Atknode is required. Don't remove it.
  include_once($config_atkroot."atk/modules/class.atkmodule.inc"); // need to include this file
                                                                   // now, because achievoModule needs it.
  include_once($config_atkroot."modules/class.achievomodule.inc"); 
  include_once($config_atkroot."atk/class.atknode.inc"); 
   
?>
