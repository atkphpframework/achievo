<?php
  // class to generate a employeeview
  //
  // typical usage:
  //
  // $dummy = new employeeview(25,7,2001);  // show July 25th 2001
  // $dummy->dumpMatrix();                // show it to the audience ....
  //
  //

  class employeeView
  {
   var $day         = 0;
   var $month       = 0;
   var $year        = 0;

   var $title       = "";
   var $viewerWidth = "100%";
   var $workHours    = array("08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30","14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00","21:30","22:00","22:30","23:00","23:30","24:00");
   var $fancyBegin  = "<TABLE height='100%' width='100%' border='0' cellPadding='0'><TBODY><TR><TD height='100%' bgColor='#000000'><TABLE width='100%' height='100%' border='0' cellPadding='0' cellSpacing='0'><TBODY><TR height='100%'>";
   var $fancyEnd    = "</TR></TBODY></TABLE></TD></TR></TBODY></TABLE>";

 /**
  * constructor
  * @param $month (if 0, then current month)
  * @param $year  (if 0, then current year)
  */
   function employeeView($day=0, $month=0, $year=0)
   {
    $this->year     = ($year  == 0) ? date("Y") : $year;
    $this->month    = ($month == 0) ? date("m") : $month;
    $this->day      = ($day == 0)   ? date("d") : $day;
   }

 /**
   * dumpMatrix
   * the actual displaying function
   */
   function dumpMatrix($selemployee)
   {
    if (strlen($this->month) == 1) $month = "0".$this->month; else $month = $this->month;
    if (strlen($this->day) == 1) $day = "0".$this->day; else $day = $this->day;
    $date = $this->year."-".$month."-".$day;

    $buff = "<table width='".$this->viewerWidth."' cellpadding='1' cellspacing='1'>\n";
    $buff .= "<tr>\n";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".atktext("employee")."</font></td>";
    for ($i=0;$i<count($this->workHours);$i++)
    {
      $time = $this->workHours[$i];
      $addurl  = $_SERVER["PHP_SELF"]."?atkaction=add&atknodetype=calendar.schedule&viewdate=$date&time=$time";
      $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".href($addurl, "<font size='-1'>".$this->workHours[$i]."</font>",SESSION_NESTED)."</font></td>";
    }
    $buff .= "</tr>\n";

    //if all users is checked then loop all users else loop only the selected users
    if ($selemployee[0] == "all")
    {
      $db = &atkGetDb();
      $employees = array();
      $sql = "SELECT userid,  id
              FROM person
              WHERE status='active' AND role='employee'
              ORDER BY lastname";
      $employees = $db->getrows($sql);
    }
    else
    {
      for ($i=0;$i<count($selemployee);$i++)
      {
        $employees[$i]["userid"] = $selemployee[$i];
      }
    }

    for ($i=0;$i<count($employees);$i++)
    {
     $buff  .= "<tr>";
     $buff  .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".$employees[$i]["userid"]."</font></td>";

     for ($j=0;$j<count($this->workHours);$j++)
     {
      //default background color is white
      $bgcolor = "#FFFFFF";

      // get all items for this workhour
      //if there are items then change the background color to yellow
      $allItems = $this->getItems($this->workHours[$j], $date, $employees[$i]["userid"], false);
      if (count($allItems) > 0) { $bgcolor = "#FFFF00"; }

      $buff .= "<td bgcolor='$bgcolor' valign='top' align='left'></td>";
     }
     $buff .= "</tr>";
    }

    $buff .= "</table><br><br>";
    return $buff;
   }

  /**
  * get the items for the specified date
  * fills an array with the items
  */
   function getItems($time, $date, $employee, $showself=true)
   {
     $db = &atkGetDb();

     $query = &$db->createQuery();
     $query->addTable('schedule');
     $query->addJoin('schedule_attendee', '', 'schedule_attendee.schedule_id=schedule.id', TRUE);
     $query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);

     $query->addField('id', ' ', 'schedule');
     $query->addField('startdate', ' ', 'schedule');
     $query->addField('enddate', ' ', 'schedule');
     $query->addField('starttime', ' ', 'schedule');
     $query->addField('endtime', ' ', 'schedule');
     $query->addField('title', ' ', 'schedule');
     $query->addField('description', ' ', 'schedule');
     $query->addField('location', ' ', 'schedule');
     $query->addField('allday', ' ', 'schedule');
     $query->addField('publicitem', ' ', 'schedule');
     $query->addField('owner', ' ', 'schedule');
     $query->addField('status', ' ', 'schedule');
     $query->addField('scheduletype', ' ', 'schedule');
     $query->addField('all_users', ' ', 'schedule');

     $query->addCondition("schedule.startdate <='$date' AND schedule.enddate >= '$date'");
     $query->addCondition("schedule.starttime <='$time' AND schedule.endtime > '$time'");
     $user = getUser();
     
     ($showself==true) ? $query->addCondition("(schedule.owner='".$user["id"]."' OR (person.userid='".$employee."' AND person.role='employee') OR schedule.all_users = 1)")
                       : $query->addCondition("((person.userid='".$employee."' AND person.role='employee') OR schedule.all_users = 1)");

     $query->addOrderBy('starttime');
     $querystring = $query->buildSelect(TRUE);
     return $db->getRows($querystring);
   }

  }
?>