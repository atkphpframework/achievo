<?php
  // class to generate a monthview
  //
  // typical usage:
  //
  // $dummy = new monthview(25,7,2001);     // show July 25th 2001
  // $dummy->showGlobal  = "wim";          // for user wim
  // $dummy->generate();                  // generate the matrix
  // $dummy->dumpMatrix();                // show it to the audience ....
  //
  //

  class monthView
  {
   var $days        = 0;
   var $month       = 0;
   var $year        = 0;
   var $weeks       = 0;
   var $items       = array();
   var $title       = "";
   var $showGlobal;
   var $viewerWidth = "100%";
   var $fancyBegin  = "<TABLE height='100%' width='100%' border='0' cellPadding='0'><TBODY><TR><TD height='100%' bgColor='#000000'><TABLE width='100%' height='100%' border='0' cellPadding='0' cellSpacing='0'><TBODY><TR height='100%'>";
   var $fancyEnd    = "</TR></TBODY></TABLE></TD></TR></TBODY></TABLE>";

 /**
  * constructor
  * @param $month (if 0, then current month)
  * @param $year  (if 0, then current year)
  */
   function monthView($month=0, $year=0)
   {
    $this->year     = ($year  == 0) ? date("Y") : $year;
    $this->month    = ($month == 0) ? date("m") : $month;
   }

  /**
   * monthNav
   * navigate through months using prev/next month link
   * @returns html code containing links for previous month, this month and next month
   */
   function monthNav()
   {

    $prevTime = date("Y-m-d", mktime(0,0,0,$this->month-1, 1, $this->year));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=monthview&viewdate=$prevTime&dayview=".$this->showGlobal;

    $thisTime = date("Y-m")."-1";
    $thisUrl  = $_SERVER["PHP_SELF"]."?view=monthview&viewdate=$thisTime&dayview=".$this->showGlobal;

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->month+1, 1, $this->year));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=monthview&viewdate=$nextTime&dayview=".$this->showGlobal;

    $this->title = "<font size='-1'><b>".text(strtolower(date("F", mktime(0,0,0,$this->month,1,$this->year))))." ".$this->year."</b></font>";

    return "<font size='-1'>".href($prevUrl, text("previous_month"))."</font>"."&nbsp;"."<font size='-1'>".href($thisUrl, text("this_month"))."</font>"."&nbsp;"."<font size='-1'>".href($nextUrl, text("next_month"))."</font>";
   }

  /**
   * wrapper around the gathering functions
   * fills several info arrays
   */
   function generate()
   {

    $days = 1;
    while (checkdate($this->month, $days, $this->year))
    {
     $days++;
    }
    $days--;

    $weeks = ceil($days/7);
    $begin = 1;
    $n     = 0;

    $this->days  = $days;
    $this->weeks = $weeks;

    for ($j=0;$j<=$weeks;$j++)
    {
     $viewtime = mktime(0,0,0,$this->month, $begin, $this->year);
     $weekday  = strftime("%w",$viewtime);

     if ($weekday==0) $weekday=7;

     for ($i=1; $i<=7;$i++)
     {
      $items[$n]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $items[$n]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
      $items[$n]['stamp'] = $viewtime-(86400*($weekday-$i));
      $n++;
     }

     $begin = $begin+7;
    }

    $this->items = $items;
   }

 /**
   * dumpMatrix
   * the actual displaying function
   */
   function dumpMatrix()
   {

    $item     = 0;
    $colwidth = floor($this->viewerWidth/8);
    $today    = date("Y-m-d");

    $buff .= "<table width='".$this->viewerWidth."' cellpadding='1' cellspacing='1'>\n";
    $buff .= "<tr>\n";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("week")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("mon")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("tue")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("wed")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("thu")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("fri")."</font></td>";
    $buff .= "<td bgcolor='#A0A0A0' valign='top' align='left'><font size='-1'>".text("sat")."</font></td>";
    $buff .= "<td bgcolor='#A0A0A0' valign='top' align='left'><font size='-1'>".text("sun")."</font></td>";
    $buff .= "</tr>\n";

    $w = 0;
    for ($i=0;$i<=$this->weeks;$i++)
    {
     $weekno = strftime("%W", mktime(0,0,0,substr($this->items[$item]["date"],5,2), substr($this->items[$item]["date"],8,2), substr($this->items[$item]["date"],0,4)));
     $href   = $_SERVER["PHP_SELF"]."?view=weekview&viewdate=".$this->items[$item]["date"];
     $buff  .= "<tr>";
     $buff  .= "<td width='$colwidth' bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".href($href, $weekno)."</font></td>";

     for ($j=0;$j<=6;$j++)
     {
      $url     = $_SERVER["PHP_SELF"]."?view=dayview&viewdate=".$this->items[$item]["date"];
      $day     = substr($this->items[$item]["date"],8,2);
      $month   = substr($this->items[$item]["date"],5,2);
      $bgcolor = "#C0C0C0";

      //default background color is white
      if ($this->month == $month) { $bgcolor = "#FFFFFF"; }

      // get all items for this day
      //if there are items then change the background color to yellow
      $allItems = $this->getItems($this->items[$item]["date"]);
      if (count($allItems) > 0) { $bgcolor = "#FFFF00"; }

      //if date is today set background color to orange or red depends on appointments
      if ($this->items[$item]["date"] == $today)
      {
        if (count($allItems) == 0) { $bgcolor = "#FFCC33"; } else { $bgcolor = "FF0000"; }
      }

      $buff .= "<td width='$colwidth' bgcolor='$bgcolor' valign='top' align='left'><font size='-1'>".href($url, $day)."</font></td>";
      $item++;
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
   function getItems($date)
   {
     global $g_db;

     $name = "atk".atkconfig("database")."query";
     $query = new $name();
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

     if ($this->showGlobal != "all")
     {
       $query->addCondition("(schedule.owner='".$this->showGlobal."' OR person.userid='".$this->showGlobal."' OR schedule.all_users=1)");
     }

     $query->addOrderBy('starttime');
     $querystring = $query->buildSelect(TRUE);
     return $g_db->getRows($querystring);
   }

  }
?>