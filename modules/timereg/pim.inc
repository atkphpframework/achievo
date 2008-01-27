<?php

  function pimTimeReg()
  {
    global $g_user;
    $hoursnode = &getNode("timereg.hours");
    $hoursnode->setTable("hoursbase");

    $pda = browserInfo::detectPDA();
    if ($pda) $nodetype = "timereg.hourspda";
    else $nodetype = "timereg.hours";

    $actions = array();
    if(!$pda)
      $actions["edit"] = dispatch_url($nodetype,"edit",array("atkselector"=>"[pk]"));

    // Show only todays bookings, and only your own.
    $hoursnode->addFilter("activitydate",strftime("%Y-%m-%d"));
    $hoursnode->addFilter($hoursnode->getTable().".userid",strtolower($g_user["id"]));

    $recs = $hoursnode->selectDb();

    if (count($recs)>0)
    {
      $recordlist = &atknew("atk.recordlist.atkrecordlist");
      $todayhours = $recordlist->render($hoursnode, $recs, $actions);
    }
    else
    {
      $todayhours = atktext('pim_timeregistration_empty').'<br>';
    }
    if ($pda) $nodetype = "timereg.hourspda";
    else $nodetype = "timereg.hours";
    $todayhours.= '<br>'.href(dispatch_url($nodetype,"add"),atktext('add'),SESSION_NESTED);
    if (!$pda) $todayhours.= ' &nbsp; '.href(dispatch_url($nodetype,"admin"),atktext('pim_gototimeregistration'),SESSION_NESTED);

    $res = '<table border="0">';
    $res.= '<tr>';
    if (!$pda) $res.= '<td width="15">&nbsp;</td>'; // spacer
    $res.= '<td valign="top">'.$todayhours.'</td>';
    $res.= '</tr></table>';

    return $res;
  }

?>