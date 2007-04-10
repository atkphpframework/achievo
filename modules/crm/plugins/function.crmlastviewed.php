<?php
 
  function smarty_function_crmlastviewed($params, &$smarty) 
  {
    $eventlog = &atkGetNode("crm.eventlog");
    $userid = atkGetUserId();
    $items = $eventlog->getRecentlyViewed($userid);
    
    $theme = &atkinstance("atk.ui.atktheme");
    $tpl = $theme->tplPath("crm_lastviewed.tpl");
    if($theme->tplPath("crm_lastviewed.tpl"))
    {
      $ui = &atkinstance("atk.ui.atkui");
      return $ui->render('crm_lastviewed.tpl',array("items"=>$items));
    }
    return "no template ?"; 
  }

?>