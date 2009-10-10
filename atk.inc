<?php
  /**
   * ATK Include file.. If you strip out stuff you don't need, you
   * might have some performance improvement.
   * Somethings can't be stripped out.
   */

  /**
   * @internal This variable indicates where to get all the includes. Usually,
   *           this is the current dir.
   */
  $config_atkroot = "./";

  // Atknode is required. Don't remove it.
  include_once($config_atkroot."atk/include/initial.inc");

  // Uniform textbox sizes
  define("TEXT_SMALL", 3);
  define("TEXT_LARGE", 10);

  // Some uniform colors
  define("COLOR_WARNING", "#FCAA60");
  define("COLOR_ERROR", "#FF0000");
  define("COLOR_OK", "#00FF00");

  // Some uniform recordlist highlight color definitions containing the
  // color which can be used when the mousepointer hovers over a row
  define("COLOR_HIGHLIGHT_WARNING", "#FFCC83");
  define("COLOR_HIGHLIGHT_ERROR", "#FF5555");
  define("COLOR_HIGHLIGHT_OK", "#66FF66");

?>
