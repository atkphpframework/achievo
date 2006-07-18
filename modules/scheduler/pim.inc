<?php
  include_once (moduleDir("scheduler")."class.scheduler_view.inc");
  include_once (moduleDir("scheduler")."class.scheduler_pimview.inc");
  
 /**
  * Generate the scheduler for the pim page
  *
  * @return string The scheduler for the pim page
  */
  
  
  function pimScheduler()
  {
    global $g_user;

    $pimview = new scheduler_pimview();
    $pimview->setViewDate(); //always set current date
    $pimview->setViewUser($g_user["id"]);
    $res = $pimview->renderView();
    $res.='<br>'.href("dispatch.php?atknodetype=scheduler.scheduler&atkaction=admin",text('pim_gotoschedule'),SESSION_NESTED);
    return $res;
  }

?>
