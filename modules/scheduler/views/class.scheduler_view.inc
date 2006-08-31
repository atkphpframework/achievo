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
    $tmp.= $months[intval($month)].' '.$year;
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
        $week_number = date("W", mktime(12,0,0,$month,$i,$year));
        $weekUrl = $_SERVER["PHP_SELF"]."?view=week&viewdate=$weekTime&user=".$this->m_viewUser;
        if($this->m_showWeekNumbers==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
    		$cellcount=0;
    	}
    	$cellcount++;

      $dayTime = date("Y-m-d",mktime(12,0,0,$month,$i,$year));

    	if (($i == date("d")) && ($year == date("Y")) && ($month==date("m")) && ($holiday->isHoliday($dayTime) || (isset($itemdates[$dayTime]) && $itemdates[$dayTime]===true)))
    	{
    		$class="calendarItemToday";
    	}
    	elseif(($i == date("d")) && ($year == date("Y")) && ($month==date("m")))
    	{
    		$class="calendarToday";
    	}
    	elseif($holiday->isHoliday($dayTime) || (isset($itemdates[$dayTime]) && $itemdates[$dayTime]===true))
    	{
    	  $class="calendarItem";
    	}
    	else
    	{
    		$class="calendarNormal";
    	}
    	/*
      $dayTime = date("Y-m-d",mktime(12,0,0,$month,$i,$year));

    	$holiday_title = '';
    	if($holiday->isHoliday($dayTime))
    	{
    	    $holidayInfo = $holiday->getHolidayInfo($dayTime);
    			$holiday_title = ' title="'.$holidayInfo["name"].'"';
    			if($class=="calendarToday") { $class="calendarHolidayToday"; } else { $class="calendarHoliday"; }
    	}
    	*/
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
    atk_var_dump($nrows,"UPDATE RECURRING EVENTS");
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $id = $nrows[$i]["id"];
      $rec = $this->item2record($nrows[$i]);

      $startdate = date("Y-m-d",($rec["lastdate"]+86400));
      $endstamp = $rec["lastdate"]+(32*86400);
      $enddate = date("Y-m-",$endstamp);
      $enddate.= date("t",$endstamp);
      atkdebug("scheduler_view::updateRecurringEvents -> GET DATES: $startdate - $enddate");
      $dates = schedulertools::getDates($rec,$startdate,$enddate);
      foreach($dates as $date)
      {
        $tmp_start = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]);
        $tmp_end = mktime($rec["endtime"]["hours"],$rec["endtime"]["minutes"],$rec["endtime"]["seconds"],$rec["enddate"]["month"],$rec["enddate"]["day"],$rec["enddate"]["year"]);
        $duration = $tmp_end-$tmp_start;

        $startdate = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],substr($date,5,2),substr($date,8,2),substr($date,0,4));
        $enddate = $startdate+$duration;
        atkdebug("STARTDATE: ".date("Ymd H:i:s",$startdate));
        atkdebug("ENDDATE: ".date("Ymd H:i:s",$enddate));

        $sql = "INSERT INTO scheduler_dates (scheduler_id,startdate,enddate) VALUES ('$id','$startdate','$enddate')";
        atkdebug("UPDATE: $sql");
        $db->query($sql);
      }
      $sql = "UPDATE scheduler_scheduler SET lastdate = '$endstamp' WHERE id='$id'";
      $db->query($sql);
    }
  }

  function getItemsCompact($startdate,$enddate)
  {
    $current_user = getUser();
    $user = array();
    if(count($user)==0 && !empty($this->m_viewUser) && $this->m_viewUser!='all') $user[] = $this->m_viewUser;

    $this->updateRecurringEvents($startdate,$enddate);
    $name = "atk".atkconfig("database")."query";
    $query = atknew($name);
    $query->addTable('scheduler_scheduler');
    $query->addJoin('scheduler_attendees', '', 'scheduler_attendees.schedule_id=scheduler_scheduler.id', TRUE);
    $query->addJoin('person', '', 'person.id=scheduler_attendees.person_id', TRUE);
    $query->addJoin('person', 'owner', 'owner.id=scheduler_scheduler.owner', TRUE);
    $query->addJoin('scheduler_dates','','scheduler_dates.scheduler_id=scheduler_scheduler.id',TRUE);
    $query->addField('startdate',' ','scheduler_dates');
    $query->addField('enddate',' ','scheduler_dates');

    $tmp_enddate = mktime(0,0,0,substr($startdate,5,2),substr($startdate,8,2),substr($startdate,0,4));
    $tmp_startdate = mktime(23,59,59,substr($enddate,5,2),substr($enddate,8,2),substr($enddate,0,4));

     $condition = "$tmp_enddate < scheduler_dates.enddate AND scheduler_dates.startdate < $tmp_startdate";
    if(count($user)>0)
    {
      $useridcondition = (count($user)>0) ? " OR scheduler_scheduler.owner IN ('".implode("','", $user)."') OR scheduler_attendees.person_id IN ('".implode("','", $user)."')" : "";
      $condition.=" AND (scheduler_scheduler.all_users = '1' $useridcondition)";
    }
    $query->addCondition($condition);


    $querystring = $query->buildSelect(TRUE);
    atkdebug("getItemsCompact: ($startdate - $enddate) ".date("Y-m-d H:i",$tmp_enddate)." - ".date("Y-m-d H:i",$tmp_startdate)."<br>QUERY STRING: $querystring");
    $db = &atkGetDb();
    $nrows = $db->getrows($querystring);

    $dates = array();
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $enddate = date("Ymd",$nrows[$i]["enddate"]);
      for($j=0;date("Ymd",($nrows[$i]["startdate"]+($j*86400)))<=$enddate;$j++)
      {
        $dates[date("Y-m-d",($nrows[$i]["startdate"]+($j*86400)))]=true;
      }
      //$dates[date("Y-m-d",$nrows[$i]["startdate"])]=true;
    }
    return $dates;
  }


 function getItems($startdate="",$enddate="",$user=array())
  {
    $current_user = getUser();
    $db = &atkGetDb();

    if(count($user)==0 && !empty($this->m_viewUser) && $this->m_viewUser!='all') $user[] = $this->m_viewUser;

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

    $name = "atk".atkconfig("database")."query";
    $query = atknew($name);
    $query->addTable('scheduler_scheduler');
    $query->addJoin('scheduler_attendees', '', 'scheduler_attendees.schedule_id=scheduler_scheduler.id', TRUE);
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

    //$tmp_startdate = 1157317200;
    //$tmp_enddate = 1154296800;
    //$condition = "scheduler_dates.startdate >= $tmp_startdate AND scheduler_dates.enddate <= $tmp_enddate";

    $condition = "$tmp_enddate < scheduler_dates.enddate AND scheduler_dates.startdate < $tmp_startdate";

    if(count($user)>0)
    {
      $useridcondition = (count($user)>0) ? " OR scheduler_scheduler.owner IN ('".implode("','", $user)."') OR scheduler_attendees.person_id IN ('".implode("','", $user)."')" : "";
      $condition.=" AND (scheduler_scheduler.all_users = '1' $useridcondition)";
    }

    $query->addCondition($condition);
    $query->addOrderBy('starttime');


    $querystring = $query->buildSelect(TRUE);
    atkdebug("Getitems for ".date("Y-m-d",$tmp_startdate)." ($tmp_startdate) - ".date("Y-m-d",$tmp_enddate)." ($tmp_enddate)");

    atkdebug("QUERY STRING: $querystring");
    $nrows = $db->getrows($querystring);
    $items = array();
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $items[] = $nrows[$i]["id"];
      $startdate = date("Ymd",$nrows[$i]["startdate"]);
      $enddate = date("Ymd",$nrows[$i]["enddate"]);

      for($j=0;date("Ymd",($nrows[$i]["startdate"]+($j*86400)))<=$enddate;$j++)
      {
        $currenttime = $nrows[$i]["startdate"]+($j*86400);
        //TODO: Optimize for week and month !!
        if($this->m_view=="day" && date("Y-m-d",$currenttime)!=$this->m_viewdate) continue;
        $currentdate = date("Ymd",$currenttime);
        $tmp_nrow = $nrows[$i];
        atkdebug("SANDY: ".date("Ymd",$currenttime." - ".$tmp_nrow["title"]));

        if($nrows[$i]["allday"]==1 || ($startdate!=$currentdate && $enddate!=$currentdate))
        {
          if($tmp_nrow["allday"]==0) $tmp_nrow["allday"]=1;
          $this->m_allDayItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
          atkdebug("SET ALLDAY");
        }
        else
        {
          if($startdate==$currentdate && $startdate!=$enddate)
          {
            $tmp_nrow["enddate"] = mktime(23,59,00,date("m",$currenttime),date("d",$currenttime),date("Y",$currenttime));
            $tmp_nrow["endtime"] = "23:59:00";
          }
          elseif($enddate==$currentdate && $startdate!=$enddate)
          {
            $tmp_nrow["startdate"] = mktime(0,0,0,date("m",$currenttime),date("d",$currenttime),date("Y",$currenttime));
            $tmp_nrow["starttime"] = "00:00:00";
          }
          //$this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
          if(($this->m_view=="day" || $this->m_view=="employee") && date("Y-m-d",$currenttime)==$this->m_viewdate)
          {
            $this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
            $this->_setMatrix($tmp_nrow);
            atkdebug("SET DAY APPOINTMENT");
          }
          elseif($this->m_view!="day" && $this->m_view!="employee" )
          {
            $this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
            atkdebug("SET NON DAY APPOINTMENT");
          }
        }
      }
    }
    atk_var_dump($this->m_appointmentItems,"Appointments");
    $this->getAttendees($items);

  }


/**
   * Get attendees of a schedule item
   * @global object $_db Database connection
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
    $query->addField('schedule_id', ' ', 'scheduler_attendees');

    $query->addField('lastname', ' ', 'person');
    $query->addField('firstname', ' ', 'person');
    $query->addField('role', ' ', 'person');
    atk_var_dump($items,"SANDY ITEMS");
    $query->addCondition("scheduler_attendees.schedule_id IN (".implode(",",$items).")");

    $querystring = $query->buildSelect(TRUE);
    $nrows = $db->getRows($querystring);
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $this->m_attendees[$nrows[$i]["schedule_id"]][] = $nrows[$i]["id"];
      if(!array_key_exists($nrows[$i]["id"],$this->m_users))
      {
        $this->m_users[$nrows[$i]["id"]] = array("firstname"=>$nrows[$i]["firstname"],
                                                 "lastname"=>$nrows[$i]["lastname"],
                                                 "role"=>$nrows[$i]["role"]
                                                );
      }
    }
    unset($nrows);
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
  function renderItem($item,$admin=false,$itemDate="",$showcategory=false)
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
    // If showcategory we will show a box with the category color
    if($showcategory)
    {
      $tmp.='<img style="background-color: '.$this->m_itemColors[$item["category"]]["back"].';" src="images/trans.gif" border="1" width="10" height="10" title="'.$this->m_itemColors[$item["category"]]["desc"].'">&nbsp;';
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
      //$attendees_rows = $this->getAttendees($item["id"]);
      $numberofattendees = count($this->m_attendees[$item["id"]]);
      atkdebug("Attendees: $numberofattendees");

      $names = "";
      for($i=0;$i<$numberofattendees;$i++)
      {

        $userid = $this->m_attendees[$item["id"]][$i];
        if($userid==$user["id"]) $isAttendee=true;
        if($i>0) $names.=", ";
        $names.= $this->m_users[$userid]["firstname"]." ".$this->m_users[$userid]["lastname"].($this->m_users[$userid]["role"]!="employee"?" (".$this->m_users[$userid]["role"].")":"");
      }
      if($numberofattendees==1)
      {
        $attendees='<img src="'.$theme->imgPath("single.png","scheduler").'" width=15 height=15 title="'.$names.'" alt="'.$names.'">';
      }
      else 
      {
        $attendees='<img src="'.$theme->imgPath("multi.png","scheduler").'" width=15 height=15 title="'.$names.'" alt="'.$names.'">';
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

    // If item is private and current is not an attendee or owner, show private event.
    if($this->isPrivate($item) && !($item["owner"]==$user["id"] || $isAttendee))
    {
      $tmp.=atkText("private_event","scheduler");
    }
    else
    {
      if($item["owner"]==$user["id"] || $isAttendee || in_array($item["owner"],$this->m_assistantFor))
      {
        $tmp.=href($_SERVER["PHP_SELF"]."?atknodetype=scheduler.scheduler&atkaction=".($this->isRecurring($item)?"editserieorsingle":"edit")."&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"])."&scheduler_id=".$item["id"]."&itemdate=".$item["startdate"],$item["title"],SESSION_NESTED);
      }
      elseif(!$this->isPrivate($item))
      {
        $tmp.=href($_SERVER["PHP_SELF"]."?atknodetype=scheduler.scheduler&atkaction=view&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"])."&scheduler_id=".$item["id"]."&itemdate=".$item["startdate"],$item["title"],SESSION_NESTED);
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
      // or an assistant of the owner
      if ($item["owner"] == $user["id"] || in_array($item["owner"],$this->m_assistantFor))
      {
        $delete = '<img src="'.$theme->iconPath("delete", "recordlist").'" border="0">';
        $tmp.= href("dispatch.php?atknodetype=scheduler.scheduler&atkaction=delete&atkselector=".rawurlencode("scheduler_scheduler.id=".$item["id"])."&scheduler_id=".$item["id"]."&itemdate=".$item["startdate"], $delete, SESSION_NESTED);
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
    atk_var_dump($user,"CURRENT USER");
    $schedulerNode = &getNode("scheduler.scheduler");
    $userprefs = $schedulerNode->getUserSchedulerPrefs($user["id"]);

    if($userprefs["timeschedule"]!="") $this->m_scheduletime = $userprefs["timeschedule"];
    if($userprefs["scheduler_emptycolor"]!="") $this->m_emptyColor = $userprefs["scheduler_emptycolor"];
    if($userprefs["scheduler_emptyworkhourscolor"]!="") $this->m_emptyWorkHoursColor = $userprefs["scheduler_emptyworkhourscolor"];
    if($userprefs["showweeknumbers"]!="") $this->m_showWeekNumbers = $userprefs["showweeknumbers"];
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
        foreach($days as $key=>$day)
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