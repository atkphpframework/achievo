<?php
atkimport("modules.scheduler.views.scheduler_view");


class scheduler_dayview extends scheduler_view
{
  var $m_view = "day";
  /**
   * @var array $m_itemMatrix Array with items for the dayview
   */
  var $m_itemMatrix = array();

  /**
   * @var array $m_matrixInfo Array with the matrix for the dayview
   */
  var $m_matrixInfo = array();

  var $m_contractInfo = array();
  var $m_allDayHours = array();
  var $m_workHours = array();
  var $m_maxcols = 15;
  var $m_weekday_arr = array(1,2,4,8,16,32,64);

  

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
     atk_var_dump($item,"SET MATRIX ITEM");
     $begin = $this->m_model->correct_time(date("H:i",$item["startdate"]),$this->m_userPrefs["timeschedule"]);
     $einde = $this->m_model->correct_time(date("H:i",$item["enddate"]),$this->m_userPrefs["timeschedule"],"up");

     $duration = $this->_duration($begin, $einde);
     $appoints = (array_key_exists($begin,$this->m_itemMatrix)?count($this->m_itemMatrix[$begin]):0);

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

    $allDayItems = $this->m_model->getAlldayItems();
    $appointments = $this->m_model->getAppointmentItems($this->m_model->getViewDate());
    if(count($appointments)>0)
    {
      foreach($appointments as $item)
      {
        $this->_setMatrix($item);
      }
    }
    $colors = $this->m_model->getLegendColors();
    $holiday = &getNode("scheduler.scheduler_holidays");
    $contractInfo = $this->m_model->getContractInfo();
    $cols   = $this->_getCols();


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
    $buff.='<table border=0 width="100%" cellspacing="2"><tr><td valign="top">';
    // if we have allday items show 'em in a seperate table
    if (array_key_exists($this->m_model->m_viewdate,$allDayItems) && count($allDayItems[$this->m_model->m_viewdate]) > 0)
    {
      $schedulerItemObj = &atkinstance("module.scheduler.utils.scheduleritem");
      $buff .= "<table border='0' cellpadding='1' cellspacing='1' width='100%'>\n";
      $buff .= "<tr>\n";
      $buff .= "<td colspan='$aCols' bgcolor='#D0D0D0' valign='top' align='left'>".atkText("allday_events","scheduler")."</td>\n";
      $buff .= "</tr>\n";

      for ($i=0;$i<=count($allDayItems[$this->m_model->m_viewdate])-1;$i++)
      {
        $buff .= "<tr>\n";
        $buff .= '<td colspan="'.$aCols.'" valign="top" align="left">';
        $buff.= $schedulerItemObj->render($allDayItems[$this->m_model->m_viewdate][$i],true,"",true);
        $buff .= "</td>";
        $buff .= "</tr>";
      }
      $buff .= "</table><br>";
    }

    // normal appointments
    atk_var_dump($this->m_itemMatrix,"ITEM MATRIX");
    atk_var_dump($this->m_matrixInfo,"MATRIX INFO");
    $buff .= "<table border='0' cellpadding='1' cellspacing='0' width='100%'>\n";
    $lastHour="";
    $newHour=true;
    $theme = &atkTheme::getInstance();
    $defaulthighlight = $theme->getAttribute("highlight");
    for ($i=0;$i<count($this->m_allDayHours);$i++)
    {
      $time    = $this->m_allDayHours[$i];
      $items   = (array_key_exists($time,$this->m_itemMatrix)?$this->m_itemMatrix[$time]:array());
      atk_var_dump($items,"ITEMS");
      $now     = $this->m_model->getYear()."-".$this->m_model->getMonth()."-".$this->m_model->getDay();
      $addurl  = atkSelf()."?atkaction=add&atknodetype=scheduler.scheduler&date=$now&time=$time";
      if(in_array($time,$this->m_workHours))
      {
        $weekday_number = date("w",mktime(12,0,0,$this->m_model->getMonth(),$this->m_model->getDay(),$this->m_model->getYear()));
        if(hasFlag($contractInfo["workdays"],$this->m_weekday_arr[$weekday_number]) && !$holiday->isHoliday($this->m_model->m_viewdate,"weekend"))
        {
          $empty_bgcolor= $this->m_userPrefs["scheduler_emptyworkhourscolor"];
        }
        else
        {
          $empty_bgcolor=$this->m_userPrefs["scheduler_emptycolor"];
        }
      }
      else
      {
        $empty_bgcolor=$this->m_userPrefs["scheduler_emptycolor"];
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
        $schedulerItemObj = &atkinstance("module.scheduler.utils.scheduleritem");
        for ($j=0;$j<$cols;$j++)
        {
          $item      = (array_key_exists($j,$items)?$items[$j]:'');
          atk_var_dump($item,"Item $j");
          if(trim($item) != "")
          {
            $timeLap   = $this->m_matrixInfo[$time][$j]["duration"];
            $info      = $this->m_matrixInfo[$time][$j]["info"];
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
              /*
              if ($info["private"] == 1 && $info["owner"] != $this->owner)
              {
                $bgcolor = $this->busyBgColor;
              }
              else
              {
                $bgcolor = $colors[$info["category"]]["back"];
              }
              */
              $bgcolor = $colors[$info["category"]]["back"];
              $txt_color = $colors[$info["category"]]["front"];

              $buff .= "<td $rowspan height='100%' valign='top' width='$cWidth' align='left' style='background-color: ".$bgcolor.";color: ".$txt_color.";'>";
              $buff .= $schedulerItemObj->render($info,true);
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
    $buff.='</td><td width="150" valign="top">';
    $buff.=$this->getSideBar();
    $buff.="</td></tr></table>";
    $buff.="<br><br>\n";
    $buff.= $this->m_model->renderLegend();
    return $buff;
  }
  
  function getSideBar()
  {
    $buff=$this->m_model->_getSmallCalendar(substr($this->m_model->m_small_viewdate,8,2),substr($this->m_model->m_small_viewdate,5,2),substr($this->m_model->m_small_viewdate,0,4));
    $buff.="<br>";
    $next_date = date("Y-m-d",mktime(12,0,0,substr($this->m_model->m_small_viewdate,5,2)+1,substr($this->m_model->m_small_viewdate,8,2),substr($this->m_model->m_small_viewdate,0,4)));
    $buff.=$this->m_model->_getSmallCalendar(substr($next_date,8,2),substr($next_date,5,2),substr($next_date,0,4),false,true);
    $buff.="<br>";
    $buff.=$this->m_model->_getEmployeesCheckboxesForm();
    return $buff;
  }

  /**
   * navigate through days using prev/next day link
   * @returns html code containing links for previous day, today and next day
   */
  function _getNav()
  {

    $theme = &atktheme::getInstance();
    $prevTime = date("Y-m-d", mktime(0,0,0,$this->m_model->getMonth(), $this->m_model->getDay()-1, $this->m_model->getYear()));
    $prevUrl  = atkSelf()."?view=".$this->m_view."&viewdate=$prevTime";
    $prevImg ='<img src="'.$theme->imgPath("arrow_left.gif","scheduler").'" border=0 width=16 height=16 title="'.atktext("previous_day","scheduler").'" alt="'.atktext("previous_day","scheduler").'">';


    $currentTime = date("Y-m-d");
    $viewTime = date("Y-m-d", mktime(0,0,0,$this->m_model->getMonth(), $this->m_model->getDay(), $this->m_model->getYear()));
    $currentUrl  = atkSelf()."?view=".$this->m_view."&viewdate=$currentTime";

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->m_model->getMonth(), $this->m_model->getDay()+1, $this->m_model->getYear()));
    $nextUrl  = atkSelf()."?view=".$this->m_view."&viewdate=$nextTime";
    $nextImg ='<img src="'.$theme->imgPath("arrow_right.gif","scheduler").'" border=0 width=16 height=16 title="'.atktext("next_day","scheduler").'" alt="'.atktext("next_day","scheduler").'">';

    return href($prevUrl, $prevImg)."&nbsp;".href($nextUrl, $nextImg)."&nbsp;".'<button id="scheduler_today" onclick="window.location=\''.atkSessionManager::sessionUrl($currentUrl).'\'"'.($viewTime==$currentTime?' disabled':'').'>'.atkText("today","scheduler").'</button>';
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
  function _render()
  {
     $page = &atkPage::getInstance();
     $page->register_script(atkconfig("atkroot")."atk/javascript/recordlist.js");
     $contractInfo = $this->m_model->getContractInfo();

     // Fill $allDayHours and $workHours
     for($i=0;$i<=23;$i++)
     {
        for($j=0;$j<=55;$j+=$this->m_userPrefs["timeschedule"])
        {
          $time = sprintf("%02d:%02d",$i,$j);
          $this->m_allDayHours[] =  $time;
          if($time>=$this->m_model->correct_time($contractInfo["workstarttime"],$this->m_userPrefs["timeschedule"],"down") && $time<$this->m_model->correct_time($contractInfo["workendtime"],$this->m_userPrefs["timeschedule"],"up"))
            $this->m_workHours[] = $time;
        }
     }

     $this->m_viewTitle = "<b>".atkText(strtolower(date("l", mktime(0, 0, 0, $this->m_model->getMonth(), $this->m_model->getDay(), $this->m_model->getYear()))),"atk")." ".$this->m_model->getDay()."-".$this->m_model->getMonth()."-".$this->m_model->getYear()."</b>";

     $this->m_model->getItems();

     // get scheduler Legend and Item colors
     $this->m_model->getLegend();

     // Return the matrix
     return $this->_dumpMatrix();
  }
}

?>