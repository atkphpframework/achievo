<?php

  include_once("atk.inc");
  atksession();
  atksecure();
  require "theme.inc";
  
  atkimport("atk.layout");
  $layout = &layout::getInstance();

  $layout->initGUI();
  $layout->output("<html>");
  $layout->head($txt_title_license);
  $layout->body();
  $layout->ui_top($txt_title_license);


  $license = file("doc/LICENSE");
  for ($i=0;$i<count($license);$i++)
  {
    $layout->output('<br>'.$license[$i]);
  }

  $layout->ui_bottom();
  $layout->output("</body>");
  $layout->output("</html>");
  $layout->outputFlush();

?>