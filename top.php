<?php
  include_once("./theme.inc");
  require_once("atk/class.atknode.inc");

  $g_layout->output('<html>');
  $g_layout->head($txt_app_title);
  $g_layout->body();

  $g_layout->ui_top($txt_app_title);

  $g_layout->output('<br>'.$txt_logout_loggedinuser.': <b>'.$g_user["name"].'</b>&nbsp; &nbsp; &nbsp;');  
  $g_layout->output(href(dispatch_url("userprefs", "edit", array("atkselector" => "employee.userid='".$g_user["name"]."'")), text('userprefs'), SESSION_NEW, false, 'target="main"'));
  $g_layout->output('<br><br>');
  $g_layout->ui_bottom();
  $g_layout->output('</body>');
  $g_layout->output('</html>');
  
  $g_layout->outputFlush();
?>
