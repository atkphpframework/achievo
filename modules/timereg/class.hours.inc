<?php

include_once "achievotools.inc";
useattrib("project.projectphaseattrib");
useattrib("project.projectattrib");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");

function date_format($datestamp)
{
  return date("m-d",$datestamp)." (".text(strtolower(date("D",$datestamp))).")";
}

class hours extends atkNode
{
  var $m_lock = "";
  var $m_viewdate = "";
  var $m_weekview = "";

  function hours()
  {
    global $g_sessionManager;

    // weekview and viewdate are variables that we need to remember..
    $this->m_weekview = $g_sessionManager->stackVar("weekview");
    $this->m_viewdate = $g_sessionManager->stackVar("viewdate");
    if ($this->m_viewdate=="") $this->m_viewdate = strftime("%Y-%m-%d");

    $flags=NF_NO_VIEW;

    $this->atkNode("hours",$flags|NF_MRA); // node() constructor is *not* called automatically!

    // default view if none is set
    if (!isset($this->m_weekview)||$this->m_weekview=="")
    {
      if (atkconfig("timereg_defaultview")=="week") $this->m_weekview=1;
    }

    $this->add(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));

    $this->add(new atkDateAttribute("activitydate","F d Y","d F Y", 0, date("Ymd"), AF_OBLIGATORY|($this->m_weekview?0:AF_HIDE_LIST)));

    $this->add(new atkAttribute("userid",AF_HIDE));

    $this->add(new atkProjectPhaseAttribute("phaseid",AF_OBLIGATORY|AF_JS_UPDATEACTIVITY));
    $this->add(new atkListAttribute("activityid",Array("dummy","dummy","dummy","dummy","dummy"),Array(),AF_OBLIGATORY));
    $this->add(new atkAttribute("remark"));
    $this->add(new atkDurationAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));
    $this->add(new atkDateAttribute("entrydate",AF_HIDE));
    $this->add(new atkDummyAttribute("javascript", "seebelow", AF_NO_LABEL|AF_HIDE_LIST));

    $this->setOrder("id"); // Sort by order of input..

    $this->setTable("hours","hours");

  }

  function setLock()
  {
    global $g_user;
    $this->m_lock = $this->getLockType(strtolower($g_user["name"]),$this->m_viewdate);

    if ($this->m_lock != "")
    {
      // This week is locked..
      $flags |= NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT;
    }
  }

  function initial_values()
  {
     global $g_user;
     $initial_date = Array();

     if ($this->m_viewdate!="")
     {
       $initial_date = Array("year"=>substr($this->m_viewdate,0,4),
                             "month"=>substr($this->m_viewdate,5,2),
                             "day"=>substr($this->m_viewdate,8,2));
     }
     else
     {
       $initial_date = Array("year"=>date("Y"),
                             "month"=>date("m"),
                             "day"=>date("d"));
     }
     return array("userid"=>strtolower($g_user["name"]),
                  "entrydate"=>Array("year"=>date("Y"),
                             "month"=>date("m"),
                             "day"=>date("d")),
                  "activitydate"=>$initial_date);
  }

  function activityid_display($rec)
  {
    // activityname is loaded in the phaseid join, so we get the info from there..
    return $rec["phaseid"]["activityname"];
  }

  function action_edit()
  {
    global $g_user;
    $this->setLock();
    $this->addFilter("userid",strtolower($g_user["name"]));
    return parent::action_edit();
  }

  function javascript_edit($rec)
  {
    global $currentphase, $currentactivity;

    if (!isset($currentphase)||$currentphase=="") $currentphase=0;
    if (!isset($currentactivity)||$currentactivity=="") $currentactivity =0;
    $result = '<script language="javascript"> set_activities('.$currentphase.','.$currentactivity.'); </script>';

    return $result;
  }

  // override the admin action, because we have the weekview/dayview switch..
  function action_admin()
  {
    global $g_user;

    $this->setLock();

    $this->addFilter("userid",strtolower($g_user["name"]));

    if ($this->m_weekview!=1)
    {
      if ($this->m_lock=="") $this->addPage();
      $this->addFilter("activitydate",$this->m_viewdate);
      $this->adminPage();
    }
    else
    {
      if (atkconfig("timereg_week_bookable")==true && $this->m_lock=="") $this->addPage();
      $this->weekview();
    }
  }

  function adminHeader()
  {
    if ($this->m_weekview!=1)
    {
      $viewdatestamp = mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));

      // we substract 3601 seconds to account for Daylight Savings Time
      $yesterday = date('Y-m-d',$viewdatestamp-(86400-3601));

      // we add 3601 seconds to account for Daylight Savings Time
      $tomorrow = date('Y-m-d',$viewdatestamp+86400+3601);

      $res = href('dispatch.php?viewdate='.$yesterday, text("houradmin_previousday"));
      $res.='&nbsp;&nbsp;<b>'.date_format($viewdatestamp).'</b>';
      if ($this->m_viewdate<date('Y-m-d')) // If current is not today we can also move forward
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$tomorrow, text("houradmin_nextday"));
      $res.= '<br><br>'.href('dispatch.php?viewdate='.$this->m_viewdate.'&weekview=1', text("houradmin_gotoweekview"));
    }
    return $res;
  }

  function weekview()
  {
    global $g_layout, $g_user;

    if ($this->m_viewdate!="")
    {
      $viewtime = mktime(12,12,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));
    }
    else
    {
      $this->m_viewdate = date("Y-m-d");
      $viewtime = time();
    }
    $g_layout->initGUI();
    $g_layout->ui_top(text('title_houradmin_weekview').' '.strftime('%V',$viewtime));

    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
      $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $weekdates[$i]['day']=text(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
      $weekdates[$i]['stamp']=$viewtime-(86400*($weekday-$i));
    }

    $week = Array();
    $projtotals = Array();
    $daytotals = Array();

    $data = $this->selectDb("activitydate >= '".$weekdates[1]['date']."' AND activitydate <= '".$weekdates[7]['date']."'","","",$this->m_listExcludes);

    for ($i=0;$i<count($data);$i++)
    {
      $rec = $data[$i];
      $key = $rec["phaseid"]["projname"]."-".$rec["phaseid"]["phasename"];
      $date = $rec["activitydate"]["year"]."-".$rec["activitydate"]["month"]."-".$rec["activitydate"]["day"];
      $time = ($rec["time"]["hours"]*60)+$rec["time"]["minutes"];
      $week[$key][$date] += $time;
      $projtotals[$key] += $time;
      $daytotals[$date] += $time;
      $total += $time;
    }

    $userid = $g_user["name"];

    // First a line with previous and next week links..
    $prevnextlinks = '<br>'.href('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date'], text('houradmin_previousweek'));
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));
    }

    if ($this->m_lock=="individual" && $this->allowed("unlock"))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=unlock&userid='.$userid.'&viewdate='.$this->m_viewdate,'<b>'.text("unlock").'</b>');
    }
    // users may only lock weeks that are not already locked. they must have the proper permission
    // and, if configured, they may only lock a week once they have booked all their hours.
    else if ($this->m_lock=="" && $this->allowed("lock") && (atkconfig("timereg_incompleteweeklock") || $this->weekComplete($userid, $this->m_viewdate)))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=lock&userid='.$userid.'&viewdate='.$this->m_viewdate,'<b>'.text("lock").'</b>');
    }

    $prevnextlinks.='<br><br>';

    $g_layout->output($prevnextlinks);
    $g_layout->output($g_layout->data_top());
    $g_layout->output($g_layout->tr_top());

    $g_layout->td_datatitle(text('project').'/'.text('phase'));
    for ($i=1;$i<=7;$i++)
    {
      if ($weekdates[$i]['date'] <= date("Y-m-d"))
      {
        $url = 'dispatch.php?viewdate='.$weekdates[$i]['date']."&weekview=0";
        $g_layout->td_datatitle(href($url,text($weekdates[$i]['day'])).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')','align="center"');
      }
      else
      {
        $g_layout->td_datatitle(text($weekdates[$i]['day']).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')','align="center"');
      }

    }
    $g_layout->td_datatitle(text('total'));

    $g_layout->output($g_layout->tr_bottom());

    while (list($proj, $times)=each($week))
    {
      $g_layout->output($g_layout->tr_top());
      $g_layout->td($proj);
      for ($i=1;$i<=7;$i++)
      {
        $g_layout->td(time_format($times[$weekdates[$i]['date']]), 'align="center"');
      }
      $g_layout->td(time_format($projtotals[$proj]), 'align="center"');
      $g_layout->output($g_layout->tr_bottom());
    }

    $g_layout->output($g_layout->tr_top());

    $g_layout->td_datatitle(text('total'));

    for ($i=1;$i<=7;$i++)
    {
      if ($daytotals[$weekdates[$i]['date']]>atkconfig("overtimethreshold"))
      {
        $color = "#FF0000";
      }
      else
      {
        $color = "#009900";
      }
      $g_layout->td_datatitle('<font color="'.$color.'">'.time_format($daytotals[$weekdates[$i]['date']]).'</font>','align="center"');
    }
    $g_layout->td_datatitle(time_format($total), 'align="center"');

    $g_layout->output($g_layout->tr_bottom());
    $g_layout->output($g_layout->data_bottom());
    $g_layout->ui_bottom();
  }

  function postAdd($rec)
  {
    // if we add a record on a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_viewdate=$rec["activitydate"]["year"]."-".sprintf("%02d",$rec["activitydate"]["month"])."-".sprintf("%02d",$rec["activitydate"]["day"]);
  }

  function postUpdate($rec)
  {
    // if we edit a record and set it to a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_viewdate=$rec["activitydate"]["year"]."-".sprintf("%02d",$rec["activitydate"]["month"])."-".sprintf("%02d",$rec["activitydate"]["day"]);
  }

  // Check if this week may be booked..
  // returns: "" - not locked
  //          "individual" - locked for the user only
  //          "all"       - locked for everybody (can only be unlocked by administrator)
  function getLockType($userid, $date)
  {
    global $g_db;

    $week = weekstamp(mktime(0,0,0,substr($date,5,2),
                                   substr($date,8,2),
                                   substr($date,0,4)));

    $query = "SELECT
                week, userid
              FROM
                hours_lock
              WHERE
                (userid = '$userid' OR userid = '*')
              AND
                week = '$week'";
    $res = $g_db->getrows($query);
    for ($i=0;$i<count($res);$i++)
    {
      if ($res[$i]["userid"] == '*') return "all";
    }

    // apparently not locked for everybody.
    if (count($res)>0)
    {
      return "individual";
    }
    return "";
  }

  function action_lock()
  {
    global $g_db;

    $viewdate = $this->m_postvars["viewdate"];
    $userid = $this->m_postvars["userid"];
    $week = weekstamp(mktime(0,0,0,substr($viewdate,5,2),
                                   substr($viewdate,8,2),
                                   substr($viewdate,0,4)));
    $query = "INSERT INTO hours_lock (week, userid) VALUES ('$week','$userid')";
    $g_db->query($query);
    $this->redirect();
  }

  function action_unlock()
  {
    global $g_db;

    $viewdate = $this->m_postvars["viewdate"];
    $userid = $this->m_postvars["userid"];
    $week = weekstamp(mktime(0,0,0,substr($viewdate,5,2),
                                   substr($viewdate,8,2),
                                   substr($viewdate,0,4)));
    $query = "DELETE FROM hours_lock WHERE week = '$week' AND userid = '$userid'";
    $g_db->query($query);
    $this->redirect();
  }

  /**
   * This function checks whether a week is 'compleet'. This means that the user
   * has booked the same ammount of hours (or more) as is specified in his contract.
   * Note: This is checked against the contract that was valid on the first day
   * of the week.
   */
  function weekComplete($userid, $date)
  {
    global $g_db;

    $start = startOfWeek($date);
    $end = endOfWeek($date);

    $query = "SELECT (uc_hours*60)-sum(time) as leftover
              FROM
                hours LEFT JOIN usercontract
                ON hours.userid = usercontract.userid
              WHERE
                startdate <= '$start'
                AND enddate > '$start'
                AND hours.userid = '$userid'
                AND activitydate between '$start' and '$end'
              GROUP BY hours.userid";
    $res = $g_db->getrows($query);
    if (count($res)==0)
    {
      // either no contract in this period, or no hours booked at all..
      return false;
    }
    else
    {
      return ($res[0]["leftover"]<=0);
    }
  }

  function activitydate_validate(&$rec)
  {
    global $g_user;
    if ($this->getLockType($g_user["name"],sprintf("%04d-%02d-%02d",$rec["activitydate"]["year"],$rec["activitydate"]["month"],$rec["activitydate"]["day"]))!="")
    {
      triggerError($rec, "activitydate", "houradmin_weeklocked");
    }
  }

  /**
   * We override the validate method, so we can check if a remark is required
   * for the chosen activity. If it is, we add the AF_OBLIGATORY flag to the
   * remark before the validation.
   *
   * (First thought would be to create a remark_validate() function, but empty
   * values are not validated by atkNode's validate function. That's why we
   * need to override validate.)
   */
  function validate(&$record, $mode)
  {
    $obj = getNode("project.activity");
    $act = $obj->selectDb("id=".$record["activityid"]);
    if (count($act)==1)
    {
      if ($act[0]["remarkrequired"]==1)
      {
        // Specification is required. So we set the obligatory flag.
        $this->m_attribList["remark"]->m_flags |= AF_OBLIGATORY;
      }
    }
    else // something is wrong with the chosen activity if we can't find
         // a record for it.
    {
      triggerError($record, "activityid", "error_obligatoryfield");
    }

    // continue with original validation
    atkNode::validate($record, $mode);
  }

}

?>
