<?php

include_once("achievotools.inc");

useattrib("atkdateattribute");

define("ONEDAY", 24*60*60);
define("ONEWEEK", 7*ONEDAY);

class timeDisplayAttribute extends atkAttribute
{
  function display($rec)
  {
    $time = $rec[$this->fieldName()];
    if ($time==0) return "";
    if ($time < 0) return sprintf("-%02d",(floor(abs($time)/60))).':'.sprintf("%02d",(abs($time)%60));
    return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
  }
}

function compare_by_field($a, $b, $field)
{
  if ($a[$field]==$b[$field])
  {
    // equal items are always sorted by week
    if ($a["period"]==$b["period"]) return 0;
    return ($a["period"] < $b["period"]) ? -1 : 1;
  }
  return ($a[$field] < $b[$field]) ? -1 : 1;
}

class weekreport extends atkNode
{
  var $m_harvestedModules   = null; //contains references to harvested modules
  var $m_harvestedColumns   = null;
  var $m_overtimeActivities = null;

  function weekreport()
  {
    $this->atkNode("weekreport",NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT); // node() constructor is *not* called automatically!
    $this->add(new atkAttribute("userid"));
    $this->add(new atkAttribute("period"));
    $this->add(new atkAttribute("startdate"));
    $this->add(new atkAttribute("enddate"));
    $this->add(new timeDisplayAttribute("mon"));
    $this->add(new timeDisplayAttribute("tue"));
    $this->add(new timeDisplayAttribute("wed"));
    $this->add(new timeDisplayAttribute("thu"));
    $this->add(new timeDisplayAttribute("fri"));
    $this->add(new timeDisplayAttribute("sat"));
    $this->add(new timeDisplayAttribute("sun"));

    $db = &atkGetDb();
    $periods = $db->getRows("SELECT * FROM workperiod ORDER BY percentage");
    for($i=0;$i<count($periods);$i++)
    {
      $this->add(new timeDisplayAttribute($periods[$i]["name"], AF_TOTAL));
    }

    $this->add(new timeDisplayAttribute("overtimecompensation",AF_TOTAL));
    $this->addAdditionalColumns();
    $this->add(new timeDisplayAttribute("total",AF_TOTAL));
    $this->add(new timeDisplayAttribute("contract",AF_TOTAL));
    $this->add(new timeDisplayAttribute("overtime",AF_TOTAL));

    $this->add(new atkAttribute("balance"));
    $this->add(new atkAttribute("status"));
  }

  /**
   * Function for harvesting additional weekreport columns.
   * Beware: A column-name cannot exist more then once.
   */
  function addAdditionalColumns()
  {
    //retrieve additional weekreport columns ordered by module
    $this->harvestColumns();
    foreach($this->m_harvestedColumns as $module=>$columns)
    {
      foreach($columns as $col)
      {
        $attr = &$this->add(new timeDisplayAttribute($col, AF_TOTAL));
        /* @var $attr atkAttribute */
        $attr->setLabel(atktext($attr->fieldName(), $module));
      }
    }
  }

  function harvestColumns()
  {
    //only harvest if we haven't harvested yet.
    if($this->m_harvestedColumns == null)
      $this->m_harvestedColumns = atkHarvestModules("weekreport_getAdditionalColumns","",true);
  }

  function getOvertimeCompensationActivities()
  {
    if($this->m_overtimeActivities != null)
      return;

    /* @var $db atkDb */
    $db = &atkGetDb();
    $query = $db->createQuery();
    $query->addField("id");
    $query->addTable("activity");
    $query->addCondition("overtimecompensation = 1");
    $this->m_overtimeActivities = $db->getrows($query->buildSelect());
  }

  function isOvertimeCompensationActivity($id)
  {
    $this->getOvertimeCompensationActivities();

    foreach($this->m_overtimeActivities as $item)
    {
      if($item["id"]==$id) return true;
    }

    return false;
  }

  function status_display($rec)
  {
    return atktext($rec["status"]);
  }

  function get_employee($user_id)
  {
    $db = &atkGetDb();
    $sql = "SELECT * FROM person WHERE status='active' AND id='$user_id'";
    $record = $db->getrows($sql);
    return $record[0];
  }

  function get_employees($userid)
  {
    $harvest = atkHarvestModules("getEmpDropDown", '', true);

    if (is_array($harvest) && count($harvest) > 0)
    {
      if (moduleExists('advancedsecurity'))
//      $employee_code.='<OPTION VALUE="all">'.atktext("all_users").'</OPTION>';
      $employee_code .= $harvest['advancedsecurity'];
    }
    else
    {

      $db = &atkGetDb();

      $sql = "SELECT id, lastname,firstname,userid
            FROM person
            WHERE status='active' AND role = 'employee'
            ORDER BY lastname
           ";

      $records = $db->getrows($sql);

      $employee_code.='<OPTION VALUE="all">'.atktext("all_users").'</OPTION>';
      for($i=0;$i<count($records);$i++)
      {
        if ($userid==$records[$i]["id"]) $selected="selected";
        else $selected="";
        $employee_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$selected.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      }
    return $employee_code;
  }

  function balance_display($rec)
  {
    $node = &atkGetNode("timereg.overtime_balance");
    return $node->getOvertimeLink($rec);
  }

  function getHarvestedModuleObjects()
  {
    $this->harvestColumns();
    foreach($this->m_harvestedColumns as $module=>$columns)
    {
      $obj = &getModule($module);
      if(is_object($obj))
      {
        //If the modules have an initialise function, call it.
        if (method_exists($obj, "weekreport_init"))
          $obj->weekreport_init();

        $this->m_harvestedModules[$module] = $obj;
      }
    }
  }

  /**
   * One line description of the function
   *
   * @todo JvdW 12-01-2004 Oracle needs all columns not used in aggregate functions to be in the group by clause.
   * There might be more places in the code where this is true and need adjusting.
   * MySQL has an extended GROUP BY which doesn't need this according to the documentation:
   * "MySQL has extended the use of GROUP BY to allow you to select fields that are not mentioned in the GROUP BY clause.
   * If you are not getting the results you expect from your query, please read the GROUP BY description.
   * See section 6.3.7 Functions and Modifiers for Use with GROUP BY Clauses."
   *
   * @param type name description
   * @return type description
   */
  function collectData($nameswitch, $userid, $startdate, $enddate, $functionlevel_condition)
  {
    /* @var $db atkDb */
    $db = &atkGetDb();
    $query = $db->createQuery();

    $query->addField("time","", "hours");
    $query->addField("activityid", "", "hours");
    $query->addField("activitydate", "", "hours");
    $query->addField("userid", "", "hours");
    $query->addField("lastname", "", "person");
    $query->addField("firstname", "", "person");
    $query->addField("workperiod", "", "hours");

    $query->addTable("person");
    $query->addTable("hours");

    if ($nameswitch=="supervisor")
    {
      if($userid!="all")  $query->addCondition("person.supervisor='$userid'");
    }
    else
    {
      if($userid!="all")  $query->addCondition("hours.userid='$userid'");
    }

    $query->addCondition("hours.activitydate >= '$startdate'");
    $query->addCondition("hours.activitydate <= '$enddate'");
    $query->addCondition("hours.userid = person.id");
    $query->addCondition($functionlevel_condition);

    $query->addOrderBy("hours.userid, activitydate, person.lastname, person.firstname, workperiod");

    //@todo: addToWeekReportQuery() for each harvestedModule.
    $this->getHarvestedModuleObjects();
    if($this->m_harvestedModules!=null)
    {
      foreach($this->m_harvestedModules as $modulename=>$obj)
      {
        if (method_exists($obj, "weekreport_addToQuery"))
          $obj->weekreport_addToQuery($query);
      }
    }

    $rows = $db->getrows($query->buildSelect());

    return $rows;
  }

  /**
   * Get hours for selected user/supervisor (=manager)
   *
   * @param string $nameswitch all employees below 'supervisor' or 'name':selected user
   * @param int $user_id selected user
   * @param string $startdate
   * @param string $enddate
   * @param string $functionlevelswitch id of selected functionlevel
   * @param string $lowerlevels checkbox 'on' or 'off'
   * @return array $hourrecords
   */
  function getHourRecords($nameswitch, $user_id, $startdate, $enddate, $functionlevelswitch, $lowerlevels)
  {
    require_once(moduleDir('report').'utils/class.reportutils.inc');
    $repUtils = &atknew('report.utils.reportutils');

    $functionlevel_condition = $repUtils->getFunctionLevelCondition($functionlevelswitch, $lowerlevels);

    if ($nameswitch == 'supervisor')
    {
      $employees = $repUtils->getEmployeesArray($user_id); // get all users below this user
      $employees[]['id'] = $user_id;                       // add current user to employees

      $hourrecords = array();

      foreach ($employees as $employee)
      {
        $records = $this->collectData($nameswitch, $employee['id'], $startdate, $enddate, $functionlevel_condition);

        if (is_array($records) && count($records) > 0)
        {
          $hourrecords = array_merge($hourrecords, $records);
        }
      }
    }
    else
    {
      // not supervisor, get record for the selected user only
      $hourrecords = $this->collectData($nameswitch, $user_id, $startdate, $enddate, $functionlevel_condition);
    }

    return $hourrecords;
  }


  function getLocks($nameswitch, $userid, $report_start_week, $report_end_week)
  {
    $db = &atkGetDb();
    if ($nameswitch=="supervisor" && $userid!='all')
    {
      $query = "SELECT
                  period, hours_lock.userid
                FROM
                  hours_lock, person
                WHERE
                  ((hours_lock.userid = person.id AND person.supervisor='$userid')
                  OR hours_lock.userid IS NULL OR hours_lock.userid='')
                  AND
                    period between '$report_start_week' and '$report_end_week' ";
    }
    else
    {
      // Collect locks..
      $query = "SELECT
                  period, userid
                FROM
                  hours_lock
                WHERE
                  period between '$report_start_week' and '$report_end_week' ";
      if ($nameswitch=="name"&&$userid!="all")
      {
        $query.="AND (userid = '$userid' OR userid IS NULL OR userid = '')";
      }
    }
    return $db->getrows($query);
  }

  function action_report(&$handler)
	{
    global $g_securityManager, $g_user, $g_sessionManager;

    require_once(moduleDir('report').'utils/class.reportutils.inc');

    $repUtils = &atknew('report.utils.reportutils');

    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");

    $atkorderby = $g_sessionManager->pageVar("atkorderby");
    $userid = $g_sessionManager->pageVar("userid");
    $employeeswitch = $g_sessionManager->pageVar("nameswitch");

    $showdetails = $g_sessionManager->pageVar("showdetails");
    $showstatus = $g_sessionManager->pageVar("showstatus");

    $functionlevelswitch = $g_sessionManager->pageVar("functionlevelswitch");
    if (moduleExists('advancedsecurity'))
      $lowerlevels = atkArrayNvl($this->m_postvars, "lowerlevels", 'off');
    else
      $lowerlevels = "off";


    /** $outputType gives control over the kind of output:
    *        0 standard(onscreen in compliance with theme, default)
    *        1 printable (onscreen in a format that easily can be printed on paper))
    *             [was $printable]
    *        2 export ( to a file on local computer ex. achievo.csv)
    */
    $outputType = $g_sessionManager->pageVar("outputType");
    if (!isset($outputType)||$outputType=="") $outputType=0;

    if (!isset($atkorderby)||$atkorderby=="") $atkorderby = "period";

//    $view_all = $g_securityManager->allowed('reports.weekreport','view_all');
    $view_all = $this->allowed('view_all');
    $view_managed = $this->allowed('view_managed');

    if (moduleExists('advancedsecurity'))
      $view_level = $this->allowed('view_level');
    else
      $view_level = false;

    $startdate = $g_sessionManager->pageVar("startdate");
    $enddate = $g_sessionManager->pageVar("enddate");

    if (is_array($startdate) && !empty($startdate["day"]) && !empty($startdate["month"]) && !empty($startdate["year"]) && isset($startdate) && $startdate!="")
    {
      $startdate = $startdate["year"]."-".sprintf("%02d",$startdate["month"])."-".sprintf("%02d",$startdate["day"]);
    }
    else
    {
      $startdate =  date("Y-m-d" ,time()-(86400*7));
    }

    if (is_array($enddate) && !empty($enddate["day"]) && !empty($enddate["month"]) && !empty($enddate["year"]) && isset($enddate) && $enddate!="")
    {
      $enddate = $enddate["year"]."-".sprintf("%02d",$enddate["month"])."-".sprintf("%02d",$enddate["day"]);
    }
    else
    {
      $enddate = date('Y-m-d');
    }

    if (!isset($userid)||$userid=="")   			$userid=$g_user["id"];
    if (!isset($employeeswitch)||$employeeswitch=="")	$employeeswitch="name";
    if (!isset($showstatus)||$showstatus=="") $showstatus = "both";

    if ($outputType==0)
    {
      $output='<form action="dispatch.php" method="get" id="entryform" name="entryform">';

      $output.='<input type="hidden" name="atknodetype" value="reports.weekreport">';
      $output.='<input type="hidden" name="atkaction" value="'.$this->m_action.'">';


      $data = array();
      $data[] = array('<b>'.atktext('sethoursfilter').'</b>');

      // we have to pass a 'dummy' record to the attributes to set their default value.
      $dummyrec = Array("startdate"=>array("year"=>substr($startdate,0,4),
                                           "month"=>substr($startdate,5,2),
                                           "day"=>substr($startdate,8,2)),
                        "enddate"=>array("year"=>substr($enddate,0,4),
                                         "month"=>substr($enddate,5,2),
                                        "day"=>substr($enddate,8,2)));

//      $nameswitch = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($employeeswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>';

//      if($view_all)
//      {
//        $users='<SELECT name="userid">'.$this->get_employees($userid).'</SELECT>';
//      }
      if($view_all)
      {
        $nameswitch = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($employeeswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>';
        $users = ' <SELECT name="userid">'.$repUtils->get_employees($userid, true).'</SELECT>';
      }
      elseif($view_managed && !$view_level)
      {
         $nameswitch = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($employeeswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>';
         $users = ' <SELECT name="userid">'.$repUtils->get_employees($userid).'</SELECT>';
      }
      elseif ($view_level)
      {
        $nameswitch = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($employeeswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>';
        $users = ' <SELECT name="userid">'.$repUtils->getLevelEmployees($userid).'</SELECT>';
      }
      else
      {
        $userid = $g_user["id"];
        $users = $g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">';
      }
      $data[] = array($nameswitch,$users);

      $data[] = array(atktext('functionlevel','project').':',$repUtils->get_functionlevels($functionlevelswitch, $lowerlevels));

      $data[] = array('<hr>','<hr>');

      /*// TODO: Even if you don't have view_all right, you should still be able to view employees
      // that you supervise.
      if($view_all)
      {
        $data[] = array(atktext("supervisor"),'<SELECT name="supervisor">'.$this->get_supervisors($supervisor).'</SELECT>');
      }    */

      $startdateatt = new atkDateAttribute("startdate","","", 0, date("Ymd"));
      $enddateatt = new atkDateAttribute("enddate","","", 0, date("Ymd"));

      $data[]= array(atktext("timespan"),$startdateatt->edit($dummyrec).' &nbsp;'.
                      atktext("until").
                      '&nbsp; '.$enddateatt->edit($dummyrec));


      $data[] = array(atktext("status"),'<select name="showstatus">
                     <option VALUE="both" '.($showstatus=="both"?"selected":"").'>'.atktext("both").'
                     <option VALUE="locked" '.($showstatus=="locked"?"selected":"").'>'.atktext("locked").'
                     <option VALUE="unlocked" '.($showstatus=="unlocked"?"selected":"").'>'.atktext("unlocked").'
                     </select>');


      $data[] = array('<b>'.atktext("report_output_options").'</b>');

      $data[] = array(atktext("report_output_type"),'<select name="outputType">
                     <option VALUE="0" selected>'.atktext("report_output_standard").'
                     <option value="1">'.atktext("report_output_printable").'
                     <option value="2">'.atktext("report_output_export").'
                     </select>');

      $checked = ($showdetails==1) ? "checked" : "";
      $data[] = array(atktext("report_show_details"),'<input type="checkbox" class="atkcheckbox" name="showdetails" value="1" '.$checked.'>');

      $data[] = array(atktext("orderby"),'<select name="atkorderby">
                     <option VALUE="period" '.($atkorderby=="period"?"selected":"").'>'.atktext("period").'
                     <option VALUE="status" '.($atkorderby=="status"?"selected":"").'>'.atktext("status").'
                     <option VALUE="userid" '.($atkorderby=="userid"?"selected":"").'>'.atktext("userid").'
                     </select>');


      $tbl = &atknew("atk.utils.atktablerenderer");
      $tbl->setColSpan(0,0,2);
      $tbl->setColSpan(6,0,2);
      $output.=$tbl->render($data);
      $output.='<input type="submit" value="'.atktext("refresh").'"></form><br>';

      $tbl = &atknew("atk.utils.atktablerenderer");

      $parameters = $ui->renderBox(array("title"=>atktext("parameters"),"content"=>$output));
      $page->addContent($parameters);

    }

    // If we haven't got a userid by now, don't display the report
    if ($userid!="")
    {
      $weekdata = array();

      // first fill the weeks and start/end dates..
      $startstamp = adodb_mktime(12,0,0,substr($startdate,5,2),
                                 substr($startdate,8,2),
                                 substr($startdate,0,4));
      $endstamp = adodb_mktime(12,0,0,substr($enddate,5,2),
                               substr($enddate,8,2),
                               substr($enddate,0,4));

      // overzicht moet beginnen op de eerste dag van de week van de gekozen datum.
      $realweekstart = startOfWeek($startdate);
      $realstartstamp = adodb_mktime(12,0,0,substr($realweekstart,5,2),
                                     substr($realweekstart,8,2),
                                     substr($realweekstart,0,4));

      $nrofdays = ceil(($endstamp-$realstartstamp)/(ONEDAY));

      for ($i=0;$i<=$nrofdays;$i++)
      {
        $curstamp = $realstartstamp + (ONEDAY*$i);
        if (strftime("%w",$curstamp)==1)
        {
          $key = weekstamp($curstamp);
          $weekdata[$key]["period"] = $key;
          $weekdata[$key]["startdate"] = strftime("%d-%m-%Y",$curstamp);
          $weekdata[$key]["enddate"] = strftime("%d-%m-%Y",$curstamp+(6*ONEDAY));
          $i+=6;
        }
      }

      $report_start_week = weekstamp($startstamp);
      $report_end_week = weekstamp($endstamp);

      // determine contracthours per week..
      $contracts = $repUtils->getContractHours($employeeswitch, $userid, $enddate, $startdate, $functionlevelswitch, $lowerlevels);

      $end = endOfWeek(date("Y-m-d", $endstamp));
      $newendstamp = adodb_mktime(0,0,0,substr($end,5,2),substr($end,8,2)+1,substr($end,0,4));
      for ($j=$startstamp;$j<=$newendstamp;$j+=ONEWEEK)
      {
        $weekstamp = weekstamp($j);
        $start_week = date("Y-m-d", adodb_mktime(12,0,0,substr($weekdata[$weekstamp]["startdate"],3,2),
                                 substr($weekdata[$weekstamp]["startdate"],0,2),
                                 substr($weekdata[$weekstamp]["startdate"],6,4)));
        $end_week = date("Y-m-d", adodb_mktime(12,0,0,substr($weekdata[$weekstamp]["enddate"],3,2),
                               substr($weekdata[$weekstamp]["enddate"],0,2),
                               substr($weekdata[$weekstamp]["enddate"],6,4)));
        $weekdata[$weekstamp]["period"] = $weekstamp;
        for ($i=0;$i<count($contracts);$i++)
        {
          if ($contracts[$i]["startdate"] <= $start_week && ($contracts[$i]["enddate"] >= $start_week || $contracts[$i]["enddate"]==NULL))
          {
            $weekdata[$weekstamp]["userdata"][$contracts[$i]["id"]]["contract"] += $contracts[$i]["uc_hours"]*60;
          }

          $weekdata[$weekstamp]["userdata"][$contracts[$i]["id"]]["name"] = $contracts[$i]["lastname"].", ".$contracts[$i]["firstname"];
        }
      }

      $db = &atkGetDb();
      $periods      = $db->getRows("SELECT * FROM workperiod");
      $strdow       = array("sun","mon","tue","wed","thu","fri","sat");

      // collect data..
      $raw = $this->getHourRecords($employeeswitch, $userid, $startdate, $enddate, $functionlevelswitch, $lowerlevels);
      $this->getHarvestedModuleObjects();

      for ($i=0,$_i=count($raw);$i<$_i;$i++)
      {
        //is this timeregistration a overtimecompensation?
        $isovertimecompensation = $this->isOvertimeCompensationActivity($raw[$i]["activityid"]);

        $stamp = adodb_mktime(12,0,0,substr($raw[$i]["activitydate"],5,2),
                                      substr($raw[$i]["activitydate"],8,2),
                                      substr($raw[$i]["activitydate"],0,4));
        $dow = strftime("%w",$stamp);
        $key = weekstamp($stamp);
        $weekdata[$key]["period"] = $key;
        $uid = $raw[$i]["userid"];

        $weekdata[$key]["userdata"][$uid][$strdow[$dow]]          += $raw[$i]["time"];
        $weekdata[$key]["userdata"][$uid][$raw[$i]["workperiod"]] += $raw[$i]["time"];

        //we do not add overtimecompensation hours to the total
        if($isovertimecompensation)
          $weekdata[$key]["userdata"][$uid]["overtimecompensation"] += $raw[$i]["time"];
        else
          $weekdata[$key]["userdata"][$uid]["total"]                += $raw[$i]["time"];

        //Check for each column if the record should be added to it.
        //Each module can determine this on its own.
        foreach($this->m_harvestedColumns as $modulename=>$columns)
        {
          //get the module object
          $obj = $this->m_harvestedModules[$modulename];

          if(method_exists($obj,"weekreport_addToColumn"))
          {
            foreach($columns as $col)
            {
              if($obj->weekreport_addToColumn($raw[$i],$col))
                $weekdata[$key]["userdata"][$uid][$col] += $raw[$i]["time"];
            }
          }
        }

        $weekdata[$key]["userdata"][$uid]["name"] = $raw[$i]["lastname"].", ".$raw[$i]["firstname"];
        // We need to display the first date and last date of each week.
        // Since we are looping days now, it is easiest if we calculate the
        // beginning of a week here.
      }

      $locks = $this->getLocks($employeeswitch, $userid, $report_start_week, $report_end_week);

      for($i=0,$_i=count($locks);$i<$_i;$i++)
      {
        if ($locks[$i]["userid"]=="" || $locks[$i]["userid"]==0)
        {
          atkdebug("<h2>A</h2>");
          $weekdata[$locks[$i]["period"]]["userdata"][$userid]["status"]="locked";
        }
        else
        {
          atkdebug("<h2>B</h2>");
          $weekdata[$locks[$i]["period"]]["userdata"][$locks[$i]["userid"]]["status"]="locked";
        }

      }

      // weekdata is an associative array, indexed by weeknumber.
      // it must be converted to a normal array in order to be usable
      // by recordlist functions.
      // we also need to calculate some numbers.
      $rldata = array();
      $balancenode = &atkGetNode("timereg.overtime_balance");
      foreach ($weekdata as $weeknr => $data)
      {
        $record = array();
        $record["startdate"] = $data["startdate"];
        $record["enddate"] = $data["enddate"];
        $record["period"] = $data["period"];
        $record["status"] = $data["status"];
        if (is_array($data["userdata"]))
        {
          foreach($data["userdata"] as $user => $userdata)
          {
            $record["userid"] = $userdata["name"];
            $record["balance"] = $balancenode->getBalance(date("Y-m-d", adodb_mktime(0,0,0,substr($data["enddate"],3,2),substr($data["enddate"],0,2),substr($data["enddate"],6,4))), $user);
            $record["balance"]["outputtype"] = $outputType;
            $newtotal = 0;

            //count total hours of working periods
            for($i=0,$_i=count($periods);$i<$_i;$i++)
            {
              $record[$periods[$i]["name"]] = $userdata[$periods[$i]["id"]];
              $newtotal += ($periods[$i]["percentage"]/100) * $userdata[$periods[$i]["id"]];
            }
            $newtotal += $userdata[0];
            if ($newtotal == 0) $newtotal = $userdata["total"];
            //subtract overtimecompensation hours from newtotal
            $newtotal -= $userdata["overtimecompensation"];
            $record["overtimecompensation"] = $userdata["overtimecompensation"];
            $record["total"] = $userdata["total"];
            $record["contract"] = $userdata["contract"];
            $record["overtime"] = $newtotal-$userdata["contract"];

            //The additional columns must be added to the record,
            //so they are rendered.
            foreach($this->m_harvestedColumns as $modulename=>$columns)
            {
              foreach($columns as $col)
                $record[$col] = $userdata[$col];
            }

            if ($userdata["status"]!="")
            {
              $record["status"] = $userdata["status"];
            }
            else
            {
              $record["status"] = $data["status"];
            }
            $cmpstatus = (($record["status"]=="locked")?"locked":"unlocked");
            if ($showstatus=="both" || $cmpstatus==$showstatus)
            {
              for($i=0,$_i=count($strdow);$i<$_i;$i++)
              {
                $record[$strdow[$i]] = $userdata[$strdow[$i]];
              }
              $rldata[] = $record;
            }
          }
        }
      }

      // sorting..
      if ($atkorderby=="") $atkorderby = "period";

      list($field, $desc) = explode(' ', $atkorderby);
      $func = create_function('$a, $b', 'return compare_by_field('.($desc != null ? '$b, $a' : '$a, $b').', "'.$field.'");');

      if (function_exists($func))
        usort($rldata, $func);

      if ($showdetails==1)
      {
        $suppress = array(); // show all columns
      }
      else
      {
        $suppress = $strdow; // suppress day details
        $suppress[] = "startdate";
        $suppress[] = "enddate";
      }
      if ($employeeswitch=="name" && $userid!="all") $suppress[] = "userid";

      if ($outputType=="0")
      {
        if ($userid!="all")
        {
          $emprec = $this->get_employee($userid);

          if ($employeeswitch=="supervisor")
          {
            $label = atktext("supervisor");
          }
          else
          {
            $label = atktext("name");
          }
          $data = array();
          $data[] = array($label.": ",$emprec["lastname"].", ".$emprec["firstname"]);
          $data[] = array(atktext("email"),$emprec["email"]);
        }
        $tbl = &atknew("atk.utils.atktablerenderer");
        $output = $tbl->render($data);
        $rl = &atknew("atk.recordlist.atkrecordlist");
        $output.='<br>'.$rl->render($this, $rldata, array(),false,$suppress,"rlform",array(),"","rlform");
        $weekreport = $ui->renderBox(array("title"=>atktext("weekreport")." ".atktext("report_intimespan")." ".$startdate." t/m ".$enddate,"content"=>$output));
        $page->addContent($weekreport);
      }
      else if ($outputType=="1")
      {
        $rl = &atknew("atk.recordlist.atkcustomrecordlist");
        $page->addContent($rl->render($this, $rldata,"<tr>","<td>","</td>","</tr>", "0","",$suppress));
      }
      else if ($outputType=="2")
      {
        // special RecordList can export to file
        $rl = &atknew("atk.recordlist.atkcustomrecordlist");
        $rl->render($this, $rldata, "", '"', '";', "\r\n", "1", "",$suppress);
      }

    }
  }
}

?>
