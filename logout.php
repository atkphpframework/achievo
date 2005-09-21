<?php
  include_once("atk.inc");
  include_once("./theme.inc");
  
  atkimport("atk.layout");
  $layout = &layout::getInstance();

  $layout->output('<html>');
  $layout->head($txt_app_title);
  $layout->body();

  $layout->ui_top($txt_app_title);
  $layout->output("<font size=+2>Thanks for using Achievo</font>");
  $layout->output("<hr>");
  $layout->output('<center><a href="index.php">Login</a></center>');
  $layout->ui_bottom();
  $layout->output('</body>');
  $layout->output('</html>');

  $layout->outputFlush();
?>