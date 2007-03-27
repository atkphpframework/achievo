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
   * Navigate through months using prev/next month link
   *
   * @return html code containing links for previous month, current month and next month
   */
  function _getNav()
  {
    $prevTime = date("Y-m-d", mktime(0,0,0,$this->m_month-1, 1, $this->m_year));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=$prevTime&user=".$this->m_viewUser;

    $thisTime = date("Y-m")."-1";
    $thisUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=$thisTime&user=".$this->m_viewUser;

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->m_month+1, 1, $this->m_year));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=month&viewdate=$nextTime&user=".$this->m_viewUser;

    $this->title = "<b>".atktext(strtolower(date("F", mktime(0,0,0,$this->m_month,1,$this->m_year))))." ".$this->m_year."</b>";

    return href($prevUrl, atktext("previous_month"))."&nbsp;".href($thisUrl, atktext("this_month"))."&nbsp;".href($nextUrl, atktext("next_month"));

  }

  /**
   * The actual displaying function
   *
   * @return string Returns a renderd day matrix
   */
  function _dumpMatrix()
  {
    $month = $this->m_month;
    $year = $this->m_year;

    $days = array();
    $days[0] = atkText("sun");
    $days[1] = atkText("mon");
    $days[2] = atkText("tue");
    $days[3] = atkText("wed");
    $days[4] = atkText("thu");
    $days[5] = atkText("fri");
    $days[6] = atkText("sat");
    // On what day does our calendar start (0 or 1)
    $daystart=$this->m_calendarStartDay;

    $months = array();
    $months[0] = atkText("january");
    $months[1] = atkText("february");
    $months[2] = atkText("march");
    $months[3] = atkText("april");
    $months[4] = atkText("may");
    $months[5] = atkText("june");
    $months[6] = atkText("july");
    $months[7] = atkText("august");
    $months[8] = atkText("september");
    $months[9] = atkText("october");
    $months[10] = atkText("november");
    $months[11] = atkText("december");

    atkimport("modules.scheduler.utils.moonphases");

    $days_in_month = strftime("%d", mktime(0,0,0,$month+1,0,$year));
    $first_day_of_month = date("w", mktime(0,0,0,$month,1,$year));

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
    $week_number =$this->getWeekNumber(2,$month,$year);

    $date = $year."-".sprintf("%02d",$month)."-".sprintf("%02d",1);
    $tmp.='<td style="border: 1px solid #000000;"><i>'.href($_SERVER["PHP_SELF"]."?view=week&viewdate=".$date,$week_number,SESSION_DEFAULT,false,'title="'.atktext("weekview","scheduler").'"').'</i>&nbsp;</td>';
    for ($i=1;$i<=$cellcount;$i++)
    {
    	$tmp.='<td>&nbsp;</td>';
    }
    $theme = &atktheme::getInstance();
    for ($i=1;$i<=$days_in_month;$i++)
    {
     	$date = $year."-".sprintf("%02d",$month)."-".sprintf("%02d",$i);

    	if ($cellcount == 7)
    	{
    	  if($bg_color=="#FFFFFF") { $bg_color="#EEEEEE"; } else { $bg_color="#FFFFFF"; }
    		$tmp.="</tr><tr style=\"background-color: $bg_color; height: 100px;\">\n";
    		$week_number++;
    		$tmp.='<td style="border: 1px solid #000000;"><i>'.href($_SERVER["PHP_SELF"]."?view=week&viewdate=".$date,sprintf("%02d",$week_number),SESSION_DEFAULT,false,'title="'.atktext("weekview","scheduler").'"').'</i>&nbsp;</td>';
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


    	$tmp.='<td align="left" valign="top" style="border: 1px solid #000000;">[&nbsp;'.href($_SERVER["PHP_SELF"]."?view=day&viewdate=".$date,$i,'','','class="'.$class.'"'.$holiday_title).'&nbsp;]&nbsp;';
      if($this->m_showLunarPhases)
      {
        $timestamp = mktime(12,0,0,$month,$i,$year);
    	  $moonphase = $this->getMoonPhaseIcon($timestamp);
        $tmp.=($moonphase!=""?"$moonphase":"");
      }
    	$addicon = '<img src="'.$theme->imgPath("add.gif","scheduler").'" border="0" width="15" height="15" style="float: right;" title="'.atktext("additem").'" alt="'.atktext("additem").'">';
    	$tmp.=href($_SERVER["PHP_SELF"]."?atkaction=add&atknodetype=scheduler.scheduler&viewdate=".$date,$addicon,SESSION_NESTED).'<br>';

    	// Allday appointments
    	if(is_array($this->m_allDayItems[$date]))
    	{
    	  for($j=0;$j<count($this->m_allDayItems[$date]);$j++)
    	  {
    	    $tmp.=$this->RenderItem($this->m_allDayItems[$date][$j],true,$date,true);
    	  }
    	}
    	// Other appointments
    	if(is_array($this->m_appointmentItems[$date]))
    	{
    	  for($j=0;$j<count($this->m_appointmentItems[$date]);$j++)
    	  {
    	    $tmp.=$this->RenderItem($this->m_appointmentItems[$date][$j],true,$date,true);
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
             </table><br><br>
             '.$this->m_legend;
    return $tmp;
  }

  /**
   * Return moonPhase icon for a given date
   *
   * @param int $timestamp
   * @return string An html image string or an empty string
   */
  function getMoonPhaseIcon($timestamp)
  {

    $moonphase = moonphases::getMoonPhase($timestamp);
    if($moonphase=="") return "";
    switch($moonphase)
    {
      case "ne": $text = atktext("full_moon","scheduler");
                 break;
      case "ny": $text = atktext("no_moon","scheduler");
                 break;
      case "ne2": $text = atktext("half_moon","scheduler");
                 break;
      case "ny2": $text = atktext("half_moon","scheduler");
                 break;
    }
    $theme = &atktheme::getInstance();
    return '<img src="'.$theme->imgPath($moonphase.".gif","scheduler").'" width=15 height=15 title="'.$text.'" alt="'.$text.'">';
  }

  /**
   * Render the view
   *
   * @return string A renderd view
   */
  function renderView()
  {
    $this->_setUserSettings();
    $startdate = date("Y-m-d",mktime(12,0,0,$this->m_month,1,$this->m_year));
    $enddate = date("Y-m-d",mktime(12,0,0,$this->m_month+1,0,$this->m_year));
    $this->getItems($startdate,$enddate);
    $this->getLegend();

    return $this->_dumpMatrix();
  }
}

?>