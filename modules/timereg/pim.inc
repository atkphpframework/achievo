<?php

  function pimTimeReg()
  {
    global $g_layout, $g_user;
    $hoursnode = getNode("timereg.hours");

    $actions = array();

    // Show only todays bookings, and only your own.
    $hoursnode->addFilter("activitydate",strftime("%Y-%m-%d"));    
    $hoursnode->addFilter("userid",strtolower($g_user["name"]));
    
    $recs = $hoursnode->selectDb();

    if (count($recs)>0)
    {
      $todayhours.= $hoursnode->recordList($recs,$actions,false, array());
    }
    else
    {
      $todayhours.= text('pim_timeregistration_empty').'<br>';
    }

    $todayhours.= '<br>'.href("dispatch.php?atknodetype=timereg.hours&atkaction=add",text('add'),SESSION_NESTED);
    $todayhours.= ' &nbsp; '.href("dispatch.php?atknodetype=timereg.hours&atkaction=admin",text('pim_gototimeregistration'),SESSION_NESTED);

    $res.= $g_layout->ret_table_simple();
    $res.= '<tr>';
    $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
    $res.= $g_layout->ret_td($todayhours, 'valign="top"');
    $res.= '</tr></table>';
    
    return $res;
  }

?>