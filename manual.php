<?php
  
  require "atk/class.atknode.inc";
  require "theme.inc";

  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->ui_top($txt_app_title);
  $g_layout->output('

<br><br><br><br><br>

At some point, in the very, very distant future, a manual will be present on this page.

<br><br>Until such time arrives, you are left on your own.
<br><br><br><br><br>

 '); 

$g_layout->ui_bottom();
$g_layout->output("</body>");
$g_layout->output("</html>");
$g_layout->outputFlush();

?>
