<?php
  include_once("atk.inc");  
  include_once("./theme.inc");

  $g_layout->output('<html>');
  $g_layout->head($txt_app_title);
  $g_layout->body();

  $g_layout->ui_top($txt_app_title);
  $g_layout->output("<font size=+2>Thanks for using Achievo</font>");
  $g_layout->output("<hr>");
  $g_layout->output('<center><a href="index.php">Login</a></center>');
  $g_layout->ui_bottom();
  $g_layout->output('</body>');
  $g_layout->output('</html>');
  
  $g_layout->outputFlush();
?>
