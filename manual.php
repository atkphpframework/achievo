<?php
  
  include_once("atk.inc");  
  atksession();
  atksecure();
  require "theme.inc";

  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->ui_top($txt_app_title);
  $g_layout->output('

<br><br><br><br><br>

At some point, in the very, very distant future, a complete annotated reference manual will be present on this page.

<br><br>For now, you can use <a href="manual/guide.html">this Achievo Guide</a>, graciously contributed by Greg Louis.
<br><br><br><br><br>

 '); 

$g_layout->ui_bottom();
$g_layout->output("</body>");
$g_layout->output("</html>");
$g_layout->outputFlush();

?>
