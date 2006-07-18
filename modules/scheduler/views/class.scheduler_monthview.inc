<?php
atkimport("modules.scheduler.views.scheduler_view");

class scheduler_monthview extends scheduler_view
{
 /**
  * scheduler monthview constructor
  */
  function scheduler_monthview()
  {
    $this->setView("month");
  }
 /**
  * navigate through months using prev/next month link
  * @returns html code containing links for previous month, current month and next month
  */
  function _getNav()
  {
    $prevTime = date("Y-m-d", mktime(0,0,0,$this->m_month-1, 1, $this->m_year));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=$prevTime&dayview=".$this->showGlobal;

    $thisTime = date("Y-m")."-1";
    $thisUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=$thisTime&dayview=".$this->showGlobal;

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->m_month+1, 1, $this->m_year));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=$nextTime&dayview=".$this->showGlobal;

    $this->title = "<b>".text(strtolower(date("F", mktime(0,0,0,$this->m_month,1,$this->m_year))))." ".$this->m_year."</b>";

    return href($prevUrl, text("previous_month"))."&nbsp;".href($thisUrl, text("this_month"))."&nbsp;".href($nextUrl, text("next_month"));

  }
 /**
  * the actual displaying function
  * @return string Returns a renderd day matrix
  */
  function _dumpMatrix()
  {
     if($day=="")
    {
       $day = $this->m_day;
       $month = $this->m_month;
       $year = $this->m_year;
    }
    $days = array();
    $days[0] = text("Su");
    $days[1] = text("Mo");
    $days[2] = text("Tu");
    $days[3] = text("We");
    $days[4] = text("Th");
    $days[5] = text("Fr");
    $days[6] = text("Sa");
    // On what day does our calendar start (0 or 1)
    $daystart=1;

    $months = array();
    $months[0] = text("januari");
    $months[1] = text("februari");
    $months[2] = text("march");
    $months[3] = text("april");
    $months[4] = text("may");
    $months[5] = text("june");
    $months[6] = text("Juli");
    $months[7] = text("august");
    $months[8] = text("september");
    $months[9] = text("october");
    $months[10] = text("november");
    $months[11] = text("december");

    $days_in_month = mktime (0,0,0,$month+1,0,$year);
    $days_in_month = (strftime ("%d",$days_in_month));

    $first_day_of_month = date("w", mktime(0,0,0,$month,1,$year));

    $previous_year = date("Y", mktime(0,0,0,$month-1,1,$year));
    $previous_month= date("m", mktime(0,0,0,$month-1,1,$year));

    $next_day = 1;
    $next_year = date("Y", mktime(0,0,0,$month+1,1,$year));
    $next_month= date("m", mktime(0,0,0,$month+1,1,$year));

    $tmp = $this->_view_header();
    $tmp.= "<b>".$months[$month-1].' '.$year."</b>";
    $tmp.='<table width="90%">
             <tr>
              <td valign="top">';
    $tmp.='<table border="0" cellpadding="0" cellspacing="0" width="90%">';
    $tmp.='<tr style="background-color: #DDDDDD;"><td width="40">Week</td>';

    $day_number = $daystart;
    for ($i=0;$i<7;$i++)
    {
    	if ($day_number == 7) $day_number = 0;
    	$tmp.='<td align="center">'.$days[$day_number].'</td>';
    	$day_number++;
    }
    $tmp.='</tr>';

    $cellcount=$first_day_of_month-$daystart;
    if ($cellcount < 0) $cellcount = 7 + $cellcount;
    $bg_color="#FFFFFF";
    $tmp.='<tr style="background-color: '.$bg_color.'; height: 100px;">';
    //$week_number = intval(strftime("%V",mktime(0,0,0,$month,1,$year)));
    $week_number = date("W", mktime(0,0,0,$month,1,$year));

    $tmp.='<td><i><a class="small" href="javascript:post_date_week(1,'.$month.', '.$year.')">'.$week_number.'</a></i>&nbsp;</td>';
    for ($i=1;$i<=$cellcount;$i++)
    {
    	$tmp.='<td>&nbsp;</td>';
    }
    atk_var_dump($this->m_appointmentItems);
   	$holiday = &getNode("scheduler.scheduler_holidays");

    for ($i=1;$i<=$days_in_month;$i++)
    {
     	$date = $year."-".sprintf("%02d",$month)."-".sprintf("%02d",$i);

    	if ($cellcount == 7)
    	{
    	  if($bg_color=="#FFFFFF") { $bg_color="#EEEEEE"; } else { $bg_color="#FFFFFF"; }
    		$tmp.="</tr><tr style=\"background-color: $bg_color; height: 100px;\">\n";
    		$week_number++;
    		$tmp.='<td style="border: 1px solid #000000;"><i>'.href($_SERVER["PHP_SELF"]."?view=week&viewdate=".$date,$week_number).'</i>&nbsp;</td>';
    		$cellcount=0;
    	}
    	$cellcount++;
    	if (($i == $current_day) && ($year == $current_year) && ($month==$current_month))
    		$class="HoverLinkSelected";
    	else
    		$class="HoverLink";

    	$date = $year."-".sprintf("%02d",$month)."-".sprintf("%02d",$i);
    	if($date==date("Y-m-d"))
    		$class="calendarToday";
    	else
    		$class="calendarNormal";

    	$holiday_title = '';
    	if($holiday->isHoliday($date))
    	{
    	    $holidayInfo = $holiday->getHolidayInfo($date);
    			$holiday_title = ' title="'.$holidayInfo["name"].'"';
    			if($class=="calendarToday") { $class="calendarHolidayToday"; } else { $class="calendarHoliday"; }
    	}

    	$tmp.='<td align="left" valign="top" style="border: 1px solid #000000;">[&nbsp;'.href($_SERVER["PHP_SELF"]."?view=day&viewdate=".$date,$i,'','','class="'.$class.'"'.$holiday_title).'&nbsp;]&nbsp;';
    	$tmp.=href($_SERVER["PHP_SELF"]."?atkaction=add&atknodetype=scheduler.scheduler&viewdate=".$date,text("additem")).'<br>';
      if($holiday->isHoliday($date))
      {
        $holidayInfo = $holiday->getHolidayInfo($date);
        $tmp.=$holidayInfo["name"]."<br>";
      }
    	// Allday appointments
    	if(is_array($this->m_allDayItems[$date]))
    	{
    	  for($j=0;$j<count($this->m_allDayItems[$date]);$j++)
    	  {
    	    $tmp.=$this->RenderItem($this->m_allDayItems[$date][$j],true,$date);
    	  }
    	}
    	// Other appointments
    	if(is_array($this->m_appointmentItems[$date]))
    	{
    	  for($j=0;$j<count($this->m_appointmentItems[$date]);$j++)
    	  {
    	    $tmp.=$this->RenderItem($this->m_appointmentItems[$date][$j],true,$date);
    	  }
    	}
    	$tmp.='</td>';
    }
    $remaining_cells = 7-$cellcount;

    for ($i=0;$i<$remaining_cells;$i++)
    {
    	$tmp.='<td>&nbsp;</td>';
    }

    $tmp.='</tr>';
    $tmp.='</table>';
    $tmp.='<td width=200 valign="top">';
    $tmp.=$this->_getSmallCalendar(substr($this->m_small_viewdate,8,2),substr($this->m_small_viewdate,5,2),substr($this->m_small_viewdate,0,4));
    $tmp.="<br>";
    $next_date = date("Y-m-d",mktime(12,0,0,substr($this->m_small_viewdate,5,2)+1,substr($this->m_small_viewdate,8,2),substr($this->m_small_viewdate,0,4)));
    $tmp.=$this->_getSmallCalendar(substr($next_date,8,2),substr($next_date,5,2),substr($next_date,0,4),false);


    $tmp.='  </td>
             </tr>
             </table>';
    return $tmp;
  }

 /**
  * Render the view
  * @return string A renderd view
  */
  function renderView()
  {
    $this->_setUserSettings();
    $startdate = date("Y-m-d",mktime(12,0,0,$this->m_month,1,$this->m_year));
    $enddate = date("Y-m-d",mktime(12,0,0,$this->m_month+1,0,$this->m_year));
    atkdebug("Monthview: $startdate - $enddate");
    $this->getItems($startdate,$enddate);
    $this->getLegend();

    return $this->_dumpMatrix();
  }


}






?>