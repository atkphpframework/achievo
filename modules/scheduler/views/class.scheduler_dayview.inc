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
   *
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
   * Calculate the duration of a calendar item
   *
   * @param string $begin (begin time, should be something like 09:00 or 10:30)
   * @param string $eind (end time, should be something like 09:00 or 10:30)
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
   *
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
    *
    * @param array $item Full record containing all info about the specified appointment
    */
   function _setMatrix($item)
   {
     $begin = $this->_correct_time(date("H:i",$item["startdate"]));
     $einde = $this->_correct_time(date("H:i",$item["enddate"]),"up");

     $duration = $this->_duration($begin, $einde);
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
   * The actual displaying function
   *
   * @return string Returns a renderd day matrix
   */
  function _dumpMatrix()
  {
    $buff   = $this->_view_header();
    $cols   = $this->_getCols();
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
    if (count($this->m_allDayItems[$this->m_viewdate]) > 0)
    {

      $buff .= "<table border='0' cellpadding='1' cellspacing='1' width='90%'>\n";
      $buff .= "<tr>\n";
      $buff .= "<td colspan='$aCols' bgcolor='#D0D0D0' valign='top' align='left'>".atkText("allday_events","scheduler")."</td>\n";
      $buff .= "</tr>\n";

      for ($i=0;$i<=count($this->m_allDayItems)-1;$i++)
      {
        $buff .= "<tr>\n";
        $buff .= '<td colspan="'.$aCols.'" valign="top" align="left">';
        $buff.= $this->renderItem($this->m_allDayItems[$this->m_viewdate][$i],true,"",true);
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
      $now     = $this->m_year."-".$this->m_month."-".$this->m_day;
      $addurl  = $_SERVER["PHP_SELF"]."?atkaction=add&atknodetype=scheduler.scheduler&date=$now&time=$time";
      if(in_array($time,$this->m_workHours))
      {
        $weekday_number = date("w",mktime(12,0,0,$this->m_month,$this->m_day,$this->m_year));
        if(hasFlag($this->m_workdays,$this->m_weekday_arr[$weekday_number]) && !$holiday->isHoliday($this->m_viewdate,"weekend"))
        {
          $empty_bgcolor= $this->m_emptyWorkHoursColor;
        }
        else
        {
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
        $buff .= "<td style='background-color: ".$empty_bgcolor."; border-top: ".($newHour?2:1)."px solid #FFFFFF;border-bottom: 1px solid #D0D0D0;  cursor: pointer; cursor: hand;' width='$cWidth' bgcolor='".$empty_bgcolor."'  onclick=\"document.location.href='".session_url($addurl,SESSION_NESTED)."'\" onmouseover=\"highlightrow(this, '".$defaulthighlight."')\" onmouseout=\"resetrow(this)\">&nbsp;</td>\n";
      }
      else
      {
        for ($j=0;$j<$cols;$j++)
        {
          $item      = $items[$j];
          $timeLap   = $this->m_matrixInfo[$time][$j]["duration"];
          $info      = $this->m_matrixInfo[$time][$j]["info"];
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
                $bgcolor = $this->busyBgColor;
              }
              else
              {
                $bgcolor = $colors[$info["category"]]["back"];
              }
              $txt_color = $colors[$info["category"]]["front"];

              $buff .= "<td $rowspan height='100%' valign='top' width='$cWidth' align='left' style='background-color: ".$bgcolor.";color: ".$txt_color.";'>";
              $buff .= $this->renderItem($info,true);
              $buff .= "</td>";
            }
          }
          else
          {
           $buff .= "<td style='background-color: ".$empty_bgcolor."; border-top: 1px solid #D0D0D0;border-bottom: 1px solid #D0D0D0;border-right: 0px solid #D0D0D; cursor: pointer; cursor: hand;' width='$cWidth' onclick=\"document.location.href='".session_url($addurl,SESSION_NESTED)."'\" onmouseover=\"highlightrow(this, '".$defaulthighlight."')\" onmouseout=\"resetrow(this)\">&nbsp;</td>\n";
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
    $cols = 0;
    while (list($key, $val) = each($this->m_itemMatrix))
    {
      $a = count($val);
      if ($cols < $a) { $cols = $a; }
    }
    return $cols;
  }

  /**
   * Render the view
   *
   * @return string A rendered view
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

     // Return the matrix
     return $this->_dumpMatrix();
  }
}

?>