<?php

  include_once("atk.inc");
  atksession();
  atksecure();
  require "theme.inc";
  
  atkimport("atk.layout");
  $layout = &layout::getInstance();

  $layout->output("<html>");
  $layout->head($txt_app_title);
  $layout->body();
  $layout->ui_top($txt_app_title);
  $layout->output('

<br><br><br><br><br>

At some point, in the very, very distant future, a complete annotated reference manual will be present on this page.

<br><br>For now, you can use <a href="manual/guide.html">this Achievo Guide</a>, graciously contributed by Greg Louis.
<br><br><br><br><br>

 ');

$layout->ui_bottom();
$layout->output("</body>");
$layout->output("</html>");
$layout->outputFlush();

?>