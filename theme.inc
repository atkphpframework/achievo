<?php
  /* Setup the system */
  $config_atkroot = "./";
  require_once($config_atkroot."atk/class.atknode.inc");

  $sql = "SELECT theme from employee where userid='".$g_user["name"]."'";
  $result = $g_db->getrows($sql);
  
  if(count($result)==1&&$result[0]["theme"]!="")
  {
    $g_theme["Name"] = $result[0]["theme"];
  }
  
  $g_layout->initTheme();
  $g_layout->import_layout();
?>
