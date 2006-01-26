<?php
  
  include_once("atk.inc");
  include_once("atk/atkbrowsertools.inc");
  atksession(); 
  atksecure();
  
  atkimport("atk.layout");
  $layout = &layout::getInstance();
 
  $layout->output('<html>');
  $layout->head(text("app_title"));
  
  $user = getUser();
  $pda = browserInfo::detectPDA();
  

  if(isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"]))
  {
    $default_url = "dispatch.php?atknodetype=".$ATK_VARS["atknodetype"]."&atkaction=".$ATK_VARS["atkaction"];
    if (isset($ATK_VARS["atkselector"])) $default_url.="&atkselector=".$ATK_VARS["atkselector"];
  }
  else if (strtolower($user["name"]) == "administrator")
  {
    $default_url = session_url("dispatch.php?atknodetype=pim.pim&atkaction=adminpim",SESSION_NEW);
  }
  else if ($pda)
  {
    $default_url = session_url("dispatch.php?atknodetype=pim.pim&atkaction=pim",SESSION_NEW);
  }
  else
  {
    $default_url = session_url("dispatch.php?atknodetype=pim.pim&atkaction=pim",SESSION_NEW);
  }
  
  
  if (!$pda)
  {
  $layout->output('
        <frameset rows="80,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ');    
    
    $layout->output('
      <frameset cols="190,*" frameborder="0" border="0">
        <frame name="menu" scrolling="no" noresize src="menu.php" marginwidth="0" marginheight="0">
        <frame name="main" scrolling="auto" noresize src="'.$default_url.'" marginwidth="0" marginheight="0">
    ');
    $layout->output('</frameset>');

    $layout->output('
        <noframes>
          <body bgcolor="#CCCCCC" text="#000000">
            <p>Your browser doesnt support frames, but this is required to run '.text("app_title").'</p>
          </body>
        </noframes>
      </frameset>
      </html>
       ');  
  }
  else 
  {    
    $config_defaulttheme="default";
    $layout->output('
    <frameset rows="30,*" frameborder="0">
      <frame name="top" src="top.php">
      <frame name="main" src="'.$default_url.'">
      <noframes>
        <body bgcolor="#CCCCCC" text="#000000">
          <p>Your PDA doesnt support frames, but this is required to run '.text("app_title").'</p>
        </body>
      </noframes>
    </frameset>
  </html>');  
  }
  $layout->outputFlush();

?>
