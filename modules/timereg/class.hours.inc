<?php

include_once("achievotools.inc");
useattrib("project.projectphaseattrib");
useattrib("project.projectattrib");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atktextattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");
useattrib("atktimeattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");

function date_format($datestamp,$dateformat)
{
  if($dateformat!="")
  {
    return date($dateformat,$datestamp);
  }
  return date("m-d",$datestamp)." (".atktext(strtolower(date("D",$datestamp))).")";
}

/**
* Converts the starttime - endtime notation to a duration notation
*/
function convertHoursToDuration(&$record)
{
  $starttime  += $record["starttime"]["hours"]   *  60;
  $starttime  += $record["starttime"]["minutes"] *   1;
  $starttime  += $record["starttime"]["seconds"] /  60;

  $endtime    += $record["endtime"]["hours"]     *  60;
  $endtime    += $record["endtime"]["minutes"]   *   1;
  $endtime    += $record["endtime"]["seconds"]   /  60;

  $record["time"] = $endtime - $starttime;
}

function is_leapyear($date)
{
  if ($date["year"]%4 == 0)
    if ($date["year"]%100 == 0)
      if ($date["year"]%400 == 0)
        if ($date["year"]%4000 == 0)
          return false;
        else
          return true;
      else
        return false;
    else
      return true;
  else
    return false;
}

function convertDurationToHours(&$record, $int = false)
{
  if ($record['starttime'] && $record["time"]!==null)
  {
    // Seperate minutes from hours
    $minutes = $record['time']%60;
    $record['time']               = $record['time']-$minutes;

    // Add the hours to the begintime so we'll get the hours of the endtime
    if ($int)
      $record['endtime']['hours'] = $record['starttime']['hours'] + $record['time']/60;
    else
      $record['endtime']['hours'] = "".$record['starttime']['hours'] + $record['time']/60 ."";

    // Next we'll do the minutes
    $record['time']               = $minutes;

    // Seperate seconds from minutes
    $seconds = $record['time']%1;
    $record['time']               = $record['time']-$seconds;

    // Check wether we don't have more than 60 minutes, otherwise we add another hour
    $record['endtime']['hours'] += intval(($record['starttime']['minutes']+$record['time'])/60);
    $record['time'] = ($record['starttime']['minutes']+$record['time'])%60;

    // Add the minutes to the begintime so we'll get the minutes of the endtime
    if ($int)
      $record['endtime']['minutes'] = $record['time']/1;
    else
      $record['endtime']['minutes'] = "".$record['time']/1 ."";

    // Next we'll do the seconds
    $record['time']               = $seconds;

    // All we have left is the seconds, so we round that
    $record['time']=round($record['time']*60);

    // Check wether we don't have more than 60 seconds, otherwise we add another minute
    $record['endtime']['minutes'] += intval(($record['starttime']['seconds']+$record['time'])/60);
    $record['time'] = ($record['starttime']['seconds']+$record['time'])%60;

    // Add the seconds to the starttime to get the endtime seconds
    if ($int)
      $record['endtime']['seconds']   = $record['time'];
    else
      $record['endtime']['seconds']   = "".$record['time'] ."";

    if ($int)
    {
      $record['starttime']['hours'] = $record['starttime']['hours']*1;
      $record['starttime']['minutes'] = $record['starttime']['minutes']*1;
      $record['starttime']['seconds'] = $record['starttime']['seconds']*1;
    }
  }
}

class to_date extends atkDateAttribute
{
  function to_date()
  {
    $this->atkDateAttribute("todate", "","", 0, date("Ymd"), AF_NO_LABEL|AF_HIDE);
  }

  function load() { return array();}
  function store() { return true;}
  function addtoeditarray() {}
}

class to_time extends atkTimeAttribute
{
  function to_time()
  {
    $this->atkTimeAttribute("endtime","0","23",array(0,15,30,45),"12", AF_OBLIGATORY);
  }

  function display($record)
  {
    convertDurationToHours($record);
    return parent::display($record);
  }

  function load() { return array();}
  function store() { return true;}
  function getOrderByStatement(){}
}

class hours extends atkNode
{
  var $m_lock = "";
  var $m_viewdate = "";
  var $m_weekview = "";
  var $m_user = "";
  var $m_insertmode = ""; // can be set to 'day' or 'multi'

  function hours($type="hours", $flags=0)
  {
    global $g_sessionManager,$g_user;

    // weekview and viewdate are variables that we need to remember..
    $this->m_weekview = $this->processGlobalVar("weekview");
    $this->m_viewdate = $this->processGlobalVar("viewdate");
    $this->m_user = $this->processGlobalVar("viewuser");

    if (is_array($this->m_viewdate)) // if set by the date jumper, viewdate is an array.
    {
      $this->m_viewdate = sprintf("%04d-%02d-%02d", $this->m_viewdate["year"], $this->m_viewdate["month"],$this->m_viewdate["day"]);
    }
    if ($this->m_viewdate=="") $this->m_viewdate = strftime("%Y-%m-%d");
    if ($this->m_user == "") $this->m_user = $g_user["id"];

    $flags|=NF_NO_VIEW|NF_TRACK_CHANGES|NF_NO_IMPORT;

    $this->atkNode($type,$flags|NF_MRA|NF_NO_ADD); // node() constructor is *not* called automatically!

    // default view if none is set
    if (!isset($this->m_weekview)||$this->m_weekview=="")
    {
      if (atkconfig("timereg_defaultview")=="week") $this->m_weekview=1;
    }

    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));

    $this->add(new atkDateAttribute("activitydate","","", 0, (atkconfig("timereg_allowfuture", false)?0:date("Ymd")), AF_OBLIGATORY|AF_FORCE_LOAD|($this->m_weekview?0:AF_HIDE_LIST)));
    $this->add(new to_date());

    // force_load flag is set for userid, since several features rely on the
    // userid, even if it's not displayed on screen.
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY));

    $this->addPhaseAndActivity();
    $this->add(new atkTextAttribute("remark", atkconfig("timereg_remark_lines", 1), 0));
    $this->addTime();

    // If we don't use duration, then we won't need this
    // Because we can set the rates with the billing module
    if (atkconfig("use_duration", true))
    {
      $relation = &new atkManyToOneRelation("workperiod","timereg.workperiod", AF_HIDE_LIST|AF_RELATION_NO_NULL_ITEM);
      $relation->setHideWhenEmpty(true);
      $this->add($relation);
    }

    $this->add(new atkDateAttribute("entrydate",AF_HIDE|AF_OBLIGATORY));
    $this->add(new atkDummyAttribute("javascript", "seebelow", AF_NO_LABEL|AF_HIDE_LIST));

    $this->setOrder("B.abbreviation"); // Sort by project abbreviation

    $this->setTable("hours","hours");
    $this->addSecurityMap("mra_move", "edit");
  }

  function action_mra_move(&$handler)
  {
    $selector = atkArrayNvl($this->m_postvars, "atkselector", array());
    $this->redirect(session_url(dispatch_url("timereg.movehours", "add", array("atkselector"=>$selector)), SESSION_REPLACE));
  }

  function processGlobalVar($name, $value="")
  {
    global $g_sessionManager;
    $session = &$g_sessionManager->getSession();
    $sessionvarid = "timereg_hours_$name";
    if (empty($value) && isset($_REQUEST[$name])) $value = $_REQUEST[$name];
    if ($value!=="") $session[$sessionvarid] = $value;
    return $session[$sessionvarid];
  }

  function descriptor(&$record)
  {
    $datestr = date("d-m-Y", dateutil::arr2stamp($record["activitydate"]));
    $employeenode = &atkGetNode("employee.employee");
    $users = $employeenode->selectDb("person.id='{$record["userid"]["id"]}'");
    return sprintf("%s, %s (%s)", $users[0]["lastname"], $users[0]["firstname"],$datestr);
  }

  function addTime()
  {
    $durationflags = AF_TOTAL;

    if (!atkconfig("use_duration", true))
    {
      $this->add(new atkTimeAttribute("starttime","0","23",array(0,15,30,45),"12", AF_OBLIGATORY));
      $this->add(new to_time());
      $durationflags|=AF_HIDE_ADD|AF_HIDE_EDIT;
    }
    else
    {
      $durationflags|=AF_OBLIGATORY;
    }

    $this->add(new atkDurationAttribute("time",atkconfig("timereg_resolution", "15m"), atkconfig("max_bookable",10).'h',$durationflags));
  }

  /**
   * Determine the number of available activities
   */
  function getActivityCount()
  {
    $db = &atkGetDb();
    $result = $db->getrows("SELECT count(*) as activitycount FROM activity");
    return $result[0]['activitycount'];
  }

  /**
   * Adds the projectphase attribute and, if there are more than 1 activities, the activity attribute
   */
  function addPhaseAndActivity()
  {
    // Determine wheter the activity attribute should be displayed or not.
    $displayactivityattribute = $this->getActivityCount() != 1;

    $this->add(new atkProjectPhaseAttribute("phaseid",AF_OBLIGATORY|AF_FORCE_LOAD|($displayactivityattribute ? AF_JS_UPDATEACTIVITY : 0)));
    $this->add(new atkListAttribute("activityid",array("dummy","dummy","dummy","dummy","dummy"),array(0,0,0,0,0),AF_OBLIGATORY|AF_NO_TRANSLATION|($displayactivityattribute ? 0 : AF_HIDE), 10));
  }

  function setLock()
  {
    $this->m_lock = $this->getLockType(strtolower($this->m_user),$this->m_viewdate);

    if ($this->m_lock != "")
    {
      // This week is locked..
      $this->addFlag(NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
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
    // Setup the initial values array
    $values = array("userid"=>array("id"=>$this->m_user),
                    "entrydate"=>Array("year"=>date("Y"),
                    "month"=>date("m"),
                    "day"=>date("d")),
                    "activitydate"=>$this->getInitialDate(),
                    "todate"=>$this->getInitialDate()
                   );

    // Set an initial activityid if there's exactly one activity
    if ($this->getActivityCount() == 1) {
      $db = &atkGetDb();
      $result = $db->getrows("SELECT id FROM activity");
      $values["activityid"] = $result[0]["id"];
    }

    // Return the initial values
    return $values;
  }

  function activityid_display($rec)
  {
    // activityname is loaded in the phaseid join, so we get the info from there..
    return $rec["phaseid"]["activityname"];
  }

  function action_edit(&$handler)
  {
    global $g_user;
    $recs = $this->selectDb($this->m_postvars["atkselector"], "hours.id", "", "", array("userid", "activitydate"));

    $this->m_lock = $this->getLockType($recs[0]["userid"]["id"],
                      sprintf("%02d-%02d-%02d", $recs[0]["activitydate"]["year"],
                                                $recs[0]["activitydate"]["month"],
                                                $recs[0]["activitydate"]["day"]));
    if ($this->m_lock=="")
    {
      if (!$this->allowed("any_user"))
      {
        $this->addFilter("hours.userid",strtolower($g_user["id"]));
      }
      return $handler->action_edit();
    }
    else
    {
      // Week is locked. We may not edit, only view.
      $handler = $this->getHandler("view");
      $handler->invoke("action_view");
    }
  }

  // If there is a $config_hours_confirm_save then Achievo will display it
  // in a confirm box on the first timeregistration for the project
  function action_save(&$handler)
  {
    if (atkconfig("hours_confirm_save") && ($this->_isThisTheFirstTimeRegForThisProject() || !empty($this->m_postvars['confirm'])))
    {
      global $g_sessionManager;
      if (!empty($this->m_postvars['confirm'])) // was 'yes' clicked?
      {
        $this->m_postvars = $g_sessionManager->getValue("postvars");
        $this->m_postvars = $this->m_postvars["postvars"];
        return $handler->action_save();
      }
      elseif (empty($this->m_postvars['cancel'])) // nothing clicked yet?
      {
        $g_sessionManager->globalVar("postvars", $this->m_postvars);

        // the 1=0 is a trick. without it, the confirmation page would try
        // to retrieve a record.
        $this->confirmAction("1=0", "save", false, TRUE, $_REQUEST);
      }
      else // was 'no' clicked?
      {
        $this->redirect(); // go back to where we came from
      }
    }
    else return $handler->action_save(); // normal behaviour
  }

  function confirmSaveText()
  {
    return atkconfig("hours_confirm_save");
  }

  function javascript_edit($rec)
  {
    global $currentphase, $currentactivity;

    if (!isset($currentphase)||$currentphase=="") $currentphase=0;
    if (!isset($currentactivity)||$currentactivity=="") $currentactivity =0;
    $result = '<script language="javascript"> set_activities('.$currentphase.','.$currentactivity.'); </script>';

    return $result;
  }

  /**
   * Updates the activitydate in atkformdata to the value of m_viewdate
   *
   * Function needed to override standard behaviour of atk when using initial_values.
   * Normally initial values are only used if the form value is NOT YET present in
   * the m_postvars["atkformdata"]. Since atk stores attributes in atkformdata when
   * performing an action (o.e. select a project and phase for registering hours)
   * the initial_values are ignored.
   *
   * We override this behaviour by manually overriding the atkformdata value of activitydate.
   *
   * @param nothing
   * @return nothing
   */
  function updatesessionrecordviewdate()
  {
    //we override the atkformadata value of activity date to make sure it keeps using our own m_viewdate
    // attribute.
    if (isset($this->m_postvars["atkformdata"]["activitydate"]))
      $this->m_postvars["atkformdata"]["activitydate"] = $this->getInitialDate();
  }

  // override the admin action, because we have the weekview/dayview switch..
  function action_admin(&$handler)
  {
    $page = &$this->getPage();

    global $g_user;

    $this->setLock();

    $this->addFilter("hours.userid",$this->m_user);

    //we need to make sure the activitydate has the same value as $this->m_viewdate.
    $this->updatesessionrecordviewdate();

    if ($this->m_weekview!=1)
    {
      if ($this->m_lock=="") $res[] = $handler->invoke("addPage");
      $this->addFilter("activitydate",$this->m_viewdate);
      $res[] = $handler->invoke("adminPage");
    }
    else
    {
      if (atkconfig("timereg_week_bookable")==true && $this->m_lock=="") $res[] = $handler->invoke("addPage");
      $res[] = $this->weekview();
    }

    $page->addContent($this->renderActionPage("admin", $res));
  }

  function adminHeader()
  {
    $res='';
    if ($this->m_weekview!=1)
    {
      $viewdatestamp = adodb_mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));

      // we substract 3601 seconds to account for Daylight Savings Time
      $yesterday = date('Y-m-d',$viewdatestamp-(86400-3601));
      // we add 3601 seconds to account for Daylight Savings Time
      $tomorrow = date('Y-m-d',$viewdatestamp+86400+3601);
      $today = date('Y-m-d');

      $ui = &$this->getUi();
      $tplvars = array( 'yesterdayurl'=>session_url('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$yesterday),
                        'weekviewurl'=>session_url('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$this->m_viewdate.'&weekview=1'),
                        'sessionform'=>session_form(),
                        'userselect'=>$this->getUserSelect(),
                        'datejumper'=>$this->getDateJumper(),
                        'currentdate'=>atktext(strtolower(date("l", $viewdatestamp)))." ".atktext(strtolower(date("F", $viewdatestamp)))." ".date("d, Y", $viewdatestamp),
                        'lockicon'=>$this->getLockIcon(),
                       );

      if ($this->m_viewdate<date('Y-m-d'))
      {
        // If current is not today we can also move forward
        $tplvars['tomorrowurl'] = session_url('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$tomorrow);
        $tplvars['todayurl'] = session_url('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$today);
      }
      $res = $ui->render($ui->templatePath('hours_adminheader.tpl'),$tplvars);
    }
    return $res;
  }

  function adminFooter()
  {
    if ($this->m_weekview!=1)
    {
      $balancenode = &atkGetNode("timereg.overtime_balance");
      $balance = $balancenode->getBalance(date("Y-m-d", adodb_mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4))), $this->m_user);

      $ui = &$this->getUi();
      $tplvars = array( 'scheduleurl'=>session_url('dispatch.php?atknodetype=calendar.schedule&atkaction=admin&viewdate=',SESSION_NESTED),
                        'balance'=>time_format((60*$balance["balance"]),true),
                       );
      return $ui->render($ui->templatePath('hours_adminfooter.tpl'),$tplvars);
    }
    else
    {
      return "";
    }
  }

  function getDateJumper()
  {
    $dummy_rec = array("viewdate"=>$this->getInitialDate());
    $datebox = new atkDateAttribute("viewdate","","",0, date("Ymd"),AF_OBLIGATORY);
    return $datebox->edit($dummy_rec);
  }

  function getUserSelect()
  {
    if ($this->allowed("any_user"))
    {
      $db = &atkGetDb();
      $users = $db->getRows("SELECT id, lastname, firstname FROM person WHERE role = 'employee' AND status = 'active' ORDER BY lastname");
      $res = '<select name="viewuser">';
      for ($i=0;$i<count($users);$i++)
      {
        $res .= '<option value="'.$users[$i]["id"].'" '.($this->m_user == $users[$i]["id"] ? 'selected' : '').'>'.$users[$i]["lastname"].', '.$users[$i]["firstname"];
      }
      $res .= '</select>';
    }
    return $res;
  }

  function weekview($filterbycoordinator=false)
  {
    if ($this->m_viewdate!="")
    {
      $viewtime = mktime(12,12,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));
    }
    else
    {
      $this->m_viewdate = date("Y-m-d");
      $viewtime = time();
    }
    $this->addStyle("style.css");

    return $this->getWeekviewData($viewtime, $filterbycoordinator);

  }

  function getWeekviewData($viewtime, $filterbycoordinator=false)
  {
    $userid = $this->m_user;
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
      $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $weekdates[$i]['day']=atktext(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
      $weekdates[$i]['stamp']=$viewtime-(86400*($weekday-$i));
    }

    $week = Array();
    $projtotals = Array();
    $daytotals = Array();

    $where = "activitydate >= '".$weekdates[1]['date']."' AND activitydate <= '".$weekdates[7]['date']."' AND hours.userid = '$userid'";
    if ($filterbycoordinator)
    {
      $coordinator = getUser();
      $where.= " AND B.coordinator = '".$coordinator["id"]."'";
    }
    $data = $this->selectDb($where,"","",$this->m_listExcludes);

    for ($i=0;$i<count($data);$i++)
    {
      $rec = $data[$i];
      $key = $rec["phaseid"]["projname"]."-".$rec["phaseid"]["phasename"];
      $date = $rec["activitydate"]["year"]."-".$rec["activitydate"]["month"]."-".$rec["activitydate"]["day"];
      $time = $rec["time"];
      $week[$key][$date] += $time;
      $projtotals[$key] += $time;
      $daytotals[$date] += $time;
      $total += $time;
    }

    $output = $this->weekviewHeader($weekdates, $viewtime);

    $weekdata = array();

    $table = &atknew("atk.utils.atktablerenderer");

    $weekdata[0][] = atktext("project")."/".atktext("phase");
    for ($i=1;$i<=7;$i++)
    {
      if ($weekdates[$i]['date'] <= date("Y-m-d"))
      {
        $url = 'dispatch.php?viewdate='.$weekdates[$i]['date']."&weekview=0";
        $weekdata[0][] = href($url,$weekdates[$i]['day']).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')';
      }
      else
      {
        $weekdata[0][] = $weekdates[$i]['day'].'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')';
      }
      $table->setColAlignment($i, TBL_CENTER);

    }
    $weekdata[0][] = atktext('total');

    $row = 1;
    while (list($proj, $times)=each($week))
    {
      $weekdata[$row][] = $proj;
      for ($i=1;$i<=7;$i++)
      {
        $weekdata[$row][] = time_format($times[$weekdates[$i]['date']]);
      }
      $weekdata[$row][] = time_format($projtotals[$proj]);
      $row++;
    }

    $weekdata[$row][] = atktext('total');

    for ($i=1;$i<=7;$i++)
    {
      if ($daytotals[$weekdates[$i]['date']]>atkconfig("overtimethreshold"))
      {
        $color = "#FF0000";
      }
      else
      {
        $color = "#000000";
      }
      $weekdata[$row][] = '<font color="'.$color.'">'.time_format($daytotals[$weekdates[$i]['date']]).'</font>';
    }
    $weekdata[$row][] = time_format($total);

    $output.=$table->render($weekdata, TBL_FOOTER|TBL_DATA, "recordlist");
    $output.= $this->weekviewFooter($weekdates, $viewtime);
    $ui = &atknew("atk.ui.atkui");
    return $ui->renderBox(array("title"=>$this->weekviewTitle($userid, $viewtime),
                                    "content"=>$output));

  }

  function weekviewTitle($userid, $viewtime)
  {
    return atktext("title_houradmin_weekview")." ".strftime("%V, %Y",$viewtime);
  }

  function weekviewHeader($weekdates, $viewtime)
  {
    global $g_user;
    $userid = $this->m_user;

    $ui = &$this->getUi();
    $tplvars = array( 'session_form'=>session_form(),
                      'prevweekurl'=>session_url('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date']),
                      'thisweekurl'=>session_url('dispatch.php?weekview=1&viewdate='.date("Y-m-d")),
                      'dayviewurl'=>session_url('dispatch.php?weekview=0&viewdate='.date("Y-m-d")),
                      'datejumper'=>$this->getDateJumper(),
                      'curuser'=>$g_user,
                      'curdate'=>adodb_date('W, Y',$viewtime),
                      'lockicon'=>$this->getLockIcon(),
                     );
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $tplvars['nextweekurl']=session_url('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date']);
    }
    if ($this->m_lock=="individual" && $this->allowed("unlock"))
    {
      $tplvars['unlockurl']=session_url('dispatch.php?atkaction=unlock&userid='.$userid.'&viewdate='.$this->m_viewdate);
    }
    // users may only lock weeks that are not already locked. they must have the proper permission
    // and, if configured, they may only lock a week once they have booked all their hours.
    else if ($this->m_lock=="" && $this->allowed("lock") && (atkconfig("timereg_incompleteweeklock") || $this->weekComplete($userid, $this->m_viewdate)))
    {
      $tplvars['lockurl']=session_url('dispatch.php?atkaction=lock&userid='.$userid.'&viewdate='.$this->m_viewdate);
    }
    return $ui->render($ui->templatePath('hours_adminheader_weekview.tpl'),$tplvars);
  }

  /**
   * Can be overriden to add a weekviewdata footer
   *
   * @return String HTML Footer to be embedded in the weekviewdata box
   */
  function weekviewFooter($weekdates, $viewtime)
  {
  }

  function postAdd($rec)
  {
    // if we add a record on a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_viewdate=$rec["activitydate"]["year"]."-".sprintf("%02d",$rec["activitydate"]["month"])."-".sprintf("%02d",$rec["activitydate"]["day"]);

    // update overtime balance
    $this->calculateNewBalance($rec, "add");
    return true;
  }

  function postUpdate($rec)
  {
    // if we edit a record and set it to a date that is not the currently viewed date, we jump
    // to that date.
    // TODO this doesn't work anymore due to the fact that we move down in the session stack
    // where viewdate still has it's old value (do we need a way to alter the previous session?)
    $this->m_viewdate=$rec["activitydate"]["year"]."-".sprintf("%02d",$rec["activitydate"]["month"])."-".sprintf("%02d",$rec["activitydate"]["day"]);

    // update overtime balance
    $this->calculateNewBalance($rec, "edit");
    return true;
  }

  function postDel($rec)
  {
    // update overtime balance
    $this->calculateNewBalance($rec, "delete");
    return true;
  }

  function calculateNewBalance($rec, $mode)
  {
    $balancenode = &atkGetNode("timereg.overtime_balance");
    $userid = $rec["userid"]["id"];
    $year  = $rec["activitydate"]["year"];
    $month = $rec["activitydate"]["month"];
    $activityday = $rec["activitydate"]["day"];
    $day = date("Y-m-d", adodb_mktime(0,0,0,$month,$activityday,$year));
    $yesterday = date("Y-m-d", adodb_mktime(0,0,0,$month,$activityday-1,$year));
    $balance = $balancenode->getBalance($day, $userid);

    // only update balance if balance was not set manually
    if ($balance["manual"] == 0)
    {
      if ($mode == "add")
      {
        // check if a record already exists for this day
        if ($balance["balance"] == atktext("not_inserted"))
        {
          // insert a new record with the balance of yesterday or 0
          $newbalance = $balancenode->getBalance($yesterday, $userid);
          if ($newbalance["balance"] == atktext("not_inserted")) $newbalance["balance"] = 0;
          $balancenode->addOvertimeBalance($userid, $day, $newbalance["balance"]);
        }
      }

      $percentage = $balancenode->getPercentage($rec["workperiod"]["id"]);
      $hours = ($rec["time"]/60);

      if ($mode == "add") $newbalance = ($balance["balance"] + (($percentage/100) * $hours));
      elseif ($mode == "edit")
      {
        $oldpercentage = $balancenode->getPercentage($rec["atkorgrec"]["workperiod"]["id"]);
        $oldhours = $rec["atkorgrec"]["time"]/60;
        $newbalance = (($balance["balance"] - (($oldpercentage/100) * $oldhours)) + (($percentage/100) * $hours));
      }
      elseif($mode == "delete") $newbalance = ($balance["balance"] - (($percentage/100) * $hours));

      $balancenode->saveOvertimeBalance($rec["userid"]["id"], $day, $newbalance);
    }
    $manual = $balancenode->getLatestManualCorrection($rec["userid"]["id"], $yesterday, $day);
    if ($manual != $yesterday)
    {
      $manualdate = date("Y-m-d", adodb_mktime(0,0,0,$manual["day"]["month"],$manual["day"]["day"],$manual["day"]["year"]));
    }
    else $manualdate = $yesterday;

    if ($manualdate < $day) 
    {      
      $balancenode->deleteBalance($rec["userid"]["id"], $day);
    }    
  }

  // Check if this week may be booked..
  // returns: "" - not locked
  //          "individual" - locked for the user only
  //          "all"       - locked for everybody (can only be unlocked by administrator)
  function getLockType($userid, $date)
  {
    static $s_locks = array();

    $week = weekstamp(adodb_mktime(0,0,0,substr($date,5,2),
                                     substr($date,8,2),
                                     substr($date,0,4)));

    if ($s_locks[$userid][$week]=="")
    {
      $query = "SELECT
                  week, userid
                FROM
                  hours_lock
                WHERE
                  (userid = '$userid' OR userid IS NULL)
                AND
                  week = '$week'";
      $db = &atkGetDb();
      $res = $db->getrows($query);

      if (count($res))
      {
        $all = false;
        for ($i=0;$i<count($res);$i++)
        {
          if (empty($res[$i]["userid"]))
          {
            $s_locks[$userid][$week] = "all";
            $all = true;
          }
        }

        // apparently not locked for everybody.
        if (!$all)
        {
          $s_locks[$userid][$week] = "individual";
        }
      }
      else
      {
        $s_locks[$userid][$week]="none";
      }
    }

    return ($s_locks[$userid][$week]=="none"?"":$s_locks[$userid][$week]);
  }

  function action_lock(&$handler)
  {
    if (atkconfig("lock_week_approval_required"))
      $this->lockWeek();
    else
      $this->lockWeek("1");
  }

  function lockWeek($approved=null)
  {
    $viewdate = $this->m_postvars["viewdate"];
    $record = array();
    $record["userid"]["id"] = $this->m_postvars["userid"];
    $record["week"] = weekstamp(adodb_mktime(0,0,0,substr($viewdate,5,2),
                                   substr($viewdate,8,2),
                                   substr($viewdate,0,4)));
    $record["approved"] = $approved;
    $locknode = &atkGetNode("timereg.hours_lock");
    $locknode->addDb($record);
    $this->redirect();
  }

  function action_unlock(&$handler)
  {
    $db = &atkGetDb();

    $viewdate = $this->m_postvars["viewdate"];
    $userid = $this->m_postvars["userid"];
    $week = weekstamp(adodb_mktime(0,0,0,substr($viewdate,5,2),
                                   substr($viewdate,8,2),
                                   substr($viewdate,0,4)));
    $query = "DELETE FROM hours_lock WHERE week = '$week' AND userid = '$userid'";
    $db->query($query);
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
    $db = &atkGetDb();

    $start = startOfWeek($date);
    $end = endOfWeek($date);

    // first get the total of contract hours
    $sql = "SELECT sum(uc_hours*60) as contract FROM usercontract WHERE startdate <= '$start'
                AND (enddate > '$start' OR enddate IS NULL) AND userid = '$userid'";
    $result = $db->getRows($sql);

    // get the total of time already registered this week
    $query = "SELECT sum(time) as totaal
              FROM
                hours
              WHERE
                hours.userid = '$userid'
                AND activitydate between '$start' and '$end'";
    $res = $db->getrows($query);
    if (count($res)==0 || count($result)==0)
    {
      // either no contract in this period, or no hours booked at all..
      return false;
    }
    else
    {
      return ($result[0]["contract"] - $res[0]["totaal"]<=0);
    }
  }

  function activitydate_validate(&$rec)
  {
    // when inserting multiple days at once, we don't break on invalid days
    // (we will just skip them later on).
    if ($rec["activitydate"]["multi"]!=1)
    {
      if ($this->getLockType($this->m_user,sprintf("%04d-%02d-%02d",
                                                   $rec["activitydate"]["year"],
                                                   $rec["activitydate"]["month"],
                                                   $rec["activitydate"]["day"]))!="")
      {
        triggerError($rec, "activitydate", "weeklocked");
      }

      // Here we check for non-valid days, this should be no problem for the normal hours node
      // As it is (near) impossible to insert false dates without some serious hacking
      // However the PDA version doensn't have the protection of DHTML, so it IS relevant there
      if (!checkdate($rec['activitydate']['month'],$rec['activitydate']['day'],$rec['activitydate']['year']))
        triggerError($rec, "activitydate", "date_invalid");
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
    // If no activities available, trigger an error:
    if ($this->getActivityCount() == 0) {
      triggerError($record, "activityid", "Please add one or more activities first");
    }
    else {
      $obj = &atkGetNode("project.activity");
      $act = $obj->selectDb("id='".$record["activityid"]."'");
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
    }

    // when inserting multiple days at once, we don't break on invalid days
    // (we will just skip them later on).
    if ($record["activitydate"]["multi"]!=1)
    {
      $activitydate = sprintf("%04d-%02d-%02d", $record["activitydate"]["year"], $record["activitydate"]["month"], $record["activitydate"]["day"]);
      $new_minutes = $record["time"];

      if ($this->exceed_hrs_in_day($record["userid"]["id"], $activitydate, $new_minutes, $record["id"]))
      {
        triggerError($record, "time", "error_hoursindayexceeded");
      }
    }
    else // when inserting multiple days at once, we must verify if end is not smaller than start.
    {
      $start = date("Y-m-d", adodb_mktime(12,0,0,$record["activitydate"]["month"],$record["activitydate"]["day"],$record["activitydate"]["year"]));
      $todate = $this->m_postvars["todate"]; // todate is not part of $record, because the todate attrib isn't really a part of the node.
      $end = date("Y-m-d", adodb_mktime(12,0,0,$todate["month"],$todate["day"],$todate["year"]));
      atkdebug("validating $start vs $end");
      if ($start>$end)
      {
        triggerError($record, "activitydate", "error_startbiggerthanend");
      }
    }
    // continue with original validation
    atkNode::validate($record, $mode);
  }

  /**
   * This method is called by the hoursurvey to get custom search fields.
   * Derived classes (for example in overloaders) can override this method and
   * Add their own custom search fields.
   */
  function getSelectionItems()
  {
    return array();
  }

  function getHourSurveyColumns()
  {
    $defaultcolumns = array("activitydate","userid","phaseid","activityid","remark","time","functionlevel", "creditor_id");
    if (moduleExists("billing")) $defaultcolumns[] = "billing_projecttypeid";
    $additionalcolumns = atkHarvestModules("getHourSurveyColumns");
    return array_merge($defaultcolumns, $additionalcolumns);
  }

  function exceed_hrs_in_day($userid, $date, $new_minutes, $recordid="")
  {
    $db = &atkGetDb();

    $query = "SELECT (sum(time) + $new_minutes) as minutes_in_day
              FROM hours
              WHERE
                userid = '$userid'
                AND activitydate = '$date'";

    // If we are editing a record, than the old value of the current record
    // must not be counted, but the new value (which was already added as
    // $new_minutes).
    if ($recordid!="") $query.= " AND id <> $recordid";
    $res = $db->getrows($query);
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

  function addPage(&$handler, $record="")
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

    return $handler->addPage($record);
  }

  function activitydate_edit($record, $prefix="", $mode="")
  {
    if ($mode=="add")
    {
      // We support 2 modes... one for single day entry, one for multiday entry.
      if ($this->m_insertmode=="multi")
      {
        $res = $this->m_attribList["activitydate"]->edit($record, $prefix, $mode);
        $toDate = new atkDateAttribute("todate", "","", 0, date("Ymd"), AF_OBLIGATORY);

        $res.= " ".atktext("uptoandincluding")." ".$toDate->edit($record, $prefix, $mode);
        $res.= '<input type="hidden" name="'.$prefix.'activitydate[multi]" value="1">';
        $newmode = "day";
      }
      else
      {
        $res = $this->m_attribList["activitydate"]->edit($record, $prefix, $mode);
        $newmode = "multi";
      }
      $res.= " ".href("dispatch.php?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&insertmode=".$newmode,
                      atktext("insert_".$newmode, $this->m_module, $this->m_type),
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
    $res= "<BR>".atktext("filterdays", $this->m_module, $this->m_type)."<BR>";
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
      $res.= atktext($days[$i]).': <input type="checkbox" name="'.$prefix.'dayfilter['.$i.']" value="1" '.$checked.'> &nbsp; ';
    }
    return $res;
  }

  function addDb(&$record, $exectrigger=true)
  {
    global $g_user;

    $res = true;

    if ($record["activitydate"]["multi"]==1)
    {
      // We're registering multiple days at once.
      $walkerStamp = adodb_mktime(12,0,0,$record["activitydate"]["month"],$record["activitydate"]["day"],$record["activitydate"]["year"]);
      $walker = date("Y-m-d", $walkerStamp);

      $todate = $this->m_postvars["todate"]; // todate is not part of $record, because the todate attrib isn't really a part of the node.

      $end = date("Y-m-d", adodb_mktime(12,0,0,$todate["month"],$todate["day"],$todate["year"]));

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
        if (!in_array($dow, $doDays) || $this->getLockType($this->m_user,$walker)!="")
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
          $res = parent::addDb($newRecord);
        }

        // End of day, add 1 day and reloop.
        $walkerStamp = adodb_mktime(12,0,0,$record["activitydate"]["month"],
                                     $record["activitydate"]["day"]+$i,
                                     $record["activitydate"]["year"]);
        $walker = date("Y-m-d", $walkerStamp);

      }
    }
    else
    {
      $res = parent::addDb($record, $exectrigger);
    }
    return $res;
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
    // Determine logged in user
    $userid = atkArrayNvl(getUser(), "id");

    // Determine locktype for given record
    $locktype = $this->getLockType($rec["userid"]["id"], sprintf("%02d-%02d-%02d", $rec["activitydate"]["year"], $rec["activitydate"]["month"], $rec["activitydate"]["day"]));

    // If the record doesn't belong to me or is locked
    if (empty($userid) || ($rec["userid"]["id"] != $userid) || ($locktype != ""))
    {

      // If not allowed to edit someone elses records, unset the actions
      if (!$this->allowed("any_user"))
      {
        unset($actions["edit"]);
        unset($actions["delete"]);
        $idx = array_search("delete", $mraactions);
        if ($idx!==false)
        {
          unset($mraactions[$idx]);
        }
      }
      else
      {
        $actionbase = dispatch_url($this->atknodetype(), "[atkaction]", array("atkselector" => "[pk]", "viewuser" => $this->m_user));
        $actions = array();
        $actions["edit"] = str_replace("[atkaction]", "edit", $actionbase);
        $actions["delete"] = str_replace("[atkaction]", "delete", $actionbase);
        $mraactions["mra_move"] = "mra_move";
      }
    }
    else
    {
      $mraactions["mra_move"] = "mra_move";
    }
  }

  function getLockIcon()
  {
    if ($this->m_lock!="")
    {
      // Week is locked
      return ' <img src="'.atkconfig("atkroot").'atk/images/lock.gif" border="0">';
    }
  }

  /**
   * Checks wether or not there are any timeregistrations on a particulair project
   * Depends on being called from action_save
   * @return bool Wether or not it's the first timereg for the project
   */
  function _isThisTheFirstTimeRegForThisProject()
  {
    $phasenode =   &atkGetNode("project.phase");
    $projectnode = &atkGetNode("project.project");
    $hoursnode =   &atkGetNode("timereg.hours");
    $record = $this->updateRecord();
    $user = getUser();

    $excludelist = $this->_getExcludeListForNode($phasenode, array("projectid"));
    $phaserecs = $phasenode->selectDb($phasenode->m_table.".id='{$record['phaseid']['phaseid']}'",null,null, $excludelist);

    $excludelist = $this->_getExcludeListForNode($phasenode, array("id", "projectid"));

    $phases = $phasenode->selectDb($phasenode->m_table.".projectid='{$phaserecs[0]['projectid']['id']}'",null,null,$excludelist);

    $inphase = array();
    foreach ($phases as $phase)
    {
      $inphase[] = $phase["id"];
    }

    $in = implode(",",$inphase);
    if ($in) $instr = "IN (".$in.")";
    $timeregcount = $hoursnode->countDb("{$hoursnode->m_table}.phaseid $instr AND {$hoursnode->m_table}.userid = '{$user['id']}'");
    atkdebug("$timeregcount timeregistrations on this project by this user");
    if ($timeregcount>0)
      return false;
    else
        return true;
  }

  function _getExcludeListForNode($node, $include)
  {
    $excludelist = array_keys($node->m_attribList);
    foreach ($excludelist as $key=>$value)
    {
      if (in_array($value, $include)) unset($excludelist[$key]);
    }
    sort($excludelist);
    return $excludelist;
  }

  function preAdd(&$record)
  {
    $this->modifyfortime($record);
    return true;
  }

  function preUpdate(&$record)
  {
    $this->modifyfortime($record);
  }

  function modifyfortime(&$record)
  {
    // if we don't have duration, then we have a begin and an end time
    // however, we still store it as a duration, only with a start time
    // so we need to remove the end time and add a 'time'
    if ($record['starttime'])
    {
      $this->performTimeChecks($record);
      convertHoursToDuration($record);
    }
  }

  function edit_values($defaults)
  {
    // Here we make sure that if we have a starttime (don't worry,
    // the convertDurationToHours() function checks for it) that
    // we convert the duration to a starttime and entime notation
    // for editting
    convertDurationToHours($defaults);
    return array('endtime'=>$defaults['endtime']);
  }

  /**
  * Does checks on a record with a starttime and endtime in it
  * before it is converted and goes into the database
  */
  function performTimeChecks(&$record)
  {
    $RECstartH = $record["starttime"]["hours"]*1;
    $RECstartM = $record["starttime"]["minutes"]*1;
    $RECstartS = $record["starttime"]["seconds"]*1;

    $RECendH = $record["endtime"]["hours"]*1;
    $RECendM = $record["endtime"]["minutes"]*1;
    $RECendS = $record["endtime"]["seconds"]*1;

    // Check if the endtime isn't before the starttime
    if ($RECstartH>$RECendH ||
         ($RECstartH==$RECendH && $RECstartM>$RECendM) || ($RECstartH==$RECendH && $RECstartM==$RECendM && $RECstartS>$RECendS))
    {
      triggerError($record, "starttime", "error_begintime_before_endtime");
      return;
    }

    if (($RECstartH==$RECendH)&&($RECstartM==$RECendM)&&($RECstartS==$RECendS))
    {
      triggerError($record, "starttime", "error_begintime_equals_endtime");
      return;
    }

    // Check if that period is still available

    // WARNING!
    // Here we do a selectDb on ourselves for the current activitydate,
    // normally this would fail, however, because this function is only called in
    // a special mode and never directly from the constructor, it should always work.
    $select = $this->selectDb("activitydate = '".$record['activitydate']['year']."-".$record['activitydate']["month"]."-".$record['activitydate']["day"]."'");

    foreach ($select as $dbrecord)
    {
      convertDurationToHours($dbrecord,true);

      $DBstartH = $dbrecord["starttime"]["hours"];
      $DBstartM = $dbrecord["starttime"]["minutes"];
      $DBstartS = $dbrecord["starttime"]["seconds"];

      $DBendH = $dbrecord["endtime"]["hours"];
      $DBendM = $dbrecord["endtime"]["minutes"];
      $DBendS = $dbrecord["endtime"]["seconds"];

      if (
      // Check if endtime of this record is earlier or the same as the starttime of this entry
      !($DBstartH>$RECendH || ($DBstartH==$RECendH && $DBstartM>$RECendM) ||
          ($DBstartH==$RECendH && $DBstartM>$RECendM ||
            ($DBstartM==$RECendM && $DBstartS>=$RECendS))
      ||
      // Check if starttime of the record is later or the same as the endtime of this entry
      ($RECstartH>$DBendH || ($RECstartH==$DBendH && $RECstartM>$DBendM) ||
          ($RECstartH==$DBendH && $RECstartM<$DBendM ||
            ($DBstartM== $RECendM && $RECstartS>=$DBendS)))))
      {
        triggerError($record, "starttime", "error_time_already_registered");
        return false;
      }
    }
  }
    // END WARNING

}

?>
