<?php
atkimport("modules.scheduler.utils.schedulertools");

class scheduler_model
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
  * @var string $m_viewUsers Current userview
  */
  var $m_viewUsers = "";

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
  * @var string $m_legend Legend of a calendar
  */
  var $m_attendees = array();
  var $m_users = array();

  var $m_showTodo = 0;
  var $m_employeeLookup = array();
  
  
  
  function scheduler_model()
  {
    
  }
  
  function getInstance()
  {
    static $s_in = null;

    if (is_null($s_in))
    {
      $s_in = new scheduler_model();
    }
    return $s_in;
  }
  
  /**
  * Set view user
  * @param string $user Current user
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
  
  function setView($view)
  {
    $this->m_view = $view;
  }

 /**
  * Set viewdate
  * @param int $day Day
  * @param int $month Month
  * @param int $year Year
  */
  function setViewDate($date)
  { 
    if($date=="")
    {
      $date = date("Y-m-d");
    }
    $day   = substr($date, 8,2);
    $month = substr($date, 5,2);
    $year  = substr($date, 0,4);
    $this->m_day    = ($day   == 0) ? date("d") : $day;
    $this->m_month  = ($month == 0) ? date("m") : $month;
    $this->m_year   = ($year  == 0) ? date("Y") : $year;
    $this->m_viewdate = sprintf('%04d-%02d-%02d',$this->m_year,$this->m_month,$this->m_day);
    $this->m_weeknumber = $this->getWeekNumber($this->m_day,$this->m_month,$this->m_year);
  }
  
  function getViewDate()
  {
    return $this->m_viewdate;
  }
  
  function getDay()
  {
    return $this->m_day;
  }
  
  function getMonth()
  {
    return $this->m_month;
  }
  
  function getYear()
  {
    return $this->m_year;
  }

 /**
  * Set the viewdate for the small calendar
  *
  * @param int $day Day
  * @param int $month Month
  * @param int $year Year
  */
  function setSmallViewDate($date)
  {
    if($date=="")
    {
      $this->m_small_viewdate = $this->m_viewdate;
    }
    else
    {
      $day   = substr($date, 8,2);
      $month = substr($date, 5,2);
      $year  = substr($date, 0,4);
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
   * Get weekNumber
   *
   * @param int $day Day
   * @param int $month Month
   * @param int $year Year
   * @return int Weeknumber
   */
  function getWeekNumber($day,$month,$year,$full=false)
  {
    atkimport("modules.scheduler.utils.weeknumber");
    $timestamp = mktime(12,0,0,$month,$day,$year);
    $startday = atkconfig("startday",1);
    $firstweekcontains = atkconfig("firstweekcontains",4);
    $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$firstweekcontains,$full);
    return $weeknumber;
  }
  
  /**
  * Get the legend of the calendar
  * @return string html table with the calendar legend
  */
  function getLegend()
  {
    static $rows = null;
    
    if(is_null($rows))
    {
      
      $g_db = &atkgetdb();
  
      // get the defined colors for each scheduler-type
      $query = "SELECT * FROM scheduler_category ORDER BY id";
      $rows = $g_db->getRows($query);
    }
    return $rows;
  }
  
  function getLegendColors()
  {
    static $itemColors =  null;

    if(is_null($itemColors))
    {
      $nrows = $this->getLegend();
      for ($i=0,$_i=count($nrows);$i<$_i;$i++)
      {
        $itemColors[$nrows[$i]["id"]]["desc"] = $nrows[$i]["description"];
        $itemColors[$nrows[$i]["id"]]["fore"] = $nrows[$i]["fgcolor"];
        $itemColors[$nrows[$i]["id"]]["back"] = $nrows[$i]["bgcolor"];
        
      }
    }
    return $itemColors;
  }
  
  function renderLegend()
  {
    $nrows = $this->getLegend();
    if(count($nrows)==0) return "";
    
    $legend = '<div style="border: 1px solid black; background: white none repeat scroll 0%; -moz-background-clip: -moz-initial; -moz-background-origin: -moz-initial; -moz-background-inline-policy: -moz-initial; font-size: 1em;">
                 <dl><b>'.atktext("legend","scheduler").':</b></dl>
               <div style="margin-left: 2em;">';
    for ($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $legend.='<p style="margin: 0px;">
                 <span style="border: medium none ; background-color: '.$nrows[$i]["bgcolor"].'; color: '.$nrows[$i]["bgcolor"].';">&nbsp;&nbsp;&nbsp;</span>
                   '.$nrows[$i]["description"].'
                </p>';
    }
    $legend.='</div><br></div>';
    return $legend;
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
      $rec = schedulerTools::item2record($nrows[$i]);

      $startdate = date("Y-m-d",($rec["lastdate"]+86400));
      $endstamp = $rec["lastdate"]+(32*86400);
      $enddate = date("Y-m-t",$endstamp);
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
  function getItemsCompact($startdate,$enddate,$user=array())
  {
    if(count($user)==0)
    {
       $user =$this->getSchedulerUsers();
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
    $userprefs = $this->getUserSchedulerPrefs();
    if($userprefs["showemployeebirthdays"])
      $this->addEmployeeBirthdays(true,$dates,date("n",$tmp_startdate),date("n",$tmp_enddate));
    if($userprefs["showtodo"])
      $this->addTodos(true,$dates,$startdate,$enddate);
    return $dates;
  }
  
  function getSchedulerUsers()
  {
    $users = $this->m_viewUsers;
    $realUsers=array();
    // Check for groups
    for($i=0,$_i=count($users);$i<$_i;$i++)
    {
      if(substr($users[$i],0,2)=="g_")
      {
        $group_id = intval(str_replace('g_','',$users[$i]));
        $groupUsers = $this->groupToUsers($group_id);
        array_merge($realUsers,$groupUsers);
      }
      else 
      {
        $realUsers[] = $users[$i];
      }
    }
    return $realUsers;
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
      $user = $this->getSchedulerUsers();
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
          $this->m_appointmentItems[date("Y-m-d",$currenttime)][] = $tmp_nrow;
          
        }
      }
    }
    $this->getAttendees($items);
    $userprefs = $this->getUserSchedulerPrefs();
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
  
  function getAllDayItems($date="")
  {
    if($date=="")
    {
      return $this->m_allDayItems;
    }
    elseif(array_key_exists($date,$this->m_allDayItems))
    {
      return $this->m_allDayItems[$date];
    }
    return array();
  }
  
  function getAppointmentItems($date="")
  {
    if($date=="")
    {
      return $this->m_appointmentItems;
    }
    elseif(array_key_exists($date,$this->m_appointmentItems))
    {
      return $this->m_appointmentItems[$date];
    }
    return array();
    
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
                                                "startdate"=>mktime(0,0,0,$this->m_month,$this->m_day,$this->m_year),
                                                "enddate"=>mktime(23,59,59,$this->m_month,$this->m_day,$this->m_year),
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
                                                  "all_users"=>false,
                                                  "recur"=>"once",
                                                  "birthday"=>true,
                                                  "startdate"=>mktime(0,0,0,$this->m_month,$this->m_day,$this->m_year),
                                                  "enddate"=>mktime(23,59,59,$this->m_month,$this->m_day,$this->m_year),
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
                                            "all_user"=>false,
                                            "recur"=>"once",
                                            "todo"=>true,
                                            "startdate"=>mktime(0,0,0,$this->m_month,$this->m_day,$this->m_year),
                                            "enddate"=>mktime(23,59,59,$this->m_month,$this->m_day,$this->m_year),
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
   * Corrects times to the timeschedule
   * @param string $time Time to be corrected
   * @params int $scheduletime Scheduler scale
   * @param string $round Round the time up or down
   * @return string Corrected time
   */
  function correct_time($time,$scheduletime=30,$round="down")
  {
    $tmp_min = substr($time,3,2);

    if($round=="down")
    {
      $minutes = sprintf("%02d",floor($tmp_min/$scheduletime)*$scheduletime);
    }
    else
    {
      $minutes = sprintf("%02d",ceil($tmp_min/$scheduletime)*$scheduletime);
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
  * Creates a html form with a date/user selector
  * @return string html form with a date/user selector
  */
  function getSelector()
  {
    $form  = '<form method="get" action="'.atkSelf().'">'."\n";
    $form .= session_form();
    $form .= '<input type="hidden" name="view" value="'.$this->m_view.'">';
    $dummy_rec = array("viewdate"=>$this->_getInitialDate());
    $datebox = new atkDateAttribute("viewdate","F d Y","d F Y",0,0,AF_OBLIGATORY);
    $form .=$datebox->edit($dummy_rec);
    $form .= '&nbsp;<input type="submit" value="'.atkText("refresh","scheduler").'">'."\n";
    $form .= '</form>'."\n";
    return $form;
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
    $buff = '<form method="get" action="'.atkSelf().'">'."\n";
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
  * @param  bool $show_groups show the user groups?
  * @param  bool $show_all show the 'all employees' option?
  * @return string html of the input elements
  */
  function _getEmployeesCheckboxes($default=array(), $show_groups=true, $show_all=true)
  {
    // get the records
    $records = $this->_getEmployees();
    if(count($default)==0)
    {
      $default = $this->m_viewUsers;
    }
    atk_var_dump($default,"DEFAULT CHECKED");
    
    $employee_code = '<ul class="employees_checklist">';
          $is_default_all = in_array('all', $default);


    if($show_groups || $show_all)
    {
      array_unshift($records, array(
          'id'        => 'spacer',
          'firstname' => '',
          'lastname'  => '',
        ));
    }
    
    // Add groups
    if($show_groups)
    {
      $userprefs = $this->getUserSchedulerPrefs();
      $groups = $userprefs["groups"];
      $total_groups = count($groups);
      if($total_groups>0)
      {
        for($i=0;$i<$total_groups;$i++)
        {
          array_unshift($records, array(
              'id'        => 'g_'.$groups[$i]["id"],
              'firstname' => '',
              'lastname'  => '<strong>'.$groups[$i]["name"].'</strong>',
            ));
        }
      }
    }
    
       
          
          
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
      if($records[$i]["id"]=="spacer") 
      {
        $employee_code.="<hr>";
        continue;
      }
      
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
  
  function groupToUsers($group_id)
  {
     $userprefs = $this->getUserSchedulerPrefs();
     $groups = $userprefs["groups"];
     $total_groups = count($groups);
     if($total_groups>0)
     {
       $members = array();
       for($i=0;$i<$total_groups;$i++)
       {
         if($groups[$i]["id"]==$group_id)
         {
           foreach($groups[$i]["members"] as $member)
           {
             $member[] = $member["userid"];
           }
         }
       }
       return $members;
     }
     else 
     {
       return array();
     }
    
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
    $userprefs = $this->getUserSchedulerPrefs();
    
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
    $prevUrl  = atkSelf()."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&small_viewdate=$prevTime";

    $nextTime = date("Y-m-d", mktime(0,0,0,$month+1, 1, $year));
    $nextUrl  = atkSelf()."?view=".$this->m_view."&viewdate=".$this->m_viewdate."&small_viewdate=$nextTime";
    $currentTime = date("Y-m-d", mktime(0,0,0,$month, 1, $year));
    $currentUrl  = atkSelf()."?view=month&viewdate=".$currentTime."&small_viewdate=$currentTime";


    $tmp ='<table border="0" cellpadding="1" cellspacing="0">';
    $tmp.='<tr style="background-color: #dddddd">';
    $tmp.='<td align="left">';
    if($monthNav) $tmp.= href($prevUrl,"&lt&lt");
    $tmp.='</td>';
    $tmp.='<td align="center" colspan="'.($userprefs["showweeknumbers"]?6:5).'">';
  	$tmp.= href($currentUrl,$months[intval($month)].' '.$year);
    $tmp.='</td>';
    $tmp.='<td align="right">';
    if($monthNav) $tmp.= href($nextUrl,"&gt;&gt;");
    $tmp.='</td>';

    $tmp.='</tr><tr>';
    if($userprefs["showweeknumbers"]==1) $tmp.='<td>&nbsp;</td>';

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
    $weekUrl = atkSelf()."?view=week&viewdate=$weekTime";
    if($userprefs["showweeknumbers"]==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
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
        $weekUrl = atkSelf()."?view=week&viewdate=$weekTime";
        if($userprefs["showweeknumbers"]==1) $tmp.='<td style="border-right: 1px solid #C0C0C0;"><i>'.href($weekUrl,$week_number).'</i>&nbsp;</td>';
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

    	$dayUrl = atkSelf()."?view=day&viewdate=$dayTime";
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
  
  
  function getContractInfo()
  {
    static $contractInfo = null;
    
    if(is_null($contractInfo))
    {
      $user = &atkGetUser();
      // These can be found in the current user contract so get the current valid contract
      $sql = "SELECT workstarttime,workendtime,WorkingDays FROM usercontract
              WHERE userid = ".($user["id"]!=""?$user["id"]:"-1")."
                AND startdate <= '".$this->m_viewdate."'
                AND (enddate >= '".$this->m_viewdate."'
                     OR enddate IS NULL)";
      $db = &atkGetDb();
      $row = $db->getrows($sql);
      $contractInfo = array();
      if(count($row)==1)
      {
        if($row[0]["workstarttime"]!="00:00:00") $contractInfo["workstarttime"] = substr($row[0]["workstarttime"],0,5);
        if($row[0]["workendtime"]!="00:00:00")$contractInfo["workendtime"] = substr($row[0]["workendtime"],0,5);
        if($row[0]["WorkingDays"]!="")
        {
          $contractInfo["workdays"]=0;
          $days = explode("|",$row[0]["WorkingDays"]);
          foreach($days as $day)
          {
            $contractInfo["workdays"]|= pow(2,$day);
          }
        }
      }
      else
      {
        $contractInfo["workstarttime"]="08:30";
        $contractInfo["workendtime"]="17:00";
        $contractInfo["workdays"]=0;
      }
    }
    return $contractInfo;
  }

  
  
  
  function getUserSchedulerPrefs()
  {
    $user = getUser();
    $schedulerNode = &getNode("scheduler.scheduler");
    $userprefs = $schedulerNode->getUserSchedulerPrefs($user["id"]);

    // Set some defaults incase they aren't set
    if($userprefs["timeschedule"]=="") $userprefs["timeschedule"]=30;
    if($userprefs["scheduler_emptycolor"]=="") $userprefs["scheduler_emptycolor"]='#DDDDDD';
    if($userprefs["scheduler_emptyworkhourscolor"]=="") $userprefs["scheduler_emptyworkhourscolor"]='#FFFFFF';
    if($userprefs["showweeknumbers"]=="") $userprefs["showweeknumbers"]=false;
    if($userprefs["showlunarphases"]=="") $userprefs["showlunarphases"]=false;
    if($userprefs["showtodo"]=="") $userprefs["showtodo"]=false;
    if($userprefs["showemployeebirthdays"]=="") $userprefs["showemployeebirthdays"]=false;
    //atk_var_dump($userprefs,"USER PREFS");
    return $userprefs;
  }
  
}

?>