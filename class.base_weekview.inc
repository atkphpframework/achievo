<?
  class weekView
  {
   var $day         = 0;
   var $month       = 0;
   var $year        = 0;
   var $items       = array();
   var $viewerWidth = "100%";
   var $fancyBegin  = "<TABLE height='100%' width='100%' border='0' cellPadding='0'><TBODY><TR><TD height='100%' bgColor='#000000'><TABLE width='100%' height='100%' border='0' cellPadding='0' cellSpacing='0'><TBODY><TR height='100%'>";
   var $fancyEnd    = "</TR></TBODY></TABLE></TD></TR></TBODY></TABLE>"; 

   function weekView($day=0, $month=0, $year=0)
   {
    $this->year     = ($year  == 0) ? date("Y") : $year;
    $this->month    = ($month == 0) ? date("m") : $month;
    $this->day      = ($day   == 0) ? date("d") : $day;  
   }

   function generate()
   {
    $viewtime = mktime(0,0,0,$this->month, $this->day, $this->year);
    $weekday  = strftime("%w",$viewtime);
 
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
     $items[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
     $items[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
     $items[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
    }
    $this->items        = $items;
    $allItems           = $this->getAppointments();
    $this->allDayItems  = $allItems[0];
    $this->appointments = $allItems[1];
   }


   function dayTime($beginHour)
   {
    if ($beginHour < 8)
    {
     $position = "night";
    }
    else
    {
     if ($beginHour >= 8 && $beginHour < 12)
     {
      $position = "morning";
     }
     else
     {
      if ($beginHour >= 12 && $beginHour < 17) 
      { 
       $position = "afternoon"; 
      } 
      else
      {
       $position = "evening";
      }
     }
    }
    return $position;
   }

   function getAppointments()
   {
    global $g_db;

    $allday    = array();
    $appoints  = array();
    $beginDate = $this->items[1]["date"];
    $endDate   = $this->items[count($this->items)-2]["date"];

    // now add them to a nice array
    $n = 0;
    for ($i=0;$i<=count($r)-1;$i++)
    {
     $date      = $r[$i]["item_begin_date"];
     $beginHour = intval(substr($r[$i]["item_begin_time"],0,2));

     if ($r[$i]["item_allday"] != 1)
     {
      $position  = $this->dayTime($beginHour);
      $appoints[$date][$position][$n] = $r[$i];
      $n++;
     }
     else
     {
      $allday[$date] .= "<font size='-1'>".$r[$i]["item_title"]."</font><br>";
     }
    }

    return array($allday, $appoints);
   }

   function weekNav()
   {
    global $PHP_SELF;

    $prevTime = date("Y-m-d", mktime(0,0,0,$this->month, $this->day-7, $this->year));
    $prevUrl  = $PHP_SELF."?view=weekview&viewdate=$prevTime&weekview=".$this->showGlobal;  

    $thisTime = date("Y-m-d");
    $thisUrl  = $PHP_SELF."?view=weekview&viewdate=$thisTime&weekview=".$this->showGlobal;    

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->month, $this->day+7, $this->year));
    $nextUrl  = $PHP_SELF."?view=weekview&viewdate=$nextTime&weekview=".$this->showGlobal; ;

    return href($prevUrl, text("Previous week"))."&nbsp;".href($thisUrl, text("This week"))."&nbsp;".href($nextUrl, text("Next week"));
   }

   function dumpAppointments($date, $dayTime)
   {
    $nAppoints = count($this->appointments[$date][$dayTime]);

    if ($dayTime == "allday")
    {
     $buff .= $this->allDayItems[$date];
    }
    else
    {
     for ($i=0;$i<=$nAppoints-1;$i++)
     {
      $buff .= "<font size='-1'>".$this->appointments[$date][$dayTime][$i]["item_title"]."</font><br>";
     }
    }

    return $buff;
   }


   function dumpMatrix()
   {
    global $PHP_SELF;

    $buff  = $this->fancyBegin."<td><table width='".$this->viewerWidth."' cellpadding='1' cellspacing='1' border='0'>\n";

    $buff .= "<tr bgcolor='#D0D0D0'>\n";
    $buff .= "<td>&nbsp;</td>";

    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $text  = text($this->items[$i]["day"])."<br>".substr($this->items[$i]["date"],8,2)."-".substr($this->items[$i]["date"],5,2);
     $href  = $PHP_SELF."?view=dayview&viewdate=".$this->items[$i]["date"]."&dayview=".$this->showGlobal;
     $buff .= "<td>".href($href, $text)."</td>";
    }
    $buff .= "</tr>";

    // allday events
    $buff .= "<tr bgcolor='#FFFFFF'>";
    $buff .= "<td valign='top' bgcolor='#F0F0F0'><font size='-1'>Allday<nobr> events</font></td>";   

    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $date  = $this->items[$i]["date"];
     $buff .= "<td>".$this->dumpAppointments($date, "allday")."</td>";
    }

    $buff .= "</tr>";
    $buff .= "</tr>";


    // midnight till 08:00
    $buff .= "<tr bgcolor='#FFFFFF'>";
    $buff .= "<td valign='top' bgcolor='#F0F0F0'><font size='-1'>00:00<nobr> - 08:00</font></td>";   

    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $date  = $this->items[$i]["date"];
     $buff .= "<td>".$this->dumpAppointments($date, "night")."</td>";
    }

    $buff .= "</tr>";


    // morning
    $buff .= "<tr bgcolor='#FFFFFF'>";
    $buff .= "<td valign='top' bgcolor='#F0F0F0'><font size='-1'>08:00<nobr> - 12:00</font></td>";   

    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $date  = $this->items[$i]["date"];
     $buff .= "<td>".$this->dumpAppointments($date, "morning")."</td>";
    }

    $buff .= "</tr>";

    // afternoon
    $buff .= "<tr bgcolor='#FFFFFF'>";
    $buff .= "<td valign='top' bgcolor='#F0F0F0'><font size='-1'>12:00<nobr> - 17:00</font></td>";   

    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $date  = $this->items[$i]["date"];
     $buff .= "<td>".$this->dumpAppointments($date, "afternoon")."</td>";
    }

    $buff .= "</tr>";

    // evening
    $buff .= "<tr bgcolor='#FFFFFF'>";
    $buff .= "<td valign='top' bgcolor='#F0F0F0'><font size='-1'>17:00<nobr> - 24:00</font></td>";   

    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $date  = $this->items[$i]["date"];
     $buff .= "<td>".$this->dumpAppointments($date, "evening")."</td>";
    }

    $buff .= "</tr>";

    $buff .= "<tr bgcolor='#D0D0D0'>\n";
    $buff .= "<td>&nbsp;</td>";
    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $text  = text("&nbsp;Add<nobr> new&nbsp;");
     $href  = $PHP_SELF."?atkaction=add&viewdate=".$this->items[$i]["date"];
     $buff .= "<td>".href($href, $text)."</td>";
    }
    $buff .= "</tr>";

    $buff .= "</table></td>".$fancyEnd;

    return $buff;
   }
  }
?>   