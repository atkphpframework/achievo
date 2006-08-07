<?php
atkimport("modules.scheduler.views.scheduler_view");


class scheduler_dayview extends scheduler_view
{

 /**
  * @var array $m_itemMatrix Array with items for the dayview
  */
  var $m_itemMatrix = array();

 /**
  * @var array $m_matrixInfo Array with the matrix for the dayview
  */
  var $m_matrixInfo = array();

  var $m_scheduletime = 30;
  var $m_work_starttime = "08:30";
  var $m_work_endtime = "17:00";
  var $m_allDayHours = array();
  var $m_workHours = array();
  var $m_maxcols = 15;

 /**
  * scheduler dayview constructor
  */
  function scheduler_dayview()
  {
     $this->setView("day");
  }


 /**
  * Find a value in an Array
  * @param string $what Look for what)
  * @param array $where Array to look in
  * @return int index within the array if found, else -1
  */
  function _findInArray($what, $where, $endtime=false)
  {
    $res = -1;

    for ($i=0;$i<=count($where)-1;$i++)
    {
      if ($what>=$where[$i] && $what<=$where[$i])
      {
        $res = ($endtime?($i-1):$i);
        break;
      }
    }
    if($res==-1) $res = count($where)-1;
    return $res;
  }

 /**
  * calculate the duration of a calendar item
  * @param string $begin (begin time, should be something like 09:00 or 10:30)
  * @param string $eind  (end time, should be something like 09:00 or 10:30)
  * @return array containing all hour items between begin and end
  */
  function _duration($begin, $eind)
  {
    $a = $this->_findInArray($begin, $this->m_allDayHours);
    $b = $this->_findInArray($eind, $this->m_allDayHours,true);

    for ($i=$a;$i<=$b;$i++)                      // a half our is now one bar, for two bars remove -1 after i<=$b
    {
      $items[] = $this->m_allDayHours[$i];
    }

    return $items;
  }


 /**
  * Check if the column is free
  * @param array $duration  Array with duration items, see _duration() function
  * @param int $col Collumn to check
  * @return bool Returns true if all cols for $col are free for the items in $duration, else false
  */
  function _isColFree($duration, $col)
  {
    $used = 0;
    for ($i=0;$i<count($duration);$i++)
    {
      if (trim($this->m_itemMatrix[$duration[$i]][$col]) != "") $used++;
    }
    if($used>0)
      return false;
    else
      return true;
  }

  /**
   * Adds an appointment to a free timeslot (duration/col)
   * $param $item  Full record containing all info about the specified appointment
   */
   function _setMatrix($item)
   {
     atkdebug("SET MATRIX: ".$item["starttime"]." - ".$item["endtime"]." - ".$item["title"]);
     /*
     $begin    = $this->_correct_time(substr($item["starttime"],0,5),"down");
     if($item["startdate"]==$item["enddate"])
     {
       $begin    = $this->_correct_time(substr($item["starttime"],0,5),"down");
       $einde    = $this->_correct_time(substr($item["endtime"],0,5),"down");
     }
     else
     {
       // Appointment for more then 1 day
       if($item["startdate"]<($this->m_year."-".$this->m_month."-".$this->m_day))
       {
         $begin = $this->_correct_time("00:00","down");
         $einde    = $this->_correct_time(substr($item["endtime"],0,5),"down");
       }
       elseif($item["enddate"]>($this->m_year."-".$this->m_month."-".$this->m_day))
       {
         $begin    = $this->_correct_time(substr($item["starttime"],0,5),"down");
         $einde = $this->_correct_time("23:59","up");
       }
     }
     */
     $begin = $this->_correct_time(date("H:i",$item["startdate"]));
     $einde = $this->_correct_time(date("H:i",$item["enddate"]),"up");

     $duration = $this->_duration($begin, $einde);
     atkdebug("setMatrix (".$this->m_maxcols.") - $begin - $einde - ".print_r($duration,true));
     $appoints = count($this->m_itemMatrix[$begin]);

     // find the first free collumn in which this item fits
     if ($appoints == 0)
     {
       $col = 0;
     }
     else
     {
       for ($col=0;$col<=$this->m_maxcols;$col++)
       {
         if ($this->_isColFree($duration, $col)) break;
       }
     }

     // add the duration (used in the display part for rowspan)
     $this->m_matrixInfo[$duration[0]][$col]["duration"] = count($duration);
     //fill the matrix
     for ($i=0;$i<count($duration);$i++)
     {
       $this->m_itemMatrix[$duration[$i]][$col]         = $item["title"];
       $this->m_matrixInfo[$duration[$i]][$col]["info"] = $item;
     }
   }

  /**
   * the actual displaying function
   * @return string Returns a renderd day matrix
   */
   function _dumpMatrix()
   {
     $buff   = "";
     $buff  .= $this->_view_header();
     $cols   = $this->_getCols();
     atkdebug("COLS: $cols");
     $colors = $this->m_itemColors;
     $holiday = &getNode("scheduler.scheduler_holidays");

     if ($cols == 0)
     {
      $cWidth = "95%";
      $aCols  = 2;
     }
     else
     {
      $cWidth = floor(95/$cols)."%";
      $aCols  = $cols+1;
     }
     $buff.='<table border=0 width="90%" cellspacing="2"><tr><td>';
     // if we have allday items show 'em in a seperate table
     //if($holiday->isHoliday($this->m_viewdate)) atkdebug("SANDY !!!!");
     if (count($this->m_allDayItems[$this->m_viewdate]) > 0 ||$holiday->isHoliday($this->m_viewdate))
     {

      $buff .= "<table border='0' cellpadding='1' cellspacing='1' width='90%'>\n";
      $buff .= "<tr>\n";
      $buff .= "<td colspan='$aCols' bgcolor='#D0D0D0' valign='top' align='left'>".atkText("allday_events","scheduler")."</td>\n";
      $buff .= "</tr>\n";
      /*
      if($holiday->isHoliday($this->m_viewdate))
      {
        $holidayInfo = $holiday->getHolidayInfo($this->m_viewdate);
        $type    = $holidayInfo["scheduletype"];

        $bgcolor = $colors[$type]["back"];
        $fgcolor = $colors[$type]["fore"];

        $buff .= "<tr>\n";
        $buff .= "<td colspan='$aCols' style='background-color: $bgcolor; color: $fgcolor' valign='top' align='left'>";

        $buff.= $holidayInfo["name"];
        $buff .= "</td>";
        $buff .= "</tr>";
      }
      */
      for ($i=0;$i<=count($this->m_allDayItems)-1;$i++)
      {
       $type    = $this->m_allDayItems[$this->m_viewdate][$i]["scheduletype"];
       $bgcolor = $colors[$type]["back"];
       $fgcolor = $colors[$type]["fore"];

       $buff .= "<tr>\n";
       $buff .= "<td colspan='$aCols' bgcolor='$bgcolor' valign='top' align='left'>";

       $buff.= $this->renderItem($this->m_allDayItems[$this->m_viewdate][$i],true);
       $buff .= "</td>";
       $buff .= "</tr>";
      }
      $buff .= "</table><br>";
     }

    // normal appointments
     $buff .= "<table border='0' cellpadding='1' cellspacing='0' width='90%'>\n";
     $lastHour="";
     $newHour=true;
     $theme = &atkTheme::getInstance();
     $defaulthighlight = $theme->getAttribute("highlight");
     for ($i=0;$i<count($this->m_allDayHours);$i++)
     {
      $time    = $this->m_allDayHours[$i];
      $items   = $this->m_itemMatrix[$time];
      $now     = $this->year."-".$this->month."-".$this->day;
      $addurl  = $_SERVER["PHP_SELF"]."?atkaction=add&atknodetype=scheduler.scheduler&date=$now&time=$time";
      if(in_array($time,$this->m_workHours))
      {
        $weekday_number = date("w",mktime(12,0,0,$this->m_month,$this->m_day,$this->m_year));
        atkdebug("Weekdag: $weekday_number | ".$this->m_workdays."  | ".$time);
        if(hasFlag($this->m_workdays,$this->m_weekday_arr[$weekday_number]) && !$holiday->isHoliday($this->m_viewdate,"weekend"))
        {
          atkdebug("Empty WorkHours");
          $empty_bgcolor= $this->m_emptyWorkHoursColor;
        }
        else
        {
          atkdebug("Empty Color");
          $empty_bgcolor=$this->m_emptyColor;
        }
      }
      else
      {
        $empty_bgcolor=$this->m_emptyColor;
      }
      $buff .= "<tr>\n";
      if($i==0 || ($i%2)==0)
      {
        if($lastHour!=substr($time,0,2))
        {
          $buff .= "<td style='background-color: #D0D0D0; border-top: 2px solid #FFFFFF;' width='2%' rowspan=2 align='right' valign='top'>".href($addurl, intval(substr($time,0,2)),SESSION_NESTED,false,'style="font-size: 20px;"')."</td>";
          $lastHour=substr($time,0,2);
          $newHour=true;
        }
        else
        {
          $newHour=false;
          $buff.="<td style='background-color: #D0D0D0; border-top: 1px solid #D0D0D0;border-bottom: 1px solid #D0D0D0;border-right: 0px solid #D0D0D;' width='2%' rowspan=2 align='right' valign='top'>&nbsp;</td>";
        }
        $buff .= "<td style='background-color: #D0D0D0; border-top: ".($newHour?2:1)."px solid #FFFFFF;border-bottom: 1px solid #D0D0D0;border-right: 1px solid #FFFFFF;' width='1%' rowspan=2 valign='top'>".href($addurl, substr($time,3,2),SESSION_NESTED)."<br>&nbsp;</td>";
      }
      else
      {
        $newHour=false;
      }

      if ($cols == 0)
      {
       $buff .= "<td style='background-color: ".$empty_bgcolor."; border-top: ".($newHour?2:1)."px solid #FFFFFF;border-bottom: 1px solid #D0D0D0;  cursor: pointer; cursor: hand;' width='$cWidth' bgcolor='".$empty_bgcolor."'  onclick=\"document.location.href='$addurl'\" onmouseover=\"highlightrow(this, '".$defaulthighlight."')\" onmouseout=\"resetrow(this)\">&nbsp;</td>\n";
      }
      else
      {
       for ($j=0;$j<$cols;$j++)
       {
        $item      = $items[$j];
        //$timeLap   = ceil($this->matrixInfo[$time][$j]["duration"]/ceil(60/$this->m_scheduletime));
        $timeLap   = $this->m_matrixInfo[$time][$j]["duration"];
        $info      = $this->m_matrixInfo[$time][$j]["info"];
        atk_var_dump($info,"INFO RECORD");
        if(trim($item) != "")
        {
         if ($timeLap != 0)
         {

          if ($timeLap == 1)
          {
           $rowspan = "";
          }
          else
          {
           $rowspan = 'rowspan="'.$timeLap.'"';
          }

          if ($info["private"] == 1 && $info["owner"] != $this->owner)
          {
           $title   = $this->busyTitle;
           $bgcolor = $this->busyBgColor;
           $fgcolor = $this->busyFgColor;
           $descr   = "";
          }
          else
          {
           $title   = $item;
           $bgcolor = $colors[$info["category"]]["back"];
           $fgcolor = $colors[$info["category"]]["fore"];
           $descr   = $info["description"];
           $status  = " [".$info["status"]."]";
          }

          $buff .= "<td $rowspan height='100%' valign='top' width='$cWidth' align='left' bgcolor='".$bgcolor."'>";
          //$buff .= $header;
          $buff .= $this->renderItem($info,true);
          $buff .= "</td>";
         }
        }
        else
        {
         $buff .= "<td style='background-color: ".$empty_bgcolor."; border-top: 1px solid #D0D0D0;border-bottom: 1px solid #D0D0D0;border-right: 0px solid #D0D0D; cursor: pointer; cursor: hand;' width='$cWidth' onclick=\"document.location.href='$addurl'\" onmouseover=\"highlightrow(this, '".$defaulthighlight."')\" onmouseout=\"resetrow(this)\">&nbsp;</td>\n";
        }
       }
      }

      $buff .= "</tr>\n";
     }
     $buff .= "</table>";
     $buff.='</td><td width="200" valign="top">';

     $buff.=$this->_getSmallCalendar(substr($this->m_small_viewdate,8,2),substr($this->m_small_viewdate,5,2),substr($this->m_small_viewdate,0,4));
     $buff.="<br>";
     $next_date = date("Y-m-d",mktime(12,0,0,substr($this->m_small_viewdate,5,2)+1,substr($this->m_small_viewdate,8,2),substr($this->m_small_viewdate,0,4)));
     $buff.=$this->_getSmallCalendar(substr($next_date,8,2),substr($next_date,5,2),substr($next_date,0,4),false,true);
     $buff.="</td></tr></table>";
     $buff.="<br><br>\n";
     $buff.= $this->m_legend;
     return $buff;
    }

  /**
   * navigate through days using prev/next day link
   * @returns html code containing links for previous day, today and next day
   */
   function _getNav()
   {

    $prevTime = date("Y-m-d", mktime(0,0,0,$this->m_month, $this->m_day-1, $this->m_year));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=$prevTime&user=".$this->m_viewUser;

    $currentTime = date("Y-m-d");
    $currentUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=$currentTime&user=".$this->m_viewUser;

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->m_month, $this->m_day+1, $this->m_year));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=".$this->m_view."&viewdate=$nextTime&user=".$this->m_viewUser;

    return href($prevUrl, atkText("previous_day","scheduler"))."&nbsp;".href($currentUrl, atkText("today","scheduler"))."&nbsp;".href($nextUrl, atkText("next_day","scheduler"));
   }

  /**
   * get number of collumns (<TD>'s)
   * @return integer containing number of <TD>'s needed within the display mode
   */
    function _getCols()
    {
      atk_var_dump($this->m_itemMatrix,"ITEM MATRIX");
     $cols = 0;
     while (list($key, $val) = each($this->m_itemMatrix))
     {
      atkdebug("$key = ".count($val));
      $a = count($val);
      if ($cols < $a) { $cols = $a; }
     }
     return $cols;
    }

 /**
  * Render the view
  * @return string A renderd view
  */
  function renderView()
  {
     $page = &atkPage::getInstance();
     $page->register_script(atkconfig("atkroot")."atk/javascript/recordlist.js");
     $this->_setUserSettings();
     // Fill $allDayHours and $workHours
     for($i=0;$i<=23;$i++)
     {
        for($j=0;$j<=55;$j+=$this->m_scheduletime)
        {
          $time = sprintf("%02d:%02d",$i,$j);
          $this->m_allDayHours[] =  $time;
          if($time>=$this->_correct_time($this->m_work_starttime,"down") && $time<$this->_correct_time($this->m_work_endtime,"up"))
            $this->m_workHours[] = $time;
        }
     }

     $this->m_viewTitle = "<b>".atkText(strtolower(date("l", mktime(0, 0, 0, $this->m_month, $this->m_day, $this->m_year))),"atk")." ".$this->m_day."-".$this->m_month."-".$this->m_year."</b>";

     $this->getItems();

     // get scheduler Legend and Item colors
     $this->getLegend();

     $holidays = &getNode("scheduler.scheduler_holidays");
     $check = $holidays->isHoliday("20040601");
     // Return the matrix :)
     return $this->_dumpMatrix();
  }
}

?>