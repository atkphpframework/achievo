<?php
  
  include_once("atk.inc");
    
  atksession(); 
  atksecure();
 
  $g_layout->output('<html>');
  $g_layout->head(text("app_title"));
  

  $g_layout->output('
        <frameset rows="70,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ');    
    
    $g_layout->output('
      <frameset cols="190,*" frameborder="0" border="0">
        <frame name="menu" scrolling="no" noresize src="menu.php" marginwidth="0" marginheight="0">
        <frame name="main" scrolling="auto" noresize src="'.session_url("dispatch.php?atknodetype=pim&atkaction=pim",SESSION_NEW).'" marginwidth="0" marginheight="0">
    ');
    $g_layout->output('</frameset>');

    $g_layout->output('
        <noframes>
          <body bgcolor="#CCCCCC" text="#000000">
            <p>Your browser doesnt support frames, but this is required to run <?php echo $txt_app_title; ?></p>
          </body>
        </noframes>
      </frameset>
      </html>
       ');  

  $g_layout->outputFlush();
?>
