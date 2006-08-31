<?php
atkimport("modules.scheduler.views.scheduler_view");

class scheduler_pimview extends scheduler_view
{
  
 /**
  * scheduler pimview constructor
  */
  function scheduler_pimview()
  {
    $this->setView("pimview");
  }

 /**
  * the actual displaying function
  * @return string Returns a renderd day matrix
  */
  function _dumpMatrix($showdays=1)
  {
    $tmp = $this->getDaySelector($showdays);
    
    $items = "";
    for($j=1;$j<=$showdays;$j++)
    {
      $showtime = time()+(($j-1)*86400);
      $showdate = date("Y-m-d",$showtime);
      $totalAllDayItems = count($this->m_allDayItems[$showdate]);
      $totalAppointmentItems = count($this->m_appointmentItems[$showdate]);
      $totalItems = $totalAllDayItems+$totalAppointmentItems;
      if($totalItems==0) continue;
      
      $items.="<br><b>".($j==1?atktext("today","scheduler").",":($j==2?atktext("tommorow","scheduler").",":"[+$j]"))." ".date("l d M",$showtime)." ($totalItems)</b><br>";
      $holiday = &atkGetNode("scheduler.scheduler_holidays");
      if($holiday->isHoliday($showdate))
      {
        $holidayInfo = $holiday->getHolidayInfo($showdate);
        $items.=$holidayInfo["name"]."<br>";
      }
      if($totalAllDayItems>0)
      {
        for($i=0;$i<$totalAllDayItems;$i++)
        {
          $items.=$this->renderItem($this->m_allDayItems[$showdate][$i],true,$showdate,true);
        }
      }
  
      if($totalAppointmentItems>0)
      {
        for($i=0;$i<$totalAppointmentItems;$i++)
        {
          $items.=$this->renderItem($this->m_appointmentItems[$showdate][$i],true,$showdate,true);
  
        }
      }
    }
    if($items=="") return $tmp.atkText("pim_scheduler_empty", "scheduler");
    return $tmp.$items;
  }

 /**
  * Render the view
  * @return string A renderd view
  */
  function renderView()
  {
    $this->getLegend();
    $showdays = $this->getShowDays();
    $startdate = $this->m_viewdate;
    $enddate = date("Y-m-d",mktime(12,0,0,$this->m_month,($this->m_day+$showdays),$this->m_year));
    
    $this->getItems($startdate,$enddate);
    return $this->_dumpMatrix($showdays);
  }
  
  function getShowDays()
  {
    global $ATK_VARS;
    
    if(isset($ATK_VARS["days"]))
    {
      $sel=$ATK_VARS["days"];
    }
    elseif($_COOKIE["schedulerpimdays"])
    {
      $sel=$_COOKIE["schedulerpimdays"];
    }
    else 
    {
      $sel=1;
    }
    setcookie("schedulerpimdays",$sel);
    return $sel;    
  }
  
  function getDaySelector($showdays)
  {
    $tmp='<form action="" method="post">';
    $tmp.='Show <select name="days">';
    for($i=1;$i<15;$i++)
    {
      $tmp.='<option value="'.$i.'"'.($showdays==$i?' selected':'').'>'.($i==1?atktext("today","scheduler"):$i." ".atktext("days","scheduler")).'</option>';
    }
    $tmp.='</select>&nbsp;<input type="submit" value="view"></form>';
    return $tmp;
  }

}


?>