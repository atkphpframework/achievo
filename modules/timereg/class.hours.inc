<?php

include_once("achievotools.inc");
useattrib("project.projectmanytoonerelation");
useattrib("project.projectattrib");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atktextattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");
useattrib("atktimeattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");
atkimport("module.timereg.locking");
atkimport("module.timereg.timeregutils");
atkimport("module.project.utils.projectutils");

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

/**
 * @todo Probably no longer used
 */
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
  var $m_user = "";
  var $m_insertmode = ""; // can be set to 'day' or 'multi'
  var $m_lockmode = "";
  var $m_weekview = "";

  function hours($type="hours", $flags=0)
  {
    // weekview and viewdate are variables that we need to remember..
    $this->m_weekview = $this->processGlobalVar("weekview");
    $this->m_viewdate = $this->processGlobalVar("viewdate");
    $this->m_user = $this->processGlobalVar("viewuser");
    $this->m_lockmode = atkConfig::get("timereg", "lockmode", "week");

    if (is_array($this->m_viewdate)) // if set by the date jumper, viewdate is an array.
    {
      $this->m_viewdate = sprintf("%04d-%02d-%02d",
                                  $this->m_viewdate["year"],
                                  $this->m_viewdate["month"],
                                  $this->m_viewdate["day"]);
    }

    //If the datejumper was not set, try the initial date.
    if ($this->m_viewdate=="")
    {
      $this->m_viewdate = $this->getInitialDate();
      $this->m_viewdate = sprintf("%04d-%02d-%02d", $this->m_viewdate["year"], $this->m_viewdate["month"],$this->m_viewdate["day"]);
    }

    if ($this->m_viewdate=="") $this->m_viewdate = strftime("%Y-%m-%d");

    if($this->m_user=="") $this->m_user=$this->getInitialUser();

    $flags|=NF_NO_VIEW|NF_TRACK_CHANGES|NF_NO_IMPORT;

    $this->atkNode($type,$flags|NF_MRA|NF_NO_ADD); // node() constructor is *not* called automatically!

    // Default view if none is set
    if (!isset($this->m_weekview)||$this->m_weekview=="")
    {
      if (atkconfig("timereg_defaultview")=="week") $this->m_weekview=1;
    }

    if (isset($this->m_weekview)&&$this->m_weekview!="") {
      /* When using week view, we need to get all days for this week and check
         locks on each individually.

         If m_viewdate happens to be locked, there might still be other,
         non-locked days. If there is, we find the first non-locked day
         of the week, and set m_viewdate to this.

         This way we know that if the day contained in m_viewdate is locked,
         the entire week is definately locked. If it isn't, we need to perform
         more fine grained control, which is implemented where needed in the
         code.
      */

      $weekdates = array_slice(TimeregUtils::getWeekDates($this->m_viewdate), 1, 7);

      $weekdatesViewDateIndex = "";

      // Check if the current viewdate is locked, and keep array index if it is.
      foreach ($weekdates as $key => $date)
      {
        if (!empty($date['lock']) && $date['date'] == $this->m_viewdate)
        {
          $weekdatesViewDateIndex = $key;
          break;
        }
      }

      // If there are any non-locked days for this perod, we need to set this as
      // m_viewdate, to allow user to add hours at all.
      if ($weekdatesViewDateIndex) {
        $this->m_viewdate = Locking::getFirstNonLockedDayOfPeriod(
                                     $weekdates, $weekdatesViewDateIndex);
      }

    } // end if


    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));

    $this->add(new atkDateAttribute("activitydate","","", 0, (atkconfig("timereg_allowfuture", false)?0:date("Ymd")), AF_OBLIGATORY|AF_FORCE_LOAD|($this->m_weekview?0:AF_HIDE_LIST)),null,10);
    $this->add(new to_date());

    // force_load flag is set for userid, since several features rely on the
    // userid, even if it's not displayed on screen.
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY));

    //phaseid must be added before the project id to make sure the join works
    //we need the projectid to sort on
    $phase = &new atkManyToOneRelation("phaseid","project.phase",AF_OBLIGATORY|AF_FORCE_LOAD);
    $phase->setDestinationFilter("phase.projectid='[projectid.id]'");
    $oneActivity = (projectutils::getActivityCount() == 1);
    if(!$oneActivity)
      $phase->addDependee("activityid");
    $this->add($phase,null, 30);

    $project = &new projectManyToOneRelation("projectid","project.projectselector",AF_OBLIGATORY|AF_FORCE_LOAD);
    $project->addDependee("phaseid");
    $this->add($project,null,20);

    if($oneActivity)
      $activity_flag = AF_HIDE|AF_FORCE_LOAD;
    else
      $activity_flag = AF_LIST_OBLIGATORY_NULL_ITEM;
    $activity = &new atkManyToOneRelation("activityid","project.activity",AF_OBLIGATORY|$activity_flag);
    $this->add($activity);


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
/**
 * @todo: we need to order by projectid.abbreviation, ATK does not include the
 * projectid and phaseid in the selectDb so we FORCE_LOAD the projectid
 * and phaseid. This should actually be fixed in atk somehow.
 */
    $this->setOrder("projectid.abbreviation");

    $this->setTable("hours","hours");
    $this->addSecurityMap("mra_move", "edit");
  }

  function action_mra_move(&$handler)
  {
    $selector = atkArrayNvl($this->m_postvars, "atkselector", array());
    $this->redirect(session_url(dispatch_url("timereg.movehours", "add", array("atkselector"=>$selector)), SESSION_REPLACE));
  }

  function activityid_selection($record, $mode="")
  {
    $filter = "";
    $attr = &$this->getAttribute('activityid');

    if ($mode == 'add' || $mode == 'edit' || $mode == 'select')
    {
      $activityids = projectutils::getAvailableActivityIds($record["phaseid"]["id"]);
      if(count($activityids)>0)
        $filter = "activity.id IN (".implode(",",$activityids).")";
      else
        $filter = "activity.id IN (NULL)";

      $attr->addDestinationFilter($filter);
    }
    return $attr->getSelectableRecords($record, $mode);
  }

  function processGlobalVar($name, $value="")
  {
    $sessionManager = &atkGetSessionManager();
    $session = &$sessionManager->getSession();
    $sessionvarid = "timereg_hours_$name";
    if (empty($value) && isset($_REQUEST[$name])) $value = $_REQUEST[$name];
    if ($value!=="") $session[$sessionvarid] = $value;
    return $session[$sessionvarid];
  }

  function descriptor($record)
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

  function setLock()
  {

    $this->m_lock = Locking::getLockType(atkArrayNvl(getUser(), "id"),
                                         $this->m_viewdate);

    // If using weekly based locking, the locks will correspond with the
    // weekview, so we can safely remove the possibility to edit/add
    if ($this->m_lockmode == "week" && $this->m_lock != "")
    {
      // This week is locked..
      $this->addFlag(NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
    }
  }

  /**
   * Get the initial date of the hours node.
   *
   * The function can be overwritten to start with a different date
   * than today. This date will be used in the add form AND in the
   * week- or dayview datejumper.
   *
   * @return array initial_date
   */
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

  /**
   * Get the initial user of the hours node.
   *
   * The function can be overwritten to start with a different user
   * than today.
   *
   * @return array initial_date
   */
  function getInitialUser()
  {
    global $g_user;
    return $g_user["id"];
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
    if (projectutils::getActivityCount() == 1) {
      $db = &atkGetDb();
      $result = $db->getrows("SELECT id FROM activity");
      $values["activityid"]["id"] = $result[0]["id"];
    }


    //Set an initial workperiod if we can find one that has the defaultrate
    // set to true.
    $default = $this->getDefaultWorkPeriod();
    if($default > 0)
    {
      $values["workperiod"]["id"] = $default;
    }

    // Return the initial values
    return $values;
  }

  /**
   * Retrieve the default workperiod.
   *
   * @return id of default workperiod or 0 if none was found.
   */
  function getDefaultWorkPeriod()
  {
    $node = &atkGetNode("timereg.workperiod");
    /* @var $node atkNode */
    $records = $node->selectDb("defaultrate='1'","","","",array("id"));

    return (count($records)>0) ? $records[0]["id"] : 0;
  }

  function action_edit(&$handler)
  {
    $user = getUser();

    $recs = $this->selectDb($this->m_postvars["atkselector"],
                            "hours.id", "", "",
                            array("userid", "activitydate"));

    $this->m_lock = Locking::getLockType($recs[0]["userid"]["id"],
                      sprintf("%02d-%02d-%02d", $recs[0]["activitydate"]["year"],
                                                $recs[0]["activitydate"]["month"],
                                                $recs[0]["activitydate"]["day"]));
    if ($this->m_lock=="")
    {
      if (!$this->allowed("any_user"))
      {
        $this->addFilter("hours.userid",strtolower($user["id"]));
      }
      return $handler->action_edit();
    }
    else
    {
      // Period is locked. We may not edit, only view.
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
      $sessionManager = &atkGetSessionManager();
      if (!empty($this->m_postvars['confirm'])) // was 'yes' clicked?
      {
        $this->m_postvars = $sessionManager->getValue("postvars");
        $this->m_postvars = $this->m_postvars["postvars"];
        return $handler->action_save();
      }
      elseif (empty($this->m_postvars['cancel'])) // nothing clicked yet?
      {
        $sessionManager->globalVar("postvars", $this->m_postvars);

        // the 1=0 is a trick. without it, the confirmation page would try
        // to retrieve a record.
        $this->confirmAction("1=0", "save", false, TRUE, $_REQUEST);
      }
      else // was 'no' clicked?
      {
        $this->redirect(); // go back to where we came from
      }
    }
    else
    {
      return $handler->action_save(); // normal behaviour
    }
  }

  function confirmSaveText()
  {
    return atkconfig("hours_confirm_save");
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

    $this->setLock();

    $this->addFilter("hours.userid",$this->m_user);

    //we need to make sure the activitydate has the same value as $this->m_viewdate.
    $this->updatesessionrecordviewdate();

    if ($this->m_weekview!=1)
    {
      if ($this->m_lock=="") $res[] = $handler->invoke("addPage", $handler->getRejectInfo());
      $this->addFilter("activitydate",$this->m_viewdate);
      $res[] = $handler->invoke("adminPage");
    }
    else
    {
      if (atkconfig("timereg_week_bookable")==true && $this->m_lock=="") $res[] = $handler->invoke("addPage", $handler->getRejectInfo());
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
                        'lockicon'=>Locking::getLockIcon($this->m_lock),
                       );

      if ($this->m_viewdate<date('Y-m-d'))
      {
        // If current is not today we can also move forward
        $tplvars['tomorrowurl'] = session_url('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$tomorrow);
        $tplvars['todayurl'] = session_url('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$today);
      }
      $res = $ui->render($ui->templatePath('hours_adminheader.tpl', 'timereg'),$tplvars);
    }
    return $res;
  }

  function adminFooter()
  {
    if ($this->m_weekview!=1)
    {
      $balancenode = &atkGetNode("timereg.overtime_balance");
      $balance = $balancenode->getBalance(date("Y-m-d", adodb_mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4))), $this->m_user);

      $tplvars = array( 'scheduleurl'=>session_url('dispatch.php?atknodetype=scheduler.scheduler&viewdate='.$this->m_viewdate.'&view=day&atkaction=admin',SESSION_NESTED),
                        'balance'=>time_format((60*$balance["balance"]),true),
                       );
      $ui = &$this->getUi();
      return $ui->render($ui->templatePath('hours_adminfooter.tpl'),$tplvars);
   }
   return '';
  }

  function getDateJumper()
  {
    $dummy_rec = array("viewdate"=>$this->getInitialDate());
    $datebox = new atkDateAttribute("viewdate","","",0, date("Ymd"),AF_OBLIGATORY);
    return $datebox->edit($dummy_rec);
  }

  /**
   * Function can be overridden to create a different user selection.
   * @param none
   * @return String html
   */
  function getUserSelect()
  {
    if ($this->allowed("any_user"))
    {
      // Init filter
      $filterArr = $this->viewuserFilter();
      $filterArr[] = "role='employee'";
      $filter = implode(" AND ",$filterArr);

      //Init order by
      $orderby = $this->viewuserOrderBy();

      //Init desc fields
      $empNode = &atkGetNode("employee.employee");
      $fieldsArr = $empNode->descriptorFields();
      if(!in_array("id", $fieldsArr)) $fieldsArr[] = "id";
      $fields = implode(", ",$fieldsArr);

      $db = &atkGetDb();
      $users = $db->getRows("SELECT $fields FROM person WHERE $filter ORDER BY $orderby");
      $res = '<select name="viewuser">';

      for ($i=0;$i<count($users);$i++)
      {
        $descriptor = $empNode->descriptor($users[$i]);
        $res .= '<option value="'.$users[$i]["id"].'" '.($this->m_user == $users[$i]["id"] ? 'selected' : '').'>'.$descriptor;
      }
      $res .= '</select>';
    }
    return $res;
  }

  /**
    * Function returns an array with filter-options that are used
    * to filter the list of selectable employees.
    * Function can be overridden to change the filter.
    *
    * @return array with filter items (WHERE clause of SQL)
    */
   function viewuserFilter()
   {
     return array("status='active'");
   }

   /**
    * Function returns the orderby clause that is used when retrieving employees.
    * Function can be overridden to change the orderby.
    *
    * @return String ORDER BY clause of SQL
   */
   function viewuserOrderBy()
   {
     return "lastname";
   }

  function weekview($filterbycoordinator=false)
  {

    $viewtime = TimeregUtils::getViewTime($this->m_viewdate);
    $weekdates = TimeregUtils::getWeekDates($this->m_viewdate);

    $this->addStyle("style.css");
    return $this->getWeekviewData($viewtime,
                                  $weekdates,
                                  $filterbycoordinator);
  }


  function getWeekviewData($viewtime, $weekdates, $filterbycoordinator=false)
  {
    $userid = $this->m_user;
    $week = Array();
    $projtotals = Array();
    $daytotals = Array();

    $where = "activitydate >= '".$weekdates[1]['date']."'
              AND activitydate <= '".$weekdates[7]['date']."'
              AND hours.userid = '$userid'";

    if ($filterbycoordinator)
    {
      $coordinator = getUser();
      $where.= " AND B.coordinator = '".$coordinator["id"]."'";
    }
    $data = $this->selectDb($where,"","",$this->m_listExcludes);

    for ($i=0;$i<count($data);$i++)
    {
      $rec = $data[$i];
      $key = $rec["projectid"]["name"]." - ".$rec["phaseid"]["name"];
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

    $weekdata[0][] = atktext("project")." - ".atktext("phase");
    for ($i=1;$i<=7;$i++)
    {
      if ($weekdates[$i]['date'] <= date("Y-m-d"))
      {
        // Individual lock icons for each locked day
        if ($this->m_lockmode != "week")
        {
          $lockicon = Locking::getLockIcon($weekdates[$i]['lock'], "11");
        }

        $url = 'dispatch.php?viewdate='.$weekdates[$i]['date']."&weekview=0";
        $weekdata[0][] = href($url,$weekdates[$i]['day']).$lockicon.'<br>
                              ('.substr($weekdates[$i]['date'],5,2).
                              '-'.substr($weekdates[$i]['date'],8,2).')';
      }
      else
      {
        $weekdata[0][] = $weekdates[$i]['day'].'<br>('.
                         substr($weekdates[$i]['date'],5,2).'-'.
                         substr($weekdates[$i]['date'],8,2).')';
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
    $output.= "<br>".$this->weekviewFooter($weekdates, $viewtime);
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
    $userid           = $this->m_user;
    $record["userid"] = $userid;
    $att = &$this->getAttribute("userid");
    $att->populate($record);

    $locks = 0;

    // Check if any day of week is locked. $locks will represent the numbers of locked days.
    for ($i=1; $i<=7; $i++) {
      $locks += (Locking::getLockType($userid, date("Y-m-d", $weekdates[$i]["stamp"])) != "") ? 1 : 0;
    }

    $ui = &$this->getUi();
    $tplvars = array( 'session_form'=>session_form(),
                      'prevweekurl'=>session_url('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date']),
                      'thisweekurl'=>session_url('dispatch.php?weekview=1&viewdate='.date("Y-m-d")),
                      'dayviewurl'=>session_url('dispatch.php?weekview=0&viewdate='.date("Y-m-d")),
                      'datejumper'=>$this->getDateJumper(),
                      'curuser'=>$record["userid"],
                      'curdate'=>adodb_date('W, Y',$viewtime),
                      'lockicon'=>Locking::getLockIcon($locks),
                      'locktext'=>($locks > 0 && $locks < 7) ? atktext("locked_partially") : null
                     );
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $tplvars['nextweekurl']=session_url('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date']);
    }
    if ($this->m_lockmode == "week")
    {
      if ($this->m_lock=="individual" && $this->allowed("unlock"))
      {
        $tplvars['unlockurl']=session_url('dispatch.php?atkaction=unlock&userid='.$userid.'&viewdate='.$this->m_viewdate, SESSION_NESTED);
      }
      // users may only lock weeks that are not already locked. they must have the proper permission
      // and, if configured, they may only lock a week once they have booked all their hours.
      else if ($this->m_lock=="" && $this->allowed("lock") && (atkconfig("timereg_incompleteweeklock") || $this->weekComplete($userid, $this->m_viewdate)))
      {
        $tplvars['lockurl']=session_url('dispatch.php?atkaction=lock&userid='.$userid.'&viewdate='.$this->m_viewdate, SESSION_NESTED);
      }
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
    $tplvars = array( 'scheduleurl'=>session_url('dispatch.php?atknodetype=scheduler.scheduler&viewdate='.$this->m_viewdate.'&view=week&atkaction=admin',SESSION_NESTED),
                      'balance'=>false,
                     );
    $ui = &$this->getUi();
    return $ui->render($ui->templatePath('hours_adminfooter.tpl'),$tplvars);
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


  function action_lock(&$handler)
  {
    $period = Locking::getPeriod($this->m_viewdate);

    if (atkconfig("lock_period_approval_required"))
    {
      Locking::lockPeriod($period);
    } else {
      Locking::lockPeriod($period, "1");
    }

   $this->redirect();
  }


  function action_unlock(&$handler)
  {
    $viewdate = $this->m_postvars["viewdate"];
    Locking::unlockPeriod(Locking::getPeriod($viewdate));
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
      if (Locking::getLockType($this->m_user,
                               sprintf("%04d-%02d-%02d",
                                       $rec["activitydate"]["year"],
                                       $rec["activitydate"]["month"],
                                       $rec["activitydate"]["day"]))!="")
     {
       triggerError($rec, "activitydate", $this->m_lockmode."locked");
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
    $this->validateActivityId($record);

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

  function validateActivityId(&$record)
  {
    // If no activities available, trigger an error:
    if (projectutils::getActivityCount() == 0) {
      triggerError($record, "activityid", "Please add one or more activities first");
    }
    else {
      $obj = &atkGetNode("project.activity");
      $act = $obj->selectDb("id='".$record["activityid"]["id"]."'");
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
    $defaultcolumns = array("activitydate","userid","phaseid","projectid","activityid","remark","time","functionlevel", "creditor_id");
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
    $sessionManager = &atkGetSessionManager();
   // Insert mode can be "" (normal) or "multi" (multiple days at once).
    $this->m_insertmode = $sessionManager->stackVar("insertmode");
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
      $res.= atktext($days[$i]).': <input type="checkbox" name="'.$prefix.'dayfilter['.$i.']" class="atkcheckbox" value="1" '.$checked.'> &nbsp; ';
    }
    return $res;
  }

  function addDb(&$record, $exectrigger=true)
  {
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
        if (!in_array($dow, $doDays) || Locking::getLockType($this->m_user,$walker)!="")
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
    $locktype = Locking::getLockType($rec["userid"]["id"],
                                     sprintf("%02d-%02d-%02d",
                                             $rec["activitydate"]["year"],
                                             $rec["activitydate"]["month"],
                                             $rec["activitydate"]["day"]));

    // If the record doesn't belong to me or is locked
    if ($this->recordIsReadonly($rec, $userid, $locktype))
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
      $actionbase = dispatch_url($this->atknodetype(), "__PLACEHOLDER__", array("atkselector" => "[pk]", "viewuser" => $this->m_user));
      atkdebug('actionbase: '.$actionbase);
      $actions = array();
      $actions["edit"] = str_replace("__PLACEHOLDER__", "edit", $actionbase);
      $actions["delete"] = str_replace("__PLACEHOLDER__", "delete", $actionbase);
      $mraactions["mra_move"] = "mra_move";
    }
  }

  function recordIsReadonly($rec, $userid, $locktype)
  {
    return (empty($userid) || (($rec["userid"]["id"] != $userid) && !$this->allowed("any_user")) || ($locktype != ""));
  }

  /**
   * Checks wether or not there are any timeregistrations on a particulair project
   * Depends on being called from action_save
   * @return bool Wether or not it's the first timereg for the project
   */
  function _isThisTheFirstTimeRegForThisProject()
  {
    $phasenode =   &atkGetNode("project.phase");
    $hoursnode =   &atkGetNode("timereg.hours");
    $record = $this->updateRecord();
    $user = getUser();

    $excludelist = $this->_getExcludeListForNode($phasenode, array("id", "projectid"));

    $projectid = (int)$record["projectid"]["id"];

    $phases = $phasenode->selectDb($phasenode->m_table.".projectid='$projectid'",null,null,$excludelist);

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
    return true;
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
    return true;
  }
    // END WARNING

}

?>
