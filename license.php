<?php

  require "atk/class.atknode.inc";
  atksecure();
  require "theme.inc";

  $g_layout->output("<html>");
  $g_layout->head($txt_title_license);
  $g_layout->body();
  $g_layout->ui_top($txt_title_license);


  $license = file("doc/LICENSE");
  for ($i=0;$i<count($license);$i++)
  {
    $g_layout->output('<br>'.$license[$i]);
  }

  $g_layout->ui_bottom();
  $g_layout->output("</body>");
  $g_layout->output("</html>");
  $g_layout->outputFlush();

?>
