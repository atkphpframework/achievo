<?php

include_once("achievotools.inc");
useattrib("project.projectphaseattrib");
useattrib("project.projectattrib");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");
userelation("atkmanytoonerelation");

function date_format($datestamp,$dateformat)
{ 
  if($dateformat!="")
  {
    return date($dateformat,$datestamp);
  }
  return date("m-d",$datestamp)." (".text(strtolower(date("D",$datestamp))).")";
}

class hours extends atkNode
{
  var $m_lock = "";
  var $m_viewdate = "";
  var $m_weekview = "";  
  var $m_insertmode = ""; // can be set to 'day' or 'multi'  

  function hours()
  {
    global $g_sessionManager,$ATK_VARS;

    // weekview and viewdate are variables that we need to remember..
    $this->m_weekview = $g_sessionManager->pageVar("weekview");
    $this->m_viewdate = $g_sessionManager->pageVar("viewdate");
    if (is_array($this->m_viewdate)) // if set by the date jumper, viewdate is an array.
    {            
      $this->m_viewdate = sprintf("%04d-%02d-%02d", $this->m_viewdate["year"], $this->m_viewdate["month"],$this->m_viewdate["day"]);
    }
    if ($this->m_viewdate=="") $this->m_viewdate = strftime("%Y-%m-%d");
   
    $flags=NF_NO_VIEW;

    $this->atkNode("hours",$flags|NF_MRA); // node() constructor is *not* called automatically!

    // default view if none is set
    if (!isset($this->m_weekview)||$this->m_weekview=="")
    {
      if (atkconfig("timereg_defaultview")=="week") $this->m_weekview=1;
    }

    $this->add(new atkAttribute("id",AF_AUTOKEY));

    $this->add(new atkDateAttribute("activitydate","F d Y","d F Y", 0, date("Ymd"), AF_OBLIGATORY|AF_FORCE_LOAD|($this->m_weekview?0:AF_HIDE_LIST)));

    // force_load flag is set for userid, since several features rely on the 
    // userid, even if it's not displayed on screen.
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD));

    $this->add(new atkProjectPhaseAttribute("phaseid",AF_OBLIGATORY|AF_JS_UPDATEACTIVITY));
    $this->add(new atkListAttribute("activityid",array("dummy","dummy","dummy","dummy","dummy"),array(0,0,0,0,0),AF_OBLIGATORY, 10));
    $this->add(new atkAttribute("remark"));
    $this->add(new atkDurationAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));
    $this->add(new atkDateAttribute("entrydate",AF_HIDE));
    $this->add(new atkDummyAttribute("javascript", "seebelow", AF_NO_LABEL|AF_HIDE_LIST));

    $this->setOrder("hours.id"); // Sort by order of input..

    $this->setTable("hours","hours");

  }
    
  function setLock()
  {
    global $g_user;
    $this->m_lock = $this->getLockType(strtolower($g_user["id"]),$this->m_viewdate);

    if ($this->m_lock != "")
    {
      // This week is locked..
      $flags |= NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT;
    }
  }
  
  function getInitialDate()
  {    
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
    return $initial_date;
   
  }

  function initial_values()
  {
     global $g_user;             
     
     return array("userid"=>array("id"=>$g_user["id"]),
                  "entrydate"=>Array("year"=>date("Y"),
                             "month"=>date("m"),
                             "day"=>date("d")),
                  "activitydate"=>$this->getInitialDate()
                  );
  }

  function activityid_display($rec)
  {
    // activityname is loaded in the phaseid join, so we get the info from there..
    return $rec["phaseid"]["activityname"];
  }

  function action_edit()
  {
    $recs = $this->selectDb($this->m_postvars["atkselector"], "hours.id", "", "", array("userid", "activitydate"));        
    
    $this->m_lock = $this->getLockType($recs[0]["userid"]["id"],
                      sprintf("%02d-%02d-%02d", $recs[0]["activitydate"]["year"], 
                                                $recs[0]["activitydate"]["month"],
                                                $recs[0]["activitydate"]["day"]));
    if ($this->m_lock=="")
    {
      $this->addFilter("hours.userid",strtolower($g_user["id"]));
      return parent::action_edit();
    }
    else
    {
      // Week is locked. We may not edit, only view.
      return $this->action_view();
    }
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

    $this->addFilter("hours.userid",$g_user["id"]);

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
      $res='<form name="dayview" method="post" action="dispatch.php">';
      $res.='<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';
      $res.='<tr><td align="left" valign="top">';
      $viewdatestamp = mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));

      // we substract 3601 seconds to account for Daylight Savings Time
      $yesterday = date('Y-m-d',$viewdatestamp-(86400-3601));

      // we add 3601 seconds to account for Daylight Savings Time
      $tomorrow = date('Y-m-d',$viewdatestamp+86400+3601);
      
      $today = date('Y-m-d');

      $res.= href('dispatch.php?viewdate='.$yesterday, text("houradmin_previousday"));
      if ($this->m_viewdate<date('Y-m-d')) 
      { 
        // If current is not today we can also move forward
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$tomorrow, text("houradmin_nextday"));
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$today, text("houradmin_today"));
      }
      $res.= '&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$this->m_viewdate.'&weekview=1', text("houradmin_gotoweekview"));
      $res.='</td><td align="right" valign="top">';
      $res.=session_form();
      $res.=$this->getDateJumper();
      $res.='&nbsp;<input type="submit" value="'.text("houradmin_goto").'">';
      $res.='</td></tr></table></form>';
      $res.='<b>'.text(strtolower(date("l")))." ".date("F d, Y", $viewdatestamp).'</b>';
    }
    return $res;
  }
  
  function getDateJumper()
  {
    $dummy_rec = array("viewdate"=>$this->getInitialDate());
    $datebox = new atkDateAttribute("viewdate","F d Y","d F Y",0, date("Ymd"),AF_OBLIGATORY);
    return $datebox->edit($dummy_rec);
  }

  function weekview()
  {
    global $g_layout, $g_user;
    atkdebug("SANDY");
    atkdebug($this->m_viewdate);
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
    $g_layout->ui_top(text('title_houradmin_weekview').' '.strftime('%V, %Y',$viewtime));

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

    $userid = $g_user["id"];

    // First a line with previous and next week links..
    $prevnextlinks='<form name="weekview" method="post" action="dispatch.php">';
    $prevnextlinks.=session_form();
    $prevnextlinks.='<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';
    $prevnextlinks.='<tr><td valign="top" align="left">';
    $prevnextlinks.= ''.href('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date'], text('houradmin_previousweek'));
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));
    }
    $prevnextlinks.= '&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.date("Y-m-d"), text('houradmin_thisweek'));
    
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
    $prevnextlinks.='</td><td valign="top" align="right">';
    $prevnextlinks.=$this->getDateJumper();
    $prevnextlinks.='&nbsp;<input type="submit" value="'.text("houradmin_goto").'">';
    $prevnextlinks.='</td></tr></table></form>';

    $prevnextlinks.='<b>'.text("houradmin_week").' '.strftime('%V, %Y',$viewtime).'</b><br><br><br>';

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
                (userid = '$userid' OR userid IS NULL OR userid='')
              AND
                week = '$week'";
    $res = $g_db->getrows($query);
    for ($i=0;$i<count($res);$i++)
    {
      if (empty($res[$i]["userid"])) return "all";
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
    
    // when inserting multiple days at once, we don't break on invalid days 
    // (we will just skip them later on).
    if ($rec["activitydate"]["multi"]!=1)
    {    
      if ($this->getLockType($g_user["id"],sprintf("%04d-%02d-%02d",$rec["activitydate"]["year"],$rec["activitydate"]["month"],$rec["activitydate"]["day"]))!="")
      {
        triggerError($rec, "activitydate", "houradmin_weeklocked");
      }
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
        // The validation of this flag is done later on by the default 
        // ::validate() method of the base class.
        $this->m_attribList["remark"]->m_flags |= AF_OBLIGATORY;
      }
    }
    else // something is wrong with the chosen activity if we can't find
         // a record for it.
    {
      triggerError($record, "activityid", "error_obligatoryfield");
    }
    
    // when inserting multiple days at once, we don't break on invalid days 
    // (we will just skip them later on).    
    if ($record["activitydate"]["multi"]!=1)
    {
      $activitydate = sprintf("%04d-%02d-%02d", $record["activitydate"]["year"], $record["activitydate"]["month"], $record["activitydate"]["day"]);
      $new_minutes = $record["time"]["hours"]*60 + $record["time"]["minutes"];

      if ($this->exceed_hrs_in_day($record["userid"]["id"], $activitydate, $new_minutes, $record["id"]))
      {
        triggerError($record, "time", "error_hoursindayexceeded");
      }
    }
    else // when inserting multiple days at once, we must verify if end is not smaller than start.
    {
      $start = date("Y-m-d", mktime(12,0,0,$record["activitydate"]["month"],$record["activitydate"]["day"],$record["activitydate"]["year"]));
      $todate = $this->m_postvars["todate"]; // todate is not part of $record, because the todate attrib isn't really a part of the node.      
      $end = date("Y-m-d", mktime(12,0,0,$todate["month"],$todate["day"],$todate["year"]));
      atkdebug("validating $start vs $end");
      if ($start>$end)
      {
        triggerError($record, "activitydate", "error_startbiggerthanend");
      }
    }
    // continue with original validation
    atkNode::validate($record, $mode);
  }
  
  function exceed_hrs_in_day($userid, $date, $new_minutes, $recordid="")
  {
    global $g_db;

    $query = "SELECT sum(time) + $new_minutes minutes_in_day
              FROM hours 
              WHERE 
                userid = '$userid' 
                AND activitydate = '$date'";
                
    // If we are editing a record, than the old value of the current record
    // must not be counted, but the new value (which was already added as
    // $new_minutes).
    if ($recordid!="") $query.= " AND id <> $recordid";
    $res = $g_db->getrows($query);
    if (count($res)==0)
    {
      // no records for $date
      return false;
    }
    else
    {
      return ($res[0]["minutes_in_day"] > 24*60 );
    }
  }    
  
  function addPage($record="")
  {
    global $g_sessionManager;
   // Insert mode can be "" (normal) or "multi" (multiple days at once).
    $this->m_insertmode = $g_sessionManager->stackVar("insertmode");    
    if ($this->m_insertmode=="multi")
    {
      // When in multidate mode, add an extra attribute for filtering
      // what days we want to filter.      
      $this->add(new atkDummyAttribute("dayfilter", "", AF_HIDE_EDIT|AF_HIDE_LIST));
    }
   
    return parent::addPage($record);
  }
  
  function activitydate_edit($record, $prefix="", $mode="")
  {  
    if ($mode=="add")
    {
      // We support 2 modes... one for single day entry, one for multiday entry.
      if ($this->m_insertmode=="multi")
      {
        $res = $this->m_attribList["activitydate"]->edit($record, $prefix, $mode);
        $toDate = new atkDateAttribute("todate", "F d Y","d F Y", 0, date("Ymd"), AF_OBLIGATORY);
        
        $res.= " ".text("uptoandincluding")." ".$toDate->edit($record, $prefix, $mode);
        $res.= '<input type="hidden" name="'.$prefix.'activitydate[multi]" value="1">';
        $newmode = "day";
      }
      else
      {
        $res = $this->m_attribList["activitydate"]->edit($record, $prefix, $mode);      
        $newmode = "multi";
      }
      $res.= " ".href("dispatch.php?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&insertmode=".$newmode, 
                      text("insert_".$newmode, $this->m_type, $this->m_module), 
                      SESSION_DEFAULT, 
                      true);
      return $res;
    }
    else
    {
      return $this->m_attribList["activitydate"]->edit($record, $prefix, $mode);
    }
  }
  
  function dayfilter_edit($record, $prefix="", $mode="")
  {
    $res= "<BR>".text("filterdays", $this->m_type, $this->m_module)."<BR>";
    $days = array("mon", "tue", "wed", "thu", "fri", "sat", "sun");    
    
    // we use a hidden variable to indicate that the dayfilter has been edited.
    // If it has not been edited, dayfilter[edited] will not be 1, and we will
    // assume a default dayfilter of mon-fri. TODOFIXME: take the correct
    // day values from the user's contract.
    $res.='<input type="hidden" name="'.$prefix.'dayfilter[edited]" value="1">';
    for ($i=0;$i<7;$i++)
    {
      if ($record["dayfilter"]["edited"]==1)
      {
        $checked = ($record["dayfilter"][$i]==1?"checked":"");
      }
      else
      {
        $checked = ($i<5?"checked":"");
      }
      $res.= text($days[$i]).': <input type="checkbox" name="'.$prefix.'dayfilter['.$i.']" value="1" '.$checked.'> &nbsp; ';
    }
    return $res;
  }
  
  function addDb(&$record, $exectrigger=true)
  {
    global $g_user;
    
    if ($record["activitydate"]["multi"]==1)
    {      
      // We're registering multiple days at once. 
      $walkerStamp = mktime(12,0,0,$record["activitydate"]["month"],$record["activitydate"]["day"],$record["activitydate"]["year"]);
      $walker = date("Y-m-d", $walkerStamp);      
      
      $todate = $this->m_postvars["todate"]; // todate is not part of $record, because the todate attrib isn't really a part of the node.
      
      $end = date("Y-m-d", mktime(12,0,0,$todate["month"],$todate["day"],$todate["year"]));
      
      // Make a quick lookup array in which we can easily see if a day was checked.
      $doDays=array();
      for ($i=0; $i<7; $i++)
      {
        if ($this->m_postvars["dayfilter"][$i]==1)
        {
          $doDays[] = (($i+1)%7); // we started with monday as 0, but php date() function counts sunday as 0.
        }
      }      
      
      for ($i=1; $walker <= $end ; $i++)
      {                       
        $dow = date("w", $walkerStamp);
        if (!in_array($dow, $doDays) || $this->getLockType($g_user["id"],$walker)!="")
        {
          atkdebug("Skipping $walker, for this day is locked or not checked");
        }
        else
        {
          $newRecord = $record; // We're going to save the data we entered.
          // but replace the activitydate field with the walker:
          $newRecord["activitydate"] = array("year"=>date("Y", $walkerStamp),
                                             "month"=>date("m", $walkerStamp),
                                             "day"=>date("d", $walkerStamp));
          parent::addDb($newRecord);
        }
        
        // End of day, add 1 day and reloop.
        $walkerStamp = mktime(12,0,0,$record["activitydate"]["month"],
                                     $record["activitydate"]["day"]+$i,
                                     $record["activitydate"]["year"]);
        $walker = date("Y-m-d", $walkerStamp);
                              
      }
    }
    else
    {
      parent::addDb($record, $exectrigger);
    }
  }
 
  /**
   * This method is called by the framework to determine the allowed actions on 
   * the hours node.
   * In this case, the edit and delete actions are disabled if the record does
   * not belong to the current user.
   * TODO/FIXME: add some supervisor right so some users may edit other users' 
   * entries.
   */
  function recordActions($rec, &$actions, &$mraactions)
  {
    $user = getUser();        
    if ($user["id"]=="" || $rec["userid"]["id"]!=$user["id"] 
        || $this->getLockType($rec["userid"]["id"],
                              sprintf("%02d-%02d-%02d", $rec["activitydate"]["year"], 
                                                        $rec["activitydate"]["month"],
                                                        $rec["activitydate"]["day"]))!="")
    {
      unset($actions["edit"]);
      unset($actions["delete"]);
      $idx = array_search("delete", $mraactions);
      if ($idx!==false)
      {
        unset($mraactions[$idx]);
      }
    }
  }
}

?>