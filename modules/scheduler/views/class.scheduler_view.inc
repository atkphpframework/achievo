<?php
/**
 * View class that handles all views of the scheduler
 *
 * @package achievo
 * @author sandy@achievo.org
 * @version 1.0
 *
 * TODO:
 * - Year day appointment
 * - Year date appointment
 * - Public function to check if somebody is free
 *   isFree($date,$time,$person,$marge)
 */
atkimport("modules.scheduler.utils.schedulertools");

class scheduler_view
{


 /**
  * @var int $m_day Day
  */
  var $m_day = 0;

 /**
  * @var int $m_day Day
  */
  var $m_month = 0;

 /**
  * @var int $m_year Year
  */
  var $m_year = 0;

 /**
  * @var string $m_viewdate Viewdate in yyyy-mm-dd
  */
  var $m_viewdate = "";

 /**
  * @var string $m_viewdate Viewdate in yyyy-mm-dd
  */
  var $m_small_viewdate = "";

 /**
  * @var int $m_year Year
  */
  var $m_calendarStartDay = 1;

 /**
  * @var string $m_view Current view
  */
  var $m_view = "";

 /**
  * @var string $m_user Current user
  */
  var $m_user = "";

 /**
  * @var string $m_viewUser Current userview
  */
  var $m_viewUsers = array();

 /**
  * @var string $m_viewTitle View Title
  */
  var $m_viewTitle = "";

 /**
  * @var array $m_allDayItems All allday calendar items
  */
  var $m_allDayItems = array();

 /**
  * @var array $m_appointmentItems All appointment calendar items
  */
  var $m_appointmentItems = array();

 /**
  * @var array $m_itemColors The colors of an calendar item
  */
  var $m_itemColors = array();

 /**
  * @var string $m_legend Legend of a calendar
  */
  var $m_legend = "";
  var $m_attendees = array();
  var $m_users = array();

  var $m_weekday_arr = array(1,2,4,8,16,32,64);
  var $m_workdays = 0;
  var $m_showWeekNumbers = 0;
  var $m_assistantFor = array();

 /**
  * @var string $m_emptyWorkHoursColor Default color for empty workhours calendar items
  */
  var $m_emptyWorkHoursColor = "#FFFFFF";

 /**
  * @var string $m_emptyColor Default color for empty calendar items
  */
  var $m_emptyColor   = "#DDDDDD";

  var $m_showTodo = 0;
  var $m_showLunarPhases = 0;

 /**
  * Calender view constructor
  * @return void
  */
  function scheduler_view()
  {
  }

 /**
  * Set Calendar view
  * @param string $view Current view
  */
  function setView($view)
  {
     $this->m_view = $view;
  }

 /**
  * Set one view user. If left to the default, all users will be displayed.
  * if the value contains the string 'all' all users will be showed.
  * @param string|array $user User to view scheduler of
  */
  function setViewUser($user)
  {  
     // empty string => all users
     if (empty($user))
     {
       $user = array();
     }
         
     // other string => only that user
     if (!is_array($user))
     {
       $user = array((string) $user);
     }
     
     $this->m_viewUsers = $user;
  }

 /**
  * Set viewdate
  * @param int $day Day
  * @param int $month Month
  * @param int $year Year
  */
  function setViewDate($day=0, $month=0,$year=0)
  {
    $this->m_day    = ($day   == 0) ? date("d") : $day;
    $this->m_month  = ($month == 0) ? date("m") : $month;
    $this->m_year   = ($year  == 0) ? date("Y") : $year;
    $this->m_viewdate = sprintf('%04d-%02d-%02d',$this->m_year,$this->m_month,$this->m_day);
  }

 /**
  * Set the viewdate for the small calendar
  *
  * @param int $day Day
  * @param int $month Month
  * @param int $year Year
  */
  function setSmallViewDate($day,$month,$year)
  {
    if($day=="")
    {
      $this->m_small_viewdate = $this->m_viewdate;
    }
    else
    {
      $this->m_small_viewdate = sprintf('%04d-%02d-%02d',$year,$month,$day);
    }
  }

 /**
  * Set the calendar start day
  * @param int $day Startday 0=Sunday, 1=Monday
  */
  function setCalendarStartDay($day)
  {
    $this->m_calendarStartDay = $day;
  }

 /**
  * Create default view header html
  *
  * @todo Convert to template
  * @return string default view header
  */
  function _view_header_html($params)
  {
    $view_header = "";
    foreach($params["links"] as $label => $url)
      $view_header .= sprintf("<a href='%s'>%s</a>&nbsp;", $url, atkText($label, "scheduler"));
    $view_header.= "<br><br><table width='90%'>";
    $view_header.= "<tr>";
    $view_header.= "<td valign='top' align='left'>".$params["nav"]."</td>";
    $view_header.= "<td valign='top' align='right'>".$params["selector"]."</td>";
    $view_header.= "</tr>";
    $view_header.= "<tr><td valign='top' align='left'><b>".$params["viewtitle"]."</b></td>";
    $view_header.= "</tr></table>";
    return $view_header;
  }

  /**
   * Returns the default view header
   *
   * @return string default view header
   */
  function _view_header()
  {
    $links = array();
    $links["additem"] = session_url(dispatch_url("scheduler.scheduler", "add"),SESSION_NESTED);
    $views = array("day", "week", "month", "birthday" /*, "year", "employee"*/);
    foreach($views as $view)
      $links[$view."view"] = session_url(dispatch_url("scheduler.scheduler", "admin", array("view"=>$view)));
      
    $links["refreshview"] = session_url(dispatch_url("scheduler.scheduler", "admin", array("view"=>$this->m_view, "viewdate"=>$this->m_viewdate)));

    $viewheaderparams = array(
      "links" => $links,
      "nav" => $this->_getNav(),
      "selector" => $this->_getSelector(),
      "viewtitle" => $this->m_viewTitle,
    );
    return $this->_view_header_html($viewheaderparams);
  }

  /**
   * Get weekNumber
   *
   * @param int $day Day
   * @param int $month Month
   * @param int $year Year
   * @return int Weeknumber
   */
  function getWeekNumber($day,$month,$year)
  {
    atkimport("modules.scheduler.utils.weeknumber");
    $timestamp = mktime(12,0,0,$month,$day,$year);
    $startday = atkconfig("startday",1);
    $firstweekcontains = atkconfig("firstweekcontains",4);
    $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$firstweekcontains);
    return $weeknumber;
  }
 
  /**
   * Creates a small calendar with month navigation
   *
   * @param int $day Day
   * @param int $month Month
   * @param int $year Year
   * @return string a small html calendar
   */
  function _getSmallCalendar($day="",$month="",$year="",$monthNav=true,$showdaysnextmonth=false)
  {
    if($day=="" && $month=="" && $year=="")
    {
       $day = $this->m_day;
       $month = $this->m_month;
       $year = $this->m_year;
    }

    atkimport("modules.utils.dateutil");
    $days = dateutil::short_daylist();
    for($i=0;$i<=6;$i++)
      $days[$i] = atkText($days[$i],"atk");

    $months = dateutil::monthlist();
    for($i=1; $i<=12; $i++)
      $months[$i] = atkText($months[$i], "atk");

    $days_in_month = mktime (0,0,0,$month+1,0,$year);
    $days_in_month = (strftime ("%d",$days_in_month));

    $first_day_of_month = date("w", mktime(0,0,0,$month,1,$year));

    $prevTime = date("Y-m-d", mktime(0,0,0,$month-1, 1, $year));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&small_viewdate=$prevTime";

    $nextTime = date("Y-m-d", mktime(0,0,0,$month+1, 1, $year));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&small_viewdate=$nextTime";
    $currentTime = date("Y-m-d", mktime(0,0,0,$month, 1, $year));
    $currentUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=".$currentTime."&small_viewdate=$currentTime";


    $tmp ='<table border="0" cellpadding="1" cellspacing="0">';
    $tmp.='<tr style="background-color: #dddddd">';
    $tmp.='<td align="left">';
    if($monthNav) $tmp.= href($prevUrl,"&lt&lt");
    $tmp.='</td>';
    $tmp.='<td align="center" colspan="'.($this->m_showWeekNumbers?6:5).'">';
  	$tmp.= href($currentUrl,$months[intval($month)].' '.$year);
    $tmp.='</td>';
    $tmp.='<td align="right">';
    if($monthNav) $tmp.= href($nextUrl,"&gt;&gt;");
    $tmp.='</td>';

    $tmp.='</tr><tr>';
    if($this->m_showWeekNumbers==1) $tmp.='<td>&nbsp;</td>';

    $day_number = $this->m_calendarStartDay;
    for ($i=0;$i<7;$i++)
    {
    	if ($day_number == 7) $day_number = 0;
    	$tmp.='<td align="center" style="border-bottom: 1px solid #C0C0C0;">'.$days[$day_number].'</td>';
    	$day_number++;
    }
    $tmp.='</tr>';

    $cellcount=$first_day_of_month-$this->m_calendarStartDay;
    if ($cellcount < 0) $cellcount = 7 + $cellcount;

    $tmp.='<tr>';
    $week_number = $this->getWeekNumber(1,$month,$year);
    $weekTime = date("Y-m-d",mktime(12,0,0,$month,1,$year));
    $weekUrl = $_SERVER["PHP_SELF"]."?view=week&viewdate=$weekTime";
    if($this->m_showWeekNumbers==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
    for ($i=1;$i<=$cellcount;$i++)
    {
    	$tmp.='<td>&nbsp;</td>';
    }
    $startdate = date("Y-m-d",mktime(0,0,0,$month,1,$year));
    $enddate = date("Y-m-d",mktime(23,59,59,$month,$days_in_month,$year));
    $itemdates = $this->getItemsCompact($startdate,$enddate);
    for ($i=1;$i<=$days_in_month;$i++)
    {
    	if ($cellcount == 7)
    	{
    		$tmp.="</tr><tr>\n";
    		$week_number++;
        $weekTime = date("Y-m-d",mktime(12,0,0,$month,$i,$year));
        switch ($this->m_calendarStartDay)
        {
          case 0: $tmp_i = $i+1;
          case 1: $tmp_i = $i;
          case 3: $tmp_i = $i-2;
          case 6: $tmp_i = $i+2;
        }
        $week_number = $this->getWeekNumber($tmp_i,$month,$year);
        $weekUrl = $_SERVER["PHP_SELF"]."?view=week&viewdate=$weekTime";
        if($this->m_showWeekNumbers==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
    		$cellcount=0;
    	}
    	$cellcount++;

      $dayTime = date("Y-m-d",mktime(12,0,0,$month,$i,$year));
    	if (($i == date("d")) && ($year == date("Y")) && ($month==date("m")) && (isset($itemdates[$dayTime]) && $itemdates[$dayTime]===true))
    	{
    		$class="calendarItemToday";
    	}
    	elseif(($i == date("d")) && ($year == date("Y")) && ($month==date("m")))
    	{
    		$class="calendarToday";
    	}
    	elseif(isset($itemdates[$dayTime]) && $itemdates[$dayTime]===true)
    	{
    	  $class="calendarItem";
    	}
    	else
    	{
    		$class="calendarNormal";
    	}

    	$dayUrl = $_SERVER["PHP_SELF"]."?view=day&viewdate=$dayTime";
    	$daynumber = ($i>9?$i:"&nbsp;".$i);
    	if(($this->m_view=="day" && $i == $this->m_day && $month == $this->m_month && $year == $this->m_year) ||
         ($this->m_view=="week" && $week_number == $this->m_weeknumber) ||
         ($this->m_view=="month" && $this->m_month==$month))
    	{
    	  $bgcolor="#FFDD00";
    	}
    	else
    	{
    	  $bgcolor="#FFFFFF";
    	}
    	$tmp.='<td align="right" bgcolor="'.$bgcolor.'">'.href($dayUrl,$daynumber,'','','class="'.$class.'"').'</td>';
    }
    $remaining_cells = 7-$cellcount;

    for ($i=0;$i<$remaining_cells;$i++)
    {
    	$tmp.='<td style="color: #C0C0C0">'.($showdaysnextmonth?($i+1):'&nbsp;').'</td>';
    }

    $tmp.='</tr>';
    $tmp.='</table>';
    return $tmp;
  }

 /**
  * Return the initalDate for a view
  * @return array Initial Date
  */
  function _getInitialDate()
  {
    if ($this->m_viewdate!="")
    {
      $initial_date = array("year"=>$this->m_year,
                            "month"=>$this->m_month,
                            "day"=>$this->m_day);
    }
    else
    {
      $initial_date = array("year"=>date("Y"),
                            "month"=>date("m"),
                            "day"=>date("d"));
    }
    return $initial_date;
  }

 /**
  * Creates a html form with a date selector
  * @return string html form with a date selector
  */
  function _getSelector()
  {
    $form  = '<form method="get" action="'.$_SERVER["PHP_SELF"].'">'."\n";
    $form .= session_form();
    $form .= '<input type="hidden" name="view" value="'.$this->m_view.'">';
    $dummy_rec = array("viewdate"=>$this->_getInitialDate());
    $datebox = new atkDateAttribute("viewdate","F d Y","d F Y",0,0,AF_OBLIGATORY);
    $form .=$datebox->edit($dummy_rec);
    $form .= '<input type="submit" value="'.atkText("refresh","scheduler").'">'."\n";
    $form .= '</form>'."\n";
    return $form;
  }

  /**
   * Gets employees. Used in getEmployeesInSelect and getEmployeesInCheckboxes
   *
   * @return array employees
   */
  function _getEmployees()
  {
    $g_db = &atkgetdb();

    $sql = "SELECT lastname,firstname,id
            FROM person
            WHERE status='active' AND role='employee'
            ORDER BY lastname
           ";

    $records = $g_db->getrows($sql);
    return $records;
  }
  
  /**
   * Gets the employee selector as a form with checkboxes
   *
   * @return string html form
   */
  function _getEmployeesCheckboxesForm()
  {
    $buff = '<form method="get" action="'.$_SERVER["PHP_SELF"].'">'."\n";
    $buff.= session_form();
    $buff.= '<input type="hidden" name="view" value="'.$this->m_view.'">';
    $buff.= '<div class="employees_checklistheader">'.atkText("employeeview","scheduler").'</div>';
    $buff.= $this->_getEmployeesCheckboxes();
    $buff.= '<input type="submit" value="'.atkText("refresh","scheduler").'">'."\n";
    $buff.= '<br>';
    $buff.= '</form>';
    
    return $buff;
  }
  
 /**
  * Get Employee selector input elements as checkboxes
  * @param array $default wich items should be checked?
  * @param  bool $show_all show the 'all employees' option?
  * @return string html of the input elements
  */
  function _getEmployeesCheckboxes($default=array(), $show_all=true)
  {
    // get the records
    $records = $this->_getEmployees();
    if(count($default)==0)
    {
      $default = $this->m_viewUsers;
    }
    
    $employee_code = '<ul class="employees_checklist">';
	  $is_default_all = in_array('all', $default);
	      
    // add the 'all' option to top of records 
    if($show_all)
    {    
      array_unshift($records, array(
        'id'        => 'all',
        'firstname' => '',
        'lastname'  => '<strong>'.atkText("allusers","scheduler").'</strong>',
      ));
    }
    
    // build up the html
    for($i=0;$i<count($records);$i++)
    {
      $sel='';
      if(in_array($records[$i]["id"],$default))
      {
        $sel="checked";
      }
      
      // name format
      $id    = $records[$i]["id"];
      $name  = $records[$i]["lastname"];
      if (!empty($records[$i]["firstname"])) {
        $name .= ', '.$records[$i]["firstname"];
      }
      
      // checkboxes
      $input = "<input id=\"user_{$id}\" type=\"checkbox\" value=\"{$id}\" name=\"user[]\" {$sel} />";
      $label = "<label for=\"user_{$id}\">{$input}{$name}</label>";
      
      $employee_code .= "<li>{$label}</li>";
    }
    
    $employee_code .= '</ul>';
    return $employee_code;
  }

 /**
  * Returns the default view navigation
  * @return string default view navigation
  */
  function _getNav()
  {
  }

 /**
  * Get the legend of the calendar
  * @return string html table with the calendar legend
  */
  function getLegend()
  {
    $g_db = &atkgetdb();

    // get the defined colors for each scheduler-type
    $query = "SELECT * FROM scheduler_category ORDER BY id";
    $nrows = $g_db->getRows($query);
    if(count($nrows)==0)
    {
      $this->m_legend="";
      return "";
    }

    $legend  = '<table border="0" cellPadding="1">
                <tbody>
                <tr>
                  <td bgColor="#000000">
                    <TABLE width="100%" border="0" cellPadding="2" cellSpacing="0">
                    <tbody>
                    <tr bgcolor="#FFFFFF"><td colspan="2"><b>'.atktext("legend","scheduler").':</b></td></tr>';

    for ($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $this->m_itemColors[$nrows[$i]["id"]]["desc"] = $nrows[$i]["description"];
      $this->m_itemColors[$nrows[$i]["id"]]["fore"] = $nrows[$i]["fgcolor"];
      $this->m_itemColors[$nrows[$i]["id"]]["back"] = $nrows[$i]["bgcolor"];

      $tmpLegend = '<tr bgcolor="#FFFFFF">
                      <td>
                        <table border="0" cellpadding="0" cellspacing="0">
                        <tr>
                          <td bgcolor="'.$nrows[$i]["bgcolor"].'"><img src="images/trans.gif" border="1" width="10" height="10"></td>
                        </tr>
                        </table>
                      </td>
                      <td><font color="#000000" face="verdana" size="-1">'.$nrows[$i]["description"].'</font></td>
                    </tr>';
      $legend.= $tmpLegend;
    }

    $this->m_legend.= $legend."</tbody></table></td></tr></tbody></table>";
  }

  /**
   * Convert an item to a record
   *
   * @param array $item Array with a scheduler item
   * @return array Item record
   */
  function item2record($item)
  {
    return array("startdate"=>array("day"=>substr($item["startdate"],8,2),"month"=>substr($item["startdate"],5,2),"year"=>substr($item["startdate"],0,4)),
                 "enddate"=>array("day"=>substr($item["enddate"],8,2),"month"=>substr($item["enddate"],5,2),"year"=>substr($item["enddate"],0,4)),
                 "starttime"=>array("hours"=>substr($item["starttime"],0,2),"minutes"=>substr($item["starttime"],3,2),"seconds"=>substr($item["starttime"],6,2)),
                 "endtime"=>array("hours"=>substr($item["endtime"],0,2),"minutes"=>substr($item["endtime"],3,2),"seconds"=>substr($item["endtime"],6,2)),
                 "recur"=>$item["recur"],
                 "lastdate"=>$item["lastdate"],
                 "id"=>$item["id"],
                 "cyclus"=>array("cyclus_enddate"=>array("day"=>substr($item["cyclus_enddate"],8,2),"month"=>substr($item["cyclus_enddate"],5,2),"year"=>substr($item["cyclus_enddate"],0,4)),
                              "startday"=>$item["startday"],
                              "endday"=>$item["endday"],
                              "startmonth"=>$item["startmonth"],
                              "endmonth"=>$item["endmonth"],
                              "every"=>$item["every"],
                              "month_time"=>$item["month_time"],
                              "weekday"=>array($item["weekday"])));
  }

  /**
   * Update recurring events for a given period
   *
   * @param string $startdate Start date
   * @param string $enddate End date
   */
  function updateRecurringEvents($startdate,$enddate)
  {
    $db = &atkGetDb();
    atkimport("module.scheduler.utils.schedulertools");
    $tmp_startdate = mktime(0,0,0,substr($startdate,5,2),substr($startdate,8,2),substr($startdate,0,4));
    $tmp_enddate = mktime(0,0,0,substr($enddate,5,2),substr($enddate,8,2),substr($enddate,0,4));

    $sql = "SELECT * FROM scheduler_scheduler,scheduler_cyclus
            WHERE scheduler_scheduler.id = scheduler_cyclus.scheduler_id
              AND scheduler_scheduler.recur!='once'
              AND scheduler_scheduler.lastdate > $tmp_startdate AND scheduler_scheduler.lastdate < $tmp_enddate
              AND scheduler_cyclus.cyclus_enddate >='$startdate' AND scheduler_cyclus.cyclus_enddate <= '$enddate' ";
    $nrows = $db->getrows($sql);
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $id = $nrows[$i]["id"];
      $rec = $this->item2record($nrows[$i]);

      $startdate = date("Y-m-d",($rec["lastdate"]+86400));
      $endstamp = $rec["lastdate"]+(32*86400);
      $enddate = date("Y-m-",$endstamp);
      $enddate.= date("t",$endstamp);
      $dates = schedulertools::getDates($rec,$startdate,$enddate);
      foreach($dates as $date)
      {
        $tmp_start = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]);
        $tmp_end = mktime($rec["endtime"]["hours"],$rec["endtime"]["minutes"],$rec["endtime"]["seconds"],$rec["enddate"]["month"],$rec["enddate"]["day"],$rec["enddate"]["year"]);
        $duration = $tmp_end-$tmp_start;

        $startdate = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],substr($date,5,2),substr($date,8,2),substr($date,0,4));
        $enddate = $startdate+$duration;

        $sql = "INSERT INTO scheduler_dates (scheduler_id,startdate,enddate) VALUES ('$id','$startdate','$enddate')";
        $db->query($sql);
      }
      $sql = "UPDATE scheduler_scheduler SET lastdate = '$endstamp' WHERE id='$id'";
      $db->query($sql);
    }
  }

  /**
   * Get Items for the small scheduler
   *
   * @param string $startdate Startdate
   * @param string $enddate Enddate
   * @return array Array with dates that have an Event
   */
  function getItemsCompact($startdate,$enddate)
  {
    $user = array(); // @FIXME This if below will always return false. user array is empty!
    if(count($user)==0)
    {
      $user = $this->m_viewUsers;
    }

    $this->updateRecurringEvents($startdate,$enddate);
    $name = "atk".atkconfig("database")."query";
    $query = atknew($name);
    $query->addTable('scheduler_scheduler');
    $query->addJoin('scheduler_attendees', '', 'scheduler_attendees.scheduler_id=scheduler_scheduler.id', TRUE);
    $query->addJoin('person', '', 'person.id=scheduler_attendees.person_id', TRUE);
    $query->addJoin('person', 'owner', 'owner.id=scheduler_scheduler.owner', TRUE);
    $query->addJoin('scheduler_dates','','scheduler_dates.scheduler_id=scheduler_scheduler.id',TRUE);
    $query->addField('startdate',' ','scheduler_dates');
    $query->addField('enddate',' ','scheduler_dates');

    $tmp_enddate = mktime(0,0,0,substr($startdate,5,2),substr($startdate,8,2),substr($startdate,0,4));
    $tmp_startdate = mktime(23,59,59,substr($enddate,5,2),substr($enddate,8,2),substr($enddate,0,4));

    $condition = "$tmp_enddate < scheduler_dates.enddate AND scheduler_dates.startdate < $tmp_startdate";
    if(count($user)>0 && !in_array('all', $user))
    {
      $useridcondition = (count($user)>0) ? " OR scheduler_scheduler.owner IN ('".implode("','", $user)."') OR scheduler_attendees.person_id IN ('".implode("','", $user)."')" : "";
      $condition.=" AND (scheduler_scheduler.all_users = '1' $useridcondition)";
    }
    $query->addCondition($condition);


    $querystring = $query->buildSelect(TRUE);
    $db = &atkGetDb();
    $nrows = $db->getrows($querystring);

    $dates = array();
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $loop_enddate = date("Ymd",$nrows[$i]["enddate"]);
      for($j=0;date("Ymd",($nrows[$i]["startdate"]+($j*86400)))<=$loop_enddate;$j++)
      {
        $dates[date("Y-m-d",($nrows[$i]["startdate"]+($j*86400)))]=true;
      }
    }
    //todo: Should we add holidays to the small scheduler ?
    //$this->addHolidays(true,$dates,$startdate,$enddate);
    $up_node = atknew("scheduler.scheduler_userpreferences");
    $userprefs = $up_node->getUserPreferences();
    if($userprefs["showemployeebirthdays"])
      $this->addEmployeeBirthdays(true,$dates,date("n",$tmp_startdate),date("n",$tmp_enddate));
    if($userprefs["showtodo"])
      $this->addTodos(true,$dates,$startdate,$enddate);
    return $dates;
  }

 /**
  * Get scheduler items
  *
  * @param string $startdate Startdate
  * @param string $enddate Enddate
  * @param array $user Array with user id's
  */
 function getItems($startdate="",$enddate="",$user=array())
  {
    $db = &atkGetDb();

    if(count($user)==0)
    {
      $user = $this->m_viewUsers;
    }

    $this->m_alldayItems = array();
    $this->m_appointmentItems = array();
    if($startdate=="")
    {
       // Set startdate and enddate with viewdate since it's not
       // possible to call this function with only an enddate.
       $startdate=$this->m_viewdate;
       $enddate=$this->m_viewdate;
    }
    $this->updateRecurringEvents($startdate,$enddate);
    $tmp_array=array();
    $this->addHolidays(false,$tmp_array,$startdate,$enddate);

    $query = &$db->createQuery();
    $query->addTable('scheduler_scheduler');
    $query->addJoin('scheduler_attendees', '', 'scheduler_attendees.scheduler_id=scheduler_scheduler.id', TRUE);
    $query->addJoin('person', '', 'person.id=scheduler_attendees.person_id', TRUE);
    $query->addJoin('person', 'owner', 'owner.id=scheduler_scheduler.owner', TRUE);
    $query->addJoin('scheduler_dates','','scheduler_dates.scheduler_id=scheduler_scheduler.id',TRUE);
    $query->addField('id', ' ', 'scheduler_scheduler');
    $query->addField('startdate as itemstartdate', '', 'scheduler_scheduler');
    $query->addField('enddate as itemenddate', '', 'scheduler_scheduler');
    $query->addField('starttime', ' ', 'scheduler_scheduler');
    $query->addField('endtime', ' ', 'scheduler_scheduler');

    $query->addField('title', ' ', 'scheduler_scheduler');
    $query->addField('description', ' ', 'scheduler_scheduler');
    $query->addField('location', ' ', 'scheduler_scheduler');
    $query->addField('allday', ' ', 'scheduler_scheduler');
    $query->addField('private', ' ', 'scheduler_scheduler');
    $query->addField('owner', ' ', 'scheduler_scheduler');
    $query->addField('all_users', ' ', 'scheduler_scheduler');
    $query->addField('category', ' ', 'scheduler_scheduler');
    $query->addField('recur', ' ', 'scheduler_scheduler');
    $query->addField('startdate',' ','scheduler_dates');
    $query->addField('enddate',' ','scheduler_dates');

    $tmp_enddate = mktime(0,0,0,substr($startdate,5,2),substr($startdate,8,2),substr($startdate,0,4));
    $tmp_startdate = mktime(23,59,59,substr($enddate,5,2),substr($enddate,8,2),substr($enddate,0,4));

    $condition = "$tmp_enddate < scheduler_dates.enddate AND scheduler_dates.startdate < $tmp_startdate";

    if(count($user)>0 && !in_array('all', $user))
    {
      $useridcondition = (count($user)>0) ? " OR scheduler_scheduler.owner IN ('".implode("','", $user)."') OR scheduler_attendees.person_id IN ('".implode("','", $user)."')" : "";
      $condition.=" AND (scheduler_scheduler.all_users = '1' $useridcondition)";
    }

    $query->addCondition($condition);
    $query->addOrderBy('starttime');


    $querystring = $query->buildSelect(TRUE);
    $nrows = $db->getrows($querystring);
    $items = array();
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $items[] = $nrows[$i]["id"];
      $check_startdate = date("Ymd",$nrows[$i]["startdate"]);
      $check_enddate = date("Ymd",$nrows[$i]["enddate"]);

      for($j=0;date("Ymd",($nrows[$i]["startdate"]+($j*86400)))<=$check_enddate;$j++)
      {
        $currenttime = $nrows[$i]["startdate"]+($j*86400);
        /** @todo Optimize for week and month */
        if($this->m_view=="day" && date("Y-m-d",$currenttime)!=$this->m_viewdate) continue;
        $currentdate = date("Ymd",$currenttime);
        $tmp_nrow = $nrows[$i];

        if($nrows[$i]["allday"]==1 || ($check_startdate!=$currentdate && $check_enddate!=$currentdate))
        {
          if($tmp_nrow["allday"]==0) $tmp_nrow["allday"]=1;
          $this->m_allDayItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
        }
        else
        {
          if($check_startdate==$currentdate && $check_startdate!=$check_enddate)
          {
            $tmp_nrow["enddate"] = mktime(23,59,00,date("m",$currenttime),date("d",$currenttime),date("Y",$currenttime));
            $tmp_nrow["endtime"] = "23:59:00";
          }
          elseif($check_enddate==$currentdate && $check_startdate!=$check_enddate)
          {
            $tmp_nrow["startdate"] = mktime(0,0,0,date("m",$currenttime),date("d",$currenttime),date("Y",$currenttime));
            $tmp_nrow["starttime"] = "00:00:00";
          }
          //$this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
          if(($this->m_view=="day" || $this->m_view=="employee") && date("Y-m-d",$currenttime)==$this->m_viewdate)
          {
            $this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
            $this->_setMatrix($tmp_nrow);
          }
          elseif($this->m_view!="day" && $this->m_view!="employee" )
          {
            $this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
          }
        }
      }
    }
    $this->getAttendees($items);
    $up_node = atknew("modules.scheduler.scheduler_userpreferences");
    $userprefs = $up_node->getUserPreferences();
    if($userprefs["showemployeebirthdays"])
    {
      $tmp_array = array();
      $this->addEmployeeBirthdays(false,$tmp_array);
    }
    if($userprefs["showtodo"])
    {
      $tmp_array=array();
      $this->addTodos(false,$tmp_array,$startdate,$enddate);
    }

  }

  /**
   * Add holidays to scheduler items
   * @param boolean $compact Compact mode or not (for the small calendar)
   * @param array $dates Array with the small calendar dates
   * @param integer $startmonth Start month of the small calendar
   * @param integer $endmonth End month of the small calendar
   */
   function addHolidays($compact=false,&$dates,$startdate="",$enddate="")
   {
     $holiday = &getNode("scheduler.scheduler_holidays");
     if($startdate=="") { $startdate=time(); } else { $startdate = str_replace("-","",$startdate); }
     if($enddate=="") { $enddate=date("Ymd"); }  else { $enddate = str_replace("-","",$enddate); }

     $startdate = mktime(12,0,0,intval(substr($startdate,4,2)),intval(substr($startdate,6,2)),intval(substr($startdate,0,4)));
     for($i=0;date("Ymd",($startdate+($i*86400)))<=$enddate;$i++)
     {
       $date = date("Y-m-d",($startdate+($i*86400)));
       if($holiday->isHoliday($date))
       {
         if($compact)
         {
           $dates[$date]=true;
         }
         else
         {
           $holidaysInfo = $holiday->getHolidayInfo($date);
           foreach($holidaysInfo as $info)
           {
             $this->m_allDayItems[$date][] = array("id"=>NULL,
                                                "itemstartdate"=>$date,
                                                "itemenddate"=>$date,
                                                "starttime"=>"00:00:00",
                                                "endtime"=>"23:59:59",
                                                "allday"=>1,
                                                "all_users"=>1,
                                                "recur"=>"once",
                                                "startdate"=>mktime(0,0,0,$i,$day,$this->m_year),
                                                "enddate"=>mktime(23,59,59,$i,$day,$this->m_year),
                                                "title"=>$info["name"],
                                                "category"=>$info["schedulercategory"]);
           }
         }
       }

     }
   }

  /**
   * Add employee birthdates to scheduler items
   *
   * @param boolean $compact Compact mode or not (for the small calendar)
   * @param array $dates Array with the small calendar dates
   * @param integer $startmonth Start month of the small calendar
   * @param integer $endmonth End month of the small calendar
   */
  function addEmployeeBirthdays($compact=false,&$dates,$startmonth=1,$endmonth=12)
  {
    $employees = &atkGetNode("employee.employee");
    $birthdays = $employees->getBirthdates();
    if(!$compact)
    {
      $startmonth=intval($this->m_month);
      $endmonth=intval($this->m_month);
    }
    for($i=$startmonth;$i<=$endmonth;$i++)
    {
      if(isset($birthdays[$i]) && is_array($birthdays[$i]))
      {
        foreach($birthdays[$i] as $day => $names)
        {
          foreach($names as $name)
          {
            atk_var_dump($name,"NAME BIRTHDAY");
            $date = date("Y-m-d",mktime(0,0,0,$i,$day,$this->m_year));
            if($compact)
            {
              $dates[$date]=true;
            }
            else
            {
              $this->m_allDayItems[$date][] = array("id"=>NULL,
                                                  "itemstartdate"=>$date,
                                                  "itemenddate"=>$date,
                                                  "starttime"=>"00:00:00",
                                                  "endtime"=>"23:59:59",
                                                  "allday"=>1,
                                                  "recur"=>"once",
                                                  "startdate"=>mktime(0,0,0,$i,$day,$this->m_year),
                                                  "enddate"=>mktime(23,59,59,$i,$day,$this->m_year),
                                                  "title"=>$name["name"]." ".atktext("birthday"));
            }
          }
        }
      }
    }
  }


  /**
   * Add todo's to the events list
   *
   * @param bool $compact Small calendor or not
   * @param array $dates Array with dates for the compact mode
   * @param string $startdate Start date
   * @param string $enddate End date
   */
  function addTodos($compact=false,&$dates,$startdate,$enddate)
  {
    $todo_node = &atkGetNode("todo.todo");
    $filter="duedate >= '".$startdate."' AND duedate<='".$enddate."' ";
    $filter.='AND todo.status NOT IN (5,2)';
    $todos = $todo_node->selectDb($filter,"","","",array("id","title","duedate"));
    foreach($todos as $todo)
    {

      $date = date("Y-m-d",mktime(0,0,0,$todo["duedate"]["month"],$todo["duedate"]["day"],$todo["duedate"]["year"]));
      if($compact)
      {
        $dates[$date]=true;
      }
      else
      {
        $this->m_allDayItems[$date][] = array("id"=>NULL,
                                            "itemstartdate"=>$date,
                                            "itemenddate"=>$date,
                                            "starttime"=>"00:00:00",
                                            "endtime"=>"23:59:59",
                                            "allday"=>1,
                                            "recur"=>"once",
                                            "startdate"=>mktime(0,0,0,$i,$day,$this->m_year),
                                            "enddate"=>mktime(23,59,59,$i,$day,$this->m_year),
                                            "title"=>atktext("todo_duedate")." ".$todo["title"]);
      }

    }


  }


  /**
   * Get attendees of a schedule item
   * @param int $scheduleid Schedule id
   * @return array Array with attendees of the schedule item
   */
  function getAttendees($items)
  {
    if(count($items)==0) return false;
    $db = &atkgetdb();

    $name = "atk".atkconfig("database")."query";
    $query = new $name();
    $query->addTable('scheduler_attendees');
    $query->addJoin('person', '', 'person.id=scheduler_attendees.person_id', TRUE);

    $query->addField('id', ' ', 'person');
    $query->addField('scheduler_id', ' ', 'scheduler_attendees');

    $query->addField('lastname', ' ', 'person');
    $query->addField('firstname', ' ', 'person');
    $query->addField('role', ' ', 'person');
    $query->addCondition("scheduler_attendees.scheduler_id IN (".implode(",",$items).")");

    $querystring = $query->buildSelect(TRUE);
    $nrows = $db->getRows($querystring);
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $this->m_attendees[$nrows[$i]["scheduler_id"]][] = $nrows[$i]["id"];
      if(!array_key_exists($nrows[$i]["id"],$this->m_users))
      {
        $this->m_users[$nrows[$i]["id"]] = array("firstname"=>$nrows[$i]["firstname"],
                                                 "lastname"=>$nrows[$i]["lastname"],
                                                 "role"=>$nrows[$i]["role"]
                                                );
      }
    }
    unset($nrows);
    return true;
  }

  /**
   * Is a calendar item recurring
   *
   * @param array $item Calendar item
   * @return bool Is recurring
   */
  function isRecurring($item)
  {
    return ($item["recur"]!="once");
  }

  /**
   * Is a calendar item all day
   *
   * @param array $item Calendar item
   * @return bool Is all day
   */
  function isAllDay($item)
  {
    return ($item["allday"]==1);
  }

  /**
   * Is a calendar item private
   *
   * @param array $item Calendar item
   * @return bool Is private
   */
  function isPrivate($item)
  {
    return ($item["private"]==1);
  }

  /**
   * Has a calendar item an alarm
   *
   * @param array $item Calendar item
   * @return bool Has an alarm
   */
  function hasAlarm($item)
  {
    return (array_key_exists('reminder', $item) && $item["reminder"]==1);
  }

  /**
   * Has a calendar item an description
   *
   * @param array $item Calendar item
   * @return bool Has an description
   */
  function hasDescription($item)
  {
    return ($item["description"]!="");
  }


 /**
  * Renders a calendar items
  * @param array $item Calendar item
  * @param bool $admin Show admin links or not
  * @return string HTML code with a renderd calendar item
  */
  function renderItem($item,$admin=false,$itemDate="",$showcategory=false)
  {
    $theme = &atktheme::getInstance();
    $user = getUser();

    if(!is_array($item)) return "";

    if($itemDate=="") $itemDate=$this->m_viewdate;
    $tmp="";
    if($admin)
    {
      $tmp .= '<table class="scheduleritem" border="0" cellpadding="0" cellspacing="0"><tr>'."\n";
      $tmp .= '<td valign="top" align="left">';
    }
    // If showcategory we will show a box with the category color
    if($showcategory)
    {
      if(isset($item["category"]))
        $tmp.='<img style="background-color: '.$this->m_itemColors[$item["category"]]["back"].';" src="images/trans.gif" border="1" width="10" height="10" title="'.$this->m_itemColors[$item["category"]]["desc"].'">&nbsp;';
    }

    $isAttendee = false;
    // Get Attendees of the Item
    if ($item["all_users"])
    {
      $names=atkText("allusers","scheduler");
      $attendees='<img src="'.$theme->imgPath("attendees.gif","scheduler").'" width=15 height=15 title="'.$names.'" alt="'.$names.'">';
    }
    else
    {
      // Also check if the current user is an attendee
      //$attendees_rows = $this->getAttendees($item["id"]);
      $numberofattendees = count($this->m_attendees[$item["id"]]);
      atk_var_dump($this->m_attendees,"ATTENDEES ".$item["id"]);
      $names = "";
      for($i=0;$i<$numberofattendees;$i++)
      {

        $userid = $this->m_attendees[$item["id"]][$i];
        if($userid==$user["id"]) $isAttendee=true;
        if($i>0) $names.=", ";
        $names.= $this->m_users[$userid]["firstname"]." ".$this->m_users[$userid]["lastname"].($this->m_users[$userid]["role"]!="employee"?" (".$this->m_users[$userid]["role"].")":"");
      }
      /*
      if($numberofattendees==1)
      {
        $attendees='<img src="'.$theme->imgPath("single.png","scheduler").'" width=15 height=15 title="'.$names.'" alt="'.$names.'">';
      }
      else
      {
        $attendees='<img src="'.$theme->imgPath("multi.png","scheduler").'" width=15 height=15 title="'.$names.'" alt="'.$names.'">';
      }
      */
      $attendees='<img src="'.$theme->imgPath("attendees.gif","scheduler").'" width=15 height=15 title="'.$names.'" alt="'.$names.'">';
    }

    if(!$this->isAllDay($item))
    {
      $tmp.=substr($item["starttime"],0,5)." ".substr($item["endtime"],0,5)." ";
    }

    if($this->hasDescription($item))
       $tmp.='<img src="'.$theme->imgPath("note.gif","scheduler").'" width=15 height=15 title="'.atktext("note").'" alt="'.atktext("note").'">';
    if($this->isRecurring($item))
      $tmp.='<img src="'.$theme->imgPath("recurring.gif","scheduler").'" width=15 height=15 title="'.atkText("recurring_event","scheduler").'" alt="'.atkText("recurring_event","scheduler").'">';
    if($this->hasAlarm($item))
      $tmp.='<img src="'.$theme->imgPath("alarm.gif","scheduler").'" width=15 height=15 title="'.atkText("reminder_event","scheduler").'" alt="'.atkText("reminder_event","scheduler").'">';
    if($this->isPrivate($item))
      $tmp.='<img src="'.$theme->imgPath("private.gif","scheduler").'" width=15 height=15 title="'.atkText("private_event").'" alt="'.atkText("private_event","scheduler").'">';
    $tmp.=$attendees;

    $tmp.="&nbsp;";

    /*
    owner = view private, edit, delete, view
    attendee = view private, view
    assistant = edit, delete, view
    */

    // If item is private and current is not an attendee or owner, show private event.
    if($this->isPrivate($item) && !($item["owner"]==$user["id"] || $isAttendee))
    {
      $tmp.=atkText("private_event","scheduler");
    }
    else
    {
      $action = "";
      if(($item["owner"]==$user["id"]) ||
         (!$this->isPrivate($item) && in_array($item["owner"],$this->m_assistantFor)))
      {
        $action = ($this->isRecurring($item)?"editserieorsingle":"edit");
      }
      elseif($isAttendee)
      {
        $action = "view";
      }

      if($action!="")
      {
        $tmp.=href(dispatch_url("scheduler.scheduler","view",array("atkselector"=>"scheduler_scheduler.id=".$item["id"],"scheduler_id"=>$item["id"],"itemdate"=>$item["startdate"])),
                   $item["title"],SESSION_NESTED);
      }
      else
      {
        $tmp.=$item["title"];
      }
      if($item["location"]!="")
        $tmp.=" (".$item["location"].")";
    }
    $tmp.="<br>";
    // Add Admin links
    if($admin)
    {
      $tmp.='</td><td valign="top" align="right">';
      // you may only delete a calendar item if you are the owner,
      // or an assistant of the owner and the item is not private
      if ($item["owner"] == $user["id"] || (!$this->isPrivate($item) && in_array($item["owner"],$this->m_assistantFor)))
      {
        $delete = '<img src="'.$theme->iconPath("delete", "recordlist").'" border="0">';
        $tmp.= href(dispatch_url("scheduler.scheduler","delete",array("atkselector"=>"scheduler_scheduler.id=".$item["id"],"scheduler_id"=>$item["id"],"itemdate"=>$item["startdate"])),
                    $delete, SESSION_NESTED);
      }
      $tmp.='</td></tr></table>';
    }

    return $tmp;
  }

  /**
   * Get the usersettings, and sets them
   */
  function _setUserSettings()
  {
    $user = getUser();
    $schedulerNode = &getNode("scheduler.scheduler");
    $userprefs = $schedulerNode->getUserSchedulerPrefs($user["id"]);

    if($userprefs["timeschedule"]!="") $this->m_scheduletime = $userprefs["timeschedule"];
    if($userprefs["scheduler_emptycolor"]!="") $this->m_emptyColor = $userprefs["scheduler_emptycolor"];
    if($userprefs["scheduler_emptyworkhourscolor"]!="") $this->m_emptyWorkHoursColor = $userprefs["scheduler_emptyworkhourscolor"];
    if($userprefs["showweeknumbers"]!="") $this->m_showWeekNumbers = $userprefs["showweeknumbers"];
    if($userprefs["showlunarphases"]!="") $this->m_showLunarPhases = $userprefs["showlunarphases"];
    if($userprefs["showtodo"]!="") $this->m_showTodo = $userprefs["showtodo"];
    // This is a general user setting
    if($user["startday"]!="") $this->setCalendarStartDay($user["startday"]);
    // These can be found in the current user contract so get the current valid contract
    $sql = "SELECT workstarttime,workendtime,WorkingDays FROM usercontract
            WHERE userid = ".($user["id"]!=""?$user["id"]:"-1")."
              AND startdate <= '".$this->m_viewdate."'
              AND (enddate >= '".$this->m_viewdate."'
                   OR enddate IS NULL)";
    $db = &atkGetDb();
    $row = $db->getrows($sql);
    if(count($row)==1)
    {
      if($row[0]["workstarttime"]!="00:00:00") $this->m_work_starttime = substr($row[0]["workstarttime"],0,5);
      if($row[0]["workendtime"]!="00:00:00")$this->m_work_endtime = substr($row[0]["workendtime"],0,5);
      if($row[0]["WorkingDays"]!="")
      {
        $this->m_workdays=0;
        $days = explode("|",$row[0]["WorkingDays"]);
        foreach($days as $day)
        {
          $this->m_workdays |= pow(2,$day);
        }
      }
    }
    else
    {
      $this->m_work_starttime="08:30";
      $this->m_work_endtime = "17:00";
      $this->m_workdays=0;
    }
    // Check if current user is assistant for other users
    $sql = "SELECT userid FROM scheduler_userassistants WHERE employeeid='{$user["id"]}'";
    $nrows = $db->getrows($sql);
    for($i=0;$i<count($nrows);$i++)
    {
      $this->m_assistantFor[] = $nrows[$i]["userid"];
    }
    $this->m_assistantFor = schedulertools::assistantFor($user["id"]);

    $this->m_owner = $user["id"];
  }

  /**
   * Corrects times to the timeschedule
   * @param string $time Time to be corrected
   * @param string $round Round the time up or down
   * @return string Corrected time
   */
  function _correct_time($time,$round="down")
  {
    $tmp_min = substr($time,3,2);

    if($round=="down")
    {
      $minutes = sprintf("%02d",floor($tmp_min/$this->m_scheduletime)*$this->m_scheduletime);
    }
    else
    {
      $minutes = sprintf("%02d",ceil($tmp_min/$this->m_scheduletime)*$this->m_scheduletime);
    }
    if($minutes==60)
    {
      return sprintf("%02d",substr($time,0,2)+1).":00";
    }
    else
    {
      return substr($time,0,2).":".$minutes;
    }
  }

  /**
   * Render the view
   * @return string A rendered view
   */
  function renderView()
  {
  }

}

?>