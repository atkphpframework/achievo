<?php   

  /* Setup the system */    
  include_once("atk.inc");    
    
  atksession();
  if($ATK_VARS["atknodetype"]=="" || !$session["login"]!=1)
  {
    // no nodetype passed, or session expired
    $g_layout->initGui();
    $g_layout->ui_top(text("title_session_expired"));
    $g_layout->output("<br><br>".text("explain_session_expired")."<br><br>");
    $g_layout->output("<a href=\"index.php?atklogout=true\" target=\"_top\">".text("relogin")."<a/><br><br>");
    $g_layout->ui_bottom();
    $g_layout->page(text("title_session_expired"));
    atkerror(text("title_session_expired"));
  }
  else
  {
    atksecure();
  
    include_once("theme.inc");
     
    // Create node
    $obj = &getNode($ATK_VARS["atknodetype"]); 
    
    if (is_object($obj))
    {
      $obj->dispatch($ATK_VARS);     
    }
    else
    {
      atkdebug("No object created!!?!");
    }
  } 
  $g_layout->outputFlush();
  
?>
