<?php

  function pimPlanning()
  {
    global $g_user, $g_layout;
    
    $planningsnode = getNode("resource_planning.resource_planning");

    $actions = array();

    $userid=$g_user['name'];
    $recs = $planningsnode->selectDb("planning.employeeid = '$userid' AND planning.plandate=now()", "planning.phaseid", ' ', "planning.plandate");

    if (count($recs)>0)
    {
      $suppressList = array("employeeid", "plandate");
      $plannedhours.= $planningsnode->recordList($recs,$actions,false, $suppressList);
    }
    else
    {
      $plannedhours.=text(resource_planning_pim_no_planning).'<br>';
    }

    $plannedhours.='<br>'.href(dispatch_url("resource_planning.resource_planning", "planweekviewempl"), text(pim_gotoweekview), SESSION_NESTED);
    $res.= $g_layout->ret_table_simple();
    $res.= '<tr>';
    $res.= $g_layout->ret_td('&nbsp;','width="15"'); // spacer
    $res.= $g_layout->ret_td($plannedhours, 'valign="top"');
    $res.= '</tr></table>';
    
    return $res;
  }

?>