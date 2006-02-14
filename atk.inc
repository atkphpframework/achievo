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
  include_once($config_atkroot."atk/include/initial.inc"); // need to include this file
                                                                   // now, because achievoModule needs it.
  // Some global achievo definitions we use in nodes.

  // Uniform textbox sizes
  define("TEXT_SMALL", 3);
  define("TEXT_LARGE", 10);

  // some uniform colors
  define("COLOR_WARNING", "#FCAA60");
  define("COLOR_ERROR", "#FF0000");
  define("COLOR_OK", "#00FF00");

  // some uniform recordlist highlight color definitions containing the
  // color which can be used when the mousepointer hovers over a row
  define("COLOR_HIGHLIGHT_WARNING", "#FFCC83");
  define("COLOR_HIGHLIGHT_ERROR", "#FF5555");
  define("COLOR_HIGHLIGHT_OK", "#66FF66");

?>
