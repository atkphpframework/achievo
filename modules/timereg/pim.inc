<?php

  function pimTimeReg()
  {
    global $g_layout;
    $hoursnode = getNode("timereg.hours");

    $actions = array();

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