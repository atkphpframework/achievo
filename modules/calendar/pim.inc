<?php
  function pimSchedule()
  {
    global $g_user, $g_layout, $g_db;

    $me = $g_user["id"];

    $schedulenode = getNode("calendar.schedule");
    $schedulenode->m_flags |= NF_NO_SEARCH;

    $date = date('Y-m-d');

    //search for appointments where user is owner and where user is attendee
    $name = "atk".atkconfig("database")."query";
    $query = new $name();
    $query->addTable('schedule');
    $query->addJoin('schedule_attendee', '', 'schedule_attendee.schedule_id=schedule.id', TRUE);
    $query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);

    $query->addField('id', ' ', 'schedule');
    $query->addField('startdate', ' ', 'schedule');
    $query->addField('enddate', ' ', 'schedule');
    $query->addField('starttime', ' ', 'schedule');
    $query->addField('endtime', ' ', 'schedule');
    $query->addField('title', ' ', 'schedule');
    $query->addField('description', ' ', 'schedule');
    $query->addField('location', ' ', 'schedule');
    $query->addField('allday', ' ', 'schedule');
    $query->addField('publicitem', ' ', 'schedule');
    $query->addField('owner', ' ', 'schedule');
    $query->addField('status', ' ', 'schedule');
    $query->addField('scheduletype', ' ', 'schedule');    

    $query->addCondition("schedule.startdate <='$date' AND schedule.enddate >= '$date'");
    $query->addCondition("(schedule.owner='".$me."' OR person.id='".$me."' OR schedule.all_users = 1)");

    $query->addOrderBy('starttime');
    $querystring = $query->buildSelect(TRUE);
    $recs = $g_db->getRows($querystring);
    $actions = array();

    for ($i=0;$i<count($recs);$i++)
    {
      $attribute = &$schedulenode->m_attribList["startdate"];
      $recs[$i]["startdate"] = $attribute->db2value($recs[$i]);
      $attribute = &$schedulenode->m_attribList["starttime"];
      $recs[$i]["starttime"] = $attribute->db2value($recs[$i]);
      $attribute = &$schedulenode->m_attribList["endtime"];
      $recs[$i]["endtime"] = $attribute->db2value($recs[$i]);
    }

    if (count($recs)>0)
    {
      $todayschedule = $schedulenode->recordList($recs,$actions,false, array('publicitem','allday','location','scheduletype','enddate','all_users'));
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
