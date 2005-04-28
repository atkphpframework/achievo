<?php

  function pimTimeReg()
  {
    global $g_layout, $g_user;
    $hoursnode = &getNode("timereg.hours");

    $hoursnode = getNode("timereg.hours");
    $pda = browserInfo::detectPDA();
    $actions = array();

    // Show only todays bookings, and only your own.
    $hoursnode->addFilter("activitydate",strftime("%Y-%m-%d"));    
    $hoursnode->addFilter("hours.userid",strtolower($g_user["id"]));
            
    $recs = $hoursnode->selectDb();

    if (count($recs)>0)
    {   
      $recordlist = &atknew("atk.recordlist.atkrecordlist");       
      $todayhours.= $recordlist->render($hoursnode, $recs, $actions);
    }
    else
    {
      $todayhours.= text('pim_timeregistration_empty').'<br>';
    }
    if ($pda) $nodetype = "timereg.hourspda";
    else $nodetype = "timereg.hours";
    $todayhours.= '<br>'.href("dispatch.php?atknodetype=$nodetype&atkaction=add",text('add'),SESSION_NESTED);
    if (!$pda) $todayhours.= ' &nbsp; '.href("dispatch.php?atknodetype=$nodetype&atkaction=admin",text('pim_gototimeregistration'),SESSION_NESTED);

    $res.= $g_layout->ret_table_simple();
    $res.= '<tr>';
    if (!$pda) $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
    $res.= $g_layout->ret_td($todayhours, 'valign="top"');
    $res.= '</tr></table>';
    
    return $res;
  }

?>