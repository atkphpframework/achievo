<?php

include_once "class.projectphaseattrib.inc";
include_once "class.projectattrib.inc";
include_once "achievotools.inc";

function time_format($time)
{
  if ($time==0) return "&nbsp;";
  return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
}    
  
function date_format($datestamp)
{
  return date("m-d",$datestamp)." (".text(strtolower(date("D",$datestamp))).")";
}

class hours extends atkNode
{
  var $m_lock = "";

  function hours()
  {
    global $g_user,$atkaction,$atkorderby, $g_sessionManager, $weekview, $viewdate; 
    
    // weekview and viewdate are variables that we need to remember..    
    $weekview = $g_sessionManager->stackVar("weekview");
    $viewdate = $g_sessionManager->stackVar("viewdate");
    if ($viewdate=="") $viewdate = strftime("%Y-%m-%d");
    
    // hoursurvey has extra sticky vars..
    if ($atkaction=="hoursurvey")
    {
      // TODO: this doesn't work any more with the new session management code.
      // $this->addStickyVar(Array("projectid","userid","phaseid","activityid"));    
    }
    
    $this->atkNode("hours"); // node() constructor is *not* called automatically!
    
    $flags=NF_NO_VIEW;    
    if ($atkaction!='hoursurvey')
    {
      $this->m_lock = $this->getLockType(strtolower($g_user["name"]),$viewdate);
    
      if ($this->m_lock != "")
      {
        // This week is locked..       
        $flags |= NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT;    
      }
    }
    
    $this->atkNode("hours",$flags); // node() constructor is *not* called automatically!
   
    $this->addAttribute(new atkDateAttribute("activitydate"));
    // default view if none is set
    if (!isset($weekview)||$weekview=="")
    {
      if (atkconfig("timereg_defaultview")=="week") $weekview=1;
    }
    
    //$this->addAttribute(new atkDateAttribute("activitydate"));

    if($atkaction!='hoursurvey')
    {
      $this->addFilter("userid",strtolower($g_user["name"])); 
    }
    if($atkaction!='hoursurvey'&&$atkaction!='edit')
    {
      if ($weekview!=1)
      {
        $this->addFilter("activitydate",$viewdate); 
      }        
    }

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
                
    if($atkaction=="hoursurvey"||$weekview==1)
    {
      $this->addAttribute(new atkDateAttribute("activitydate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY));
    }
    else
    {
      $this->addAttribute(new atkDateAttribute("activitydate","F d Y","d F Y", 0, date("Ymd"), AF_OBLIGATORY|AF_HIDE_LIST));
    }
    
    if ($atkaction=="hoursurvey")
    {
      $this->addAttribute(new atkAttribute("userid"));
    }
    else
    {
      $this->addAttribute(new atkAttribute("userid",AF_HIDE));
    }
    
    $this->addAttribute(new atkProjectPhaseAttribute("phaseid",AF_OBLIGATORY));
    
    $this->addAttribute(new atkListAttribute("activityid",Array("dummy","dummy","dummy","dummy","dummy"),Array(),AF_OBLIGATORY));      
    
    $this->addAttribute(new atkAttribute("remark"));      
    
    $this->addAttribute(new atkDurationAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));
                
    $this->addAttribute(new atkDateAttribute("entrydate",AF_HIDE));      
                
    $this->addAttribute(new atkDummyAttribute("javascript", "seebelow", AF_NO_LABEL|AF_HIDE_LIST));

    if($atkaction=="hoursurvey")
    {
      $this->setOrder("id");
    }
    else
    {
      if($atkorderby!="") $this->setOrder($atkorderby);
    }
    
    $this->setTable("hours","hours");
   
  }
      
  function initial_values()
  {
     global $g_user, $viewdate;

     $initial_date = Array();    
     
     if ($viewdate!="")
     {
       $initial_date = Array("year"=>substr($viewdate,0,4),
                             "month"=>substr($viewdate,5,2),
                             "day"=>substr($viewdate,8,2));
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
     
  function action_hoursurvey()
  {      
    include 'hoursurvey.inc';
  }
  
  function javascript_edit($rec)
  {
    global $currentphase, $currentactivity, $weekview, $viewdate;
    
    if (!isset($currentphase)||$currentphase=="") $currentphase=0;
    if (!isset($currentactivity)||$currentactivity=="") $currentactivity =0;
    $result = '<script language="javascript"> set_activities('.$currentphase.','.$currentactivity.'); </script>';
        
    return $result;
  }
   
  // override the admin action, because we have the weekview/dayview switch..
  function action_admin()
  {      
    global $weekview;
    
    if ($weekview!=1)
    {
      if ($this->m_lock=="") $this->addPage();
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
    global $weekview, $viewdate;
    if ($weekview!=1)
    {
      $viewdatestamp = mktime(0,0,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
      $yesterday = date('Y-m-d',$viewdatestamp-86400);
      $tomorrow = date('Y-m-d',$viewdatestamp+86400);
    
      $res = href('dispatch.php?viewdate='.$yesterday, text("houradmin_previousday"));
      $res.='&nbsp;&nbsp;<b>'.date_format($viewdatestamp).'</b>';
      if ($viewdate<date('Y-m-d')) // If current is not today we can also move forward
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$tomorrow, text("houradmin_nextday"));
      $res.= '<br><br>'.href('dispatch.php?viewdate='.$viewdate.'&weekview=1', text("houradmin_gotoweekview"));
    }
    return $res;
  }
  
  function weekview()
  {
    global $g_layout, $viewdate, $g_user;
    
    if ($viewdate!="")
    {
      $viewtime = mktime(0,0,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
    }
    else
    {
      $viewdate = date("Y-m-d");
      $viewtime = time();
    }
    
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
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=unlock&userid='.$userid.'&viewdate='.$viewdate,'<b>'.text("unlock").'</b>');
    }
    // users may only lock weeks that are not already locked. they must have the proper permission
    // and, if configured, they may only lock a week once they have booked all their hours.
    else if ($this->m_lock=="" && $this->allowed("lock") && (atkconfig("timereg_incompleteweeklock") || $this->weekComplete($userid, $viewdate)))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=lock&userid='.$userid.'&viewdate='.$viewdate,'<b>'.text("lock").'</b>');
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
    $this->m_postvars["viewdate"]=$rec["activitydate"]["year"]."-".sprintf("%02d",$rec["activitydate"]["month"])."-".sprintf("%02d",$rec["activitydate"]["day"]);
  }
  
  function postUpdate($rec)
  {
    // if we edit a record and set it to a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_postvars["viewdate"]=$rec["activitydate"]["year"]."-".sprintf("%02d",$rec["activitydate"]["month"])."-".sprintf("%02d",$rec["activitydate"]["day"]);
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
                ON userid = uc_userid
              WHERE
                uc_startdate <= '$start'
                AND uc_enddate > '$start'
                AND userid = '$userid'
                AND activitydate between '$start' and '$end'
              GROUP BY userid";
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
  
}
?>