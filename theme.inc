<?php
  /* Setup the system */
//  $config_atkroot = "./";
//  require_once($config_atkroot."atk/class.atknode.inc");
  if (is_object($g_sessionManager))
  {
    $g_theme["Name"] = $g_sessionManager->getValue("atkTheme", "globals");
  }
   
  if(empty($g_theme["Name"]))
  { 
    $g_theme["Name"] = atkconfig("defaulttheme"); // default from configfile 
  
    $sql = "SELECT theme from person where userid='".$g_user["name"]."'";
    $result = $g_db->getrows($sql);
    
    if(count($result)==1&&$result[0]["theme"]!="")
    {
      $g_theme["Name"] = $result[0]["theme"];    
    }
    if (is_object($g_sessionManager))
    {
      $g_sessionManager->globalVar("atkTheme", $g_theme["Name"], true);
    }
  }
  $g_layout->initTheme();
  $g_layout->import_layout();
  
?>