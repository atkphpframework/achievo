<?php
atkimport("modules.scheduler.views.scheduler_view");

class scheduler_pimview extends scheduler_view
{
  var $m_view = "pim";
 
  
 /**
  * the actual displaying function
  * @return string Returns a renderd day matrix
  */
  function _dumpMatrix($showdays=1)
  {
    $schedulerItemObj = null;
    $tmp = $this->getDaySelector($showdays);
    
    $items = "";
    $alldayItems = $this->m_model->getAllDayItems();
    $appointmentItems = $this->m_model->getAppointmentItems();
    for($j=1;$j<=$showdays;$j++)
    {
      $showtime = time()+(($j-1)*86400);
      $showdate = date("Y-m-d",$showtime);
      $totalAllDayItems = (array_key_exists($showdate,$alldayItems)?count($alldayItems[$showdate]):0);
      $totalAppointmentItems = (array_key_exists($showdate,$appointmentItems)?count($appointmentItems[$showdate]):0);
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
        if(is_null($schedulerItemObj)) $schedulerItemObj = &atkinstance("module.scheduler.utils.scheduleritem");
        for($i=0;$i<$totalAllDayItems;$i++)
        {
          $items.=$schedulerItemObj->render($alldayItems[$showdate][$i],true,$showdate,true);
        }
      }
  
      if($totalAppointmentItems>0)
      {
        if(is_null($schedulerItemObj)) $schedulerItemObj = &atkinstance("module.scheduler.utils.scheduleritem");
        for($i=0;$i<$totalAppointmentItems;$i++)
        {
          $items.=$schedulerItemObj->render($appointmentItems[$showdate][$i],true,$showdate,true);
        }
      }
    }
    if($items=="") return $tmp.'<br /><br />'.atkText("pim_scheduler_empty", "scheduler").'<br /><br />';
    return $tmp.$items;
  }

 /**
  * Render the view
  * @return string A renderd view
  */
  function _render()
  {
    $this->m_model->getLegend();
    $showdays = $this->getShowDays();
    $startdate = $this->m_model->getViewDate();
    $enddate = date("Y-m-d",mktime(12,0,0,$this->m_model->getMonth(),($this->m_model->getDay()+$showdays),$this->m_model->getYear()));
    
    $this->m_model->getItems($startdate,$enddate);
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
    $tmp.=session_form();
    $tmp.=atktext("show","scheduler").' <select name="days">';
    for($i=1;$i<15;$i++)
    {
      $tmp.='<option value="'.$i.'"'.($showdays==$i?' selected':'').'>'.($i==1?atktext("today","scheduler"):$i." ".atktext("days","scheduler")).'</option>';
    }
    $tmp.='</select>&nbsp;<input type="submit" value="'.atktext('view',"scheduler").'"></form>';
    return $tmp;
  }

}


?>