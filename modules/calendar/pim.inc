<?php
  function pimSchedule()
  {
    global $g_user, $g_layout;
    
    $me = $g_user["name"];
    
    $schedulenode = getNode("calendar.schedule");
    $schedulenode->m_flags |= NF_NO_SEARCH;
    $recs = $schedulenode->selectDb("owner='$me' AND now()>=startdate AND now()<=enddate");

    $actions = array();

    if (count($recs)>0)
    {
      $todayschedule = $schedulenode->recordList($recs,$actions,false, array('publicitem','allday','location','scheduletype','enddate'));
    }
    else
    {
      $todayschedule = text('pim_schedule_empty').'<br>';
    }
    $todayschedule.='<br>'.href("dispatch.php?atknodetype=calendar.schedule&atkaction=admin",text('pim_gotoschedule'),SESSION_NESTED);

    $res.= $g_layout->ret_table_simple();
    $res.= '<tr>';
    $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
    $res.= $g_layout->ret_td($todayschedule, 'valign="top"');
    $res.= '</tr></table>';
    return $res;
  }

?>