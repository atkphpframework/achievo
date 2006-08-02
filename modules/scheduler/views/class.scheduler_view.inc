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
  var $m_viewUser = "";

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

  var $m_weekday_arr = array(1,2,4,8,16,32,64);
  var $m_workdays = 0;
  var $m_showWeekNumbers = 0;

 /**
  * @var string $m_emptyWorkHoursColor Default color for empty workhours calendar items
  */
  var $m_emptyWorkHoursColor = "#FFFFFF";

 /**
  * @var string $m_emptyColor Default color for empty calendar items
  */
  var $m_emptyColor   = "#DDDDDD";

  var $m_showTodo = 0;
  var $m_employeeLookup = array();

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
  * Set view user
  * @param string $user Current user
  */
  function setViewUser($user)
  {
     $this->m_viewUser = $user;
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
  * Returns the default view header
  * @return string default view header
  */
  function _view_header($selector=true)
  {
    $view_header = href($_SERVER["PHP_SELF"]."?atkaction=add&atknodetype=scheduler.scheduler",atkText("additem","scheduler"))."&nbsp;";
    $view_header.= href($_SERVER["PHP_SELF"]."?view=day&viewdate=".$this->m_viewdate."&user=".$this->m_viewUser,atkText("dayview","scheduler"))."&nbsp;";
    $view_header.= href($_SERVER["PHP_SELF"]."?view=week&viewdate=".$this->m_viewdate."&user=".$this->m_viewUser,atkText("weekview","scheduler"))."&nbsp;";
    $view_header.= href($_SERVER["PHP_SELF"]."?view=month&viewdate=".$this->m_viewdate."&user=".$this->m_viewUser, atkText("monthview","scheduler"))."&nbsp;";
    //$view_header.= href($_SERVER["PHP_SELF"]."?view=year&viewdate=".$this->m_viewdate."&user=".$this->m_viewUser, atkText("yearview","scheduler"))."&nbsp;";
    //$view_header.= href($_SERVER["PHP_SELF"]."?view=employee&viewdate=".$this->m_viewdate, atkText("employeeview","scheduler"))."&nbsp;";
    $view_header.= href($_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&user=".$this->m_viewUser, atkText("refreshview","scheduler"));
    $view_header.= "<br><br><table width='90%'>";
    if($selector)
    {
      $view_header.= "<tr>";
      $view_header.= "<td valign='top' align='left'>".$this->_getNav()."</td>";
      $view_header.= "<td valign='top' align='right'>".$this->_getSelector()."</td>";
      $view_header.= "</tr>";
    }
    $view_header.= "<tr><td valign='top' align='left'><b>".$this->m_viewTitle."</b></td>";
    $view_header.= "</tr></table>";
    return $view_header;
  }

 /**
  * Creates a small calendar with month navigation
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

    $days = array();
    $days[0] = atkText("sun","atk");
    $days[1] = atkText("mon","atk");
    $days[2] = atkText("tue","atk");
    $days[3] = atkText("wed","atk");
    $days[4] = atkText("thu","atk");
    $days[5] = atkText("fri","atk");
    $days[6] = atkText("sat","atk");
    // On what day does our calendar start (0 or 1)



    atkimport("modules.utils.dateutil");
    $months = dateutil::monthlist();
    for($i=1; $i<=12; $i++)
      $months[$i] = atkText($months[$i], "atk");


    $days_in_month = mktime (0,0,0,$month+1,0,$year);
    $days_in_month = (strftime ("%d",$days_in_month));

    $first_day_of_month = date("w", mktime(0,0,0,$month,1,$year));

    $previous_year = date("Y", mktime(0,0,0,$month-1,1,$year));
    $previous_month= date("m", mktime(0,0,0,$month-1,1,$year));
    $prevTime = date("Y-m-d", mktime(0,0,0,$month-1, 1, $year));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&small_viewdate=$prevTime&user=".$this->m_viewUser;
    $next_day = 1;
    $next_year = date("Y", mktime(0,0,0,$month+1,1,$year));
    $next_month= date("m", mktime(0,0,0,$month+1,1,$year));

    $nextTime = date("Y-m-d", mktime(0,0,0,$month+1, 1, $year));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&small_viewdate=$nextTime&user=".$this->m_viewUser;

    /*
    $tmp = '<h3 align="center">';
    if($monthNav) $tmp.= href($prevUrl,"&lt&lt");
    $tmp.= " ".$months[$month]." $year ";
    if($monthNav) $tmp.= href($nextUrl,"&gt;&gt;");
    */
    $tmp ='<table border="0" cellpadding="1" cellspacing="0">';
    $tmp.='<tr style="background-color: #dddddd">';
    $tmp.='<td align="left">';
    if($monthNav) $tmp.= href($prevUrl,"&lt&lt");
    $tmp.='</td>';
    $tmp.='<td align="center" colspan="'.($this->m_showWeekNumbers?6:5).'">';
    $tmp.= $months[$month].' '.$year;
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
    atkdebug("Weeknumber: $month - 1 - $year = ".date("W", mktime(12,0,0,$month,1,$year)));
    $week_number = date("W", mktime(12,0,0,$month,1,$year));
    $weekTime = date("Y-m-d",mktime(12,0,0,$month,1,$year));
    $weekUrl = $_SERVER["PHP_SELF"]."?view=week&viewdate=$weekTime&user=".$this->m_viewUser;
    if($this->m_showWeekNumbers==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
    for ($i=1;$i<=$cellcount;$i++)
    {
    	$tmp.='<td>&nbsp;</td>';
    }
    $holiday = &getNode("scheduler.scheduler_holidays");
    for ($i=1;$i<=$days_in_month;$i++)
    {
    	if ($cellcount == 7)
    	{
    		$tmp.="</tr><tr>\n";
    		$week_number++;
        $weekTime = date("Y-m-d",mktime(12,0,0,$month,$i,$year));
        $week_number = date("W", mktime(12,0,0,$month,$i,$year));
        $weekUrl = $_SERVER["PHP_SELF"]."?view=week&viewdate=$weekTime&user=".$this->m_viewUser;
        if($this->m_showWeekNumbers==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
    		$cellcount=0;
    	}
    	$cellcount++;
    	if (($i == date("d")) && ($year == date("Y")) && ($month==date("m")))
    		$class="calendarToday";
    	else
    		$class="calendarNormal";

      $dayTime = date("Y-m-d",mktime(12,0,0,$month,$i,$year));

    	$holiday_title = '';
    	if($holiday->isHoliday($dayTime))
    	{
    	    $holidayInfo = $holiday->getHolidayInfo($dayTime);
    			$holiday_title = ' title="'.$holidayInfo["name"].'"';
    			if($class=="calendarToday") { $class="calendarHolidayToday"; } else { $class="calendarHoliday"; }
    	}
    	$dayUrl = $_SERVER["PHP_SELF"]."?view=day&viewdate=$dayTime&user=".$this->m_viewUser;
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
    	$tmp.='<td align="right" bgcolor="'.$bgcolor.'">'.href($dayUrl,$daynumber,'','','class="'.$class.'"'.$holiday_title).'</td>';
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
  * Creates a html form with a date/user selector
  * @return string html form with a date/user selector
  */
  function _getSelector()
  {
    $form  = '<form method="get" action="'.$_SERVER["PHP_SELF"].'">'."\n";
    $form .= session_form();
    $form .= '<input type="hidden" name="view" value="'.$this->m_view.'">';
    $dummy_rec = array("viewdate"=>$this->_getInitialDate());
    $datebox = new atkDateAttribute("viewdate","F d Y","d F Y",0,0,AF_OBLIGATORY);
    $form .=$datebox->edit($dummy_rec);
    $form.="&nbsp;&nbsp;";
    $form .= $this->_getEmployees();
    $form .= '&nbsp;<input type="submit" value="'.atkText("refresh","scheduler").'">'."\n";
    $form .= '</form>'."\n";
    return $form;
  }

 /**
  * Get a Employees select box or checkboxes
  * @param array $user_id Userid
  * @param string $form_type Selector (select,default) or Checkboxes (checkbox)
  * @return string User selector / checkboxes
  */
  function _getEmployees($default=array(),$form_type="select",$show_all=true)
  {
    $g_db = &atkgetdb();
    if(count($default)==0) $default[] = $this->m_viewUser;

    $sql = "SELECT lastname,firstname,id
            FROM person
            WHERE status='active' AND role='employee'
            ORDER BY lastname
           ";

    $records = $g_db->getrows($sql);
    if($show_all)
    {
      if($form_type=="select")
      {
        $employee_code='<select name="user"><option value="all">'.atkText("allusers","scheduler");
      }
      else
      {
        if (in_array("all",$default)) { $sel="checked"; } else { $sel=""; }
        $employee_code='<input type="checkbox" value="all" name="employee[]" '.$sel.'>'.atkText("allusers","scheduler").'<br>';
      }
    }
    for($i=0;$i<count($records);$i++)
    {
      if($this->m_view=="employee") $this->m_employeeLookup[$records[$i]["id"]] = $records[$i]["lastname"].",<br>".$records[$i]["firstname"];
      if(in_array($records[$i]["id"],$default)) { $sel=($form_type=="select"?"selected":"checked"); } else { $sel=""; }
      if($form_type=="select")
      {
        $employee_code.='<option value="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</option>';
      }
      else
      {

        $employee_code.='<input type="checkbox" value="'.$records[$i]["id"].'"  name="employee[]" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"]."(".$records[$i]["userid"].")<br>";
      }
    }
    if($form_type=="select") $employee_code.="</select>";
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


    $legend  = '<table border="0" cellPadding="1">
                <tbody>
                <tr>
                  <td bgColor="#000000">
                    <TABLE width="100%" border="0" cellPadding="2" cellSpacing="0">
                    <tbody>';

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

  function &getItemsQuery($startdate="",$enddate="",$user=array())
  {
    $name = "atk.db.atk".atkconfig("database")."query";
    $query = &atkNew($name);
    $query->addTable('scheduler_scheduler');
    $query->addJoin('scheduler_attendees', '', 'scheduler_attendees.schedule_id=scheduler_scheduler.id', TRUE);
//$query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);
//$query->addJoin('person', 'owner', 'owner.id=schedule.owner', TRUE);
    $query->addField('id', ' ', 'scheduler_scheduler');
    $query->addField('startdate', ' ', 'scheduler_scheduler');
    $query->addField('enddate', ' ', 'scheduler_scheduler');
    $query->addField('starttime', ' ', 'scheduler_scheduler');
    $query->addField('endtime', ' ', 'scheduler_scheduler');
    $query->addField('startmonth', ' ', 'scheduler_scheduler');
    $query->addField('endmonth', ' ', 'scheduler_scheduler');
    $query->addField('title', ' ', 'scheduler_scheduler');
    $query->addField('description', ' ', 'scheduler_scheduler');
    $query->addField('location', ' ', 'scheduler_scheduler');
    $query->addField('allday', ' ', 'scheduler_scheduler');
    $query->addField('private', ' ', 'scheduler_scheduler');
    $query->addField('owner', ' ', 'scheduler_scheduler');
//$query->addField('status', ' ', 'scheduler_scheduler');
    $query->addField('all_users', ' ', 'scheduler_scheduler');
    $query->addField('category', ' ', 'scheduler_scheduler');
    $query->addField('recur', ' ', 'scheduler_scheduler');
    $query->addField('every', ' ', 'scheduler_scheduler');
    $query->addField('cyclus_startdate', ' ', 'scheduler_scheduler');
    $query->addField('cyclus_enddate', ' ', 'scheduler_scheduler');
    $query->addField('weekday', ' ', 'scheduler_scheduler');
    $query->addField('startday', ' ', 'scheduler_scheduler');
    $query->addField('endday', ' ', 'scheduler_scheduler');
    $query->addField('month_time', ' ', 'scheduler_scheduler');
//$query->addField('userid', '', 'owner', 'owneruser_AE_');
    $query->addCondition("(
                            (
                              scheduler_scheduler.startdate ".($this->m_view=="day"?"<=":">=")." '$startdate'
                              AND scheduler_scheduler.enddate ".($this->m_view=="day"?">=":"<=")." '$enddate'
                              AND scheduler_scheduler.recur='once'
                            )
                            OR
                            (
                              scheduler_scheduler.recur<>'once'
                            )
                          )
                          AND
                          (
                            scheduler_scheduler.cyclus_enddate is null
                            OR scheduler_scheduler.cyclus_enddate >= '$enddate'
                            OR scheduler_scheduler.cyclus_enddate BETWEEN '$startdate' AND '$enddate'
                          )
                          AND
                          (
                            scheduler_scheduler.cyclus_startdate is null
                            OR scheduler_scheduler.cyclus_startdate <= '$startdate'
                            OR scheduler_scheduler.cyclus_startdate BETWEEN '$startdate' AND '$enddate'
                          )
                        ");
    if(count($user)>0)
    {
      $useridcondition = (count($user)>0) ? " OR (scheduler_scheduler.owner IN ('".implode("','", $user)."') OR scheduler_attendees.person_id IN ('".implode("','", $user)."')" : "";
      $query->addCondition("scheduler_scheduler.all_users = '1' $useridcondition)");
    }
    $query->addOrderBy('starttime');
    return $query;
  }

  function isItemVisibleOnDate($scheduleritem, $date)
  {
    // Initialize some variables
    if (is_string($date))
      $starttime = mktime(12,0,0,substr($startdate,5,2),substr($startdate,8,2)+$datecounter,substr($startdate,0,4));
    else
      $starttime = $date;
    $unix_date = $starttime;
    $month_time = ceil(date("d",$starttime)/7);
    $day = date("j",$starttime);
    $month = date("n",$starttime);
    $year = date("Y",$starttime);
    $weekday_number = date("w",mktime(12,0,0,$month,$day,$year));
    $weekday = $this->weekdaynumbertoflag($weekday_number);

    // Initially we assume the item to be visible
    $show_item = true;

    // Check weekday / monthtime / day / day+month
    switch($scheduleritem["recur"])
    {
      case 'once':
        if(date("Y-m-d",$starttime)>=$scheduleritem["startdate"] && date("Y-m-d",$starttime)<=$scheduleritem["enddate"]) { $show_item = true; } else { $show_item = false; }
        break;
      case 'weekly':
        if(hasFlag($scheduleritem["weekday"],$weekday)) { $show_item = true; } else { $show_item = false; }
      break;
      case 'monthly_date':
        if($scheduleritem["startday"]<=$day && $scheduleritem["endday"]>=$day)  { $show_item = true; } else { $show_item = false; }
      break;
      case 'monthly_day':
        if(hasFlag($scheduleritem["weekday"],$weekday) && $scheduleritem["month_time"]==$month_time) { $show_item = true; } else { $show_item = false; }
      break;
      case 'yearly_date':
        if(($scheduleritem["startday"]<=$day && $scheduleritem["endday"]>=$day) && ($scheduleritem["startmonth"]<=$month && $scheduleritem["endmonth"]>=$month))  { $show_item = true; } else { $show_item = false; }
      break;
      case 'yearly_day':
        if($scheduleritem["startmonth"]==$month && hasFlag($scheduleritem["weekday"],$weekday) && $scheduleritem["month_time"]==$month_time) { $show_item = true; } else { $show_item = false; }
      break;
    }

    // Check the every ... (days,weeks,months,years)
    if($scheduleritem["every"]>=1 && $show_item==true)
    {
 			if (!empty($scheduleritem["cyclus_startdate"]))
 			{
 			  $cyclus_startdate_array = explode('-',$scheduleritem["cyclus_startdate"]);
			  $cyclus_startdate_time = mktime(12,0,0,intval($cyclus_startdate_array[1]),intval($cyclus_startdate_array[2]),intval($cyclus_startdate_array[0]));
 			}
      switch($scheduleritem["recur"])
      {
        case 'daily':
          $interval = $unix_date - $cyclus_startdate_time;
		    	$interval_days = floor($interval/(60*60*24));
		    	$devided = $interval_days/$scheduleritem["every"];
			    $match = (int) $devided;
		    	if ($devided != $match)
		    	{
			    	$show_item = false;
			    }
		      break;
		    case 'weekly':
          $interval = $unix_date - $cyclus_startdate_time;
		    	$interval_weeks = floor($interval/(60*60*24*7));
		    	$devided = $interval_weeks/$scheduleritem["every"];

			    $match = (int) $devided;
		    	if ($devided != $match)
		    	{
			    	$show_item = false;
			    }
		      break;
		    case 'monthly_date':
					$start_month = $cyclus_startdate_array[1];
					$this_month = strftime('%m', $unix_date);
					$interval = $this_month - $start_month;
					$devided = $interval/$scheduleritem["every"];
					$match = (int) $devided;
					if ($devided != $match)
					{
						$show_item= false;
					}
				  break;

				case 'monthly_day':
					$start_month = $cyclus_startdate_array[1];
					$this_month = strftime('%m', $unix_date);
					$interval = $this_month - $start_month;
					$devided = $interval/$scheduleritem["every"];
					$match = (int) $devided;
					if ($devided != $match)
					{
						$show_item = false;
					}
				  break;
				case 'yearly_date':
				  break;
				case 'yearly_day':
				  atkdebug("YEARLY DAY");
					$start_year = $cyclus_startdate_array[0];
					$this_year = strftime('%Y', $unix_date);
					$interval = $this_year - $start_year;
					$devided = $interval/$scheduleritem["every"];

					$match = (int) $devided;
					if ($devided != $match)
					{
						$show_item = false;
					}
				  break;
      }
    }
    return $show_item;
  }

  /**
   * Get all calendar items for a date range
   *
   * @param string $startdate Startdate of the daterange
   * @param string $enddate Enddate of the daterange
   * @param array $user Array with the users
   */
  function getItems($startdate="",$enddate="",$user=array())
  {
    // If no user given, but a viewuser is set, then filter using the viewuser
    if(count($user)==0 && !empty($this->m_viewUser) && $this->m_viewUser!='all')
      $user[] = $this->m_viewUser;

    // If no startdate is set, use the viewdate as both the start and end date
    // since it's not possible to call this function with only an enddate.
    if($startdate=="")
    {
      $startdate=$this->m_viewdate;
      $enddate=$this->m_viewdate;
    }

    // Get the raw items from the database
    $query = $this->getItemsQuery($startdate, $enddate, $user);
    $recs = $query->executeSelect(true);

    // Clear the alldayItems and appointementItems before populating them
    $this->m_alldayItems = array();
    $this->m_appointmentItems = array();

    // Only perform the loop if there are any records
    if(count($recs) > 0)
    {

      // Loop through all dates between the initial starttime and endtime and increment startdate for day
      $starttime = mktime(12,0,0,substr($startdate,5,2),substr($startdate,8,2),substr($startdate,0,4));
      $endtime = mktime(12,0,0,substr($enddate,5,2),substr($enddate,8,2),substr($enddate,0,4));
      while(date("Ymd",$starttime)<=date("Ymd",$endtime))
      {

        // Datekey
        $datekey = date("Y-m-d", $starttime);
        $this->m_allDayItems[$datekey] = array();
        $this->m_appointmentItems[$datekey] = array();

        // for everydate we need to loop true the recs
        // else we can't see if a rec is valid or not.
        for($i=0,$_i=count($recs);$i<$_i;$i++)
        {

          // Only add the item if it should be visible for this day
          if($this->isItemVisibleOnDate($recs[$i], $starttime))
          {

            // Add the item to $this->m_allDayItems if it spans this day completely
            if ($this->isAllDay($recs[$i]) ||
                 ($recs[$i]["startdate"]<($this->m_viewdate)
                  &&$recs[$i]["enddate"]>($this->m_viewdate)
                 )
               )
            {
              $this->m_allDayItems[$datekey][] = $recs[$i];
            }

            // Else add the item to $this->m_appointmentItems
            else
            {
              $this->m_appointmentItems[$datekey][] = $recs[$i];
              if($this->m_view=="day" || $this->m_view=="employee") $this->_setMatrix($recs[$i]);
            }

          }

        }

        // Increment the starttime to the next day
        $starttime += 86400;
      }
    }
  }

  function weekdaynumbertoflag($weekday_number)
  {
    if (($weekday_number >=0) && ($weekday_number <= 6))
      $weekday = pow(2,$weekday_number);
    else
      $weekday = 0;

    return $weekday;
  }

  /**
   * Get attendees of a schedule item
   *
   * @param int $scheduleid Schedule id
   * @return array Array with attendees of the schedule item
   */
  function getAttendees($scheduleid)
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    $query->addTable('scheduler_attendees');
    $query->addJoin('person', '', 'person.id=scheduler_attendees.person_id', true);
    $query->addField('id', ' ', 'person');
    $query->addField('lastname', ' ', 'person');
    $query->addField('firstname', ' ', 'person');
    $query->addField('role', ' ', 'person');
    $query->addCondition("scheduler_attendees.schedule_id = $scheduleid");
    $result = $query->executeSelect(true);
    return $result;
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
    return ($item["reminder"]==1);
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
  function renderItem($item,$admin=false,$itemDate="")
  {
    $theme = &atktheme::getInstance();

    $user = getUser();
    if(!is_array($item)) return "";
    if($itemDate=="") $itemDate=$this->m_viewdate;
    $tmp="";
    if($admin)
    {
      $tmp .= '<table width="100%" border="0" cellpadding="0" cellspacing="0"><tr>'."\n";
      $tmp .= '<td valign="top" align="left">';
    }
    $isAttendee = false;
    // Get Attendees of the Item
    if ($item["all_users"])
    {
      $names=atkText("allusers","scheduler");
      $attendees='<img src="modules/scheduler/images/multi.png" title="'.$names.'" alt="'.$names.'">';
    }
    else
    {
      // Also check if the current user is an attendee
      $attendees_rows = $this->getAttendees($item["id"]);
      $numberofattendees = count($attendees_rows);
      atkdebug("Attendees: $numberofattendees");
      if($numberofattendees==1)
      {
        if($attendees_rows[0]["id"]==$user["id"]) $isAttendee=true;
        $name = $attendees_rows[0]["firstname"]." ".$attendees_rows[0]["lastname"]." (".$attendees_rows[0]["role"].")";
        $attendees='<img src="modules/scheduler/images/single.png" title="'.$name.'" alt="'.$name.'">';
      }
      elseif($numberofattendees>1)
      {
        for($i=0;$i<$numberofattendees;$i++)
        {
          if($attendees_rows[$i]["id"]==$user["id"]) $isAttendee=true;
          if($i>0) $names.=", ";
          $names.=$attendees_rows[$i]["firstname"]." ".$attendees_rows[$i]["lastname"]." (".$attendees_rows[$i]["role"].")";
        }
        $attendees='<img src="modules/scheduler/images/multi.png" title="'.$names.'" alt="'.$names.'">';
      }
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

    //atk_var_dump($item,"ITEM");
    //atkdebug("SANDY:  owner:".$item["owner"]." - userid".$user["id"]. " - attendee".($isAttendee?"true":"false"));
    if($this->isPrivate($item) && !($item["owner"]==$user["id"] || $isAttendee))
    {
      $tmp.=atkText("private_event","scheduler");
    }
    else
    {
      if($item["owner"]==$user["id"] || $isAttendee || is_allowed("scheduler.scheduler", "edit_all"))
      {
        $tmp.=href($_SERVER["PHP_SELF"]."?atkaction=edit&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"]),$item["title"],SESSION_NESTED);
      }
      elseif(!$this->isPrivate($item))
      {
        $tmp.=href($_SERVER["PHP_SELF"]."?atkaction=view&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"]),$item["title"],SESSION_NESTED);
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
      /*
      if ((!$this->isPrivate($item) || $isAttendee) && $item["owner"]!=$user["id"])
      {
        $view = '<img src="'.$theme->iconPath("view", "recordlist").'" border="0">';
        $tmp.= href($_SERVER["PHP_SELF"]."?atkaction=view&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"]), $view, SESSION_NESTED);
      }
      */
      // you may only edit and delete a calendar item if you are the owner,
      // an attendee or have the right to edit and delete all calendaritems
      if ($item["owner"] == $user["id"] || (is_allowed("scheduler.scheduler", "edit_all") || ($isAttendee || $item["all_users"])))
      {
        //$edit = '<img src="'.$theme->iconPath("edit", "recordlist").'" border="0">';
        $delete = '<img src="'.$theme->iconPath("delete", "recordlist").'" border="0">';
        //$tmp.= "&nbsp;&nbsp;".href($_SERVER["PHP_SELF"]."?atkaction=edit&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"]), $edit, SESSION_NESTED);
        $tmp.= href("dispatch.php?atkaction=delete&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"])."&itemdate=".$itemDate, $delete, SESSION_NESTED);
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
    //if($userprefs["workstarttime"]!="00:00:00") $this->m_work_starttime = $userprefs["workstarttime"]["hours"].":".$userprefs["workstarttime"]["minutes"];
    //if($userprefs["workendtime"]!="00:00:00")$this->m_work_endtime = $userprefs["workendtime"]["hours"].":".$userprefs["workendtime"]["minutes"];
    if($userprefs["scheduler_emptycolor"]!="") $this->m_emptyColor = $userprefs["scheduler_emptycolor"];
    if($userprefs["scheduler_emptyworkhourscolor"]!="") $this->m_emptyWorkHoursColor = $userprefs["scheduler_emptyworkhourscolor"];
    //if($userprefs["startday"]!="") $this->setCalendarStartDay($userprefs["startday"]);
    //if($userprefs["workdays"]!="")
    //{
    //  $this->m_workdays=0;
    //  foreach($userprefs["workdays"] as $day)
    //  {
    //    $this->m_workdays |= $day;
    //  }
    //}
    if($userprefs["showweeknumbers"]!="") $this->m_showWeekNumbers = $userprefs["showweeknumbers"];
    if($userprefs["showtodo"]!="") $this->m_showTodo = $userprefs["showtodo"];
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