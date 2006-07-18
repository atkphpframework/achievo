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
  function _dumpMatrix()
  {
    $tmp="";
    $holiday = &getNode("scheduler.scheduler_holidays");
    if($holiday->isHoliday($this->m_viewdate))
    {
      $holidayInfo = $holiday->getHolidayInfo($this->m_viewdate);
      $tmp.=$holidayInfo["name"]."<br>";
    }
    if(count($this->m_allDayItems[$this->m_viewdate])>0)
    {
      for($i=0;$i<count($this->m_allDayItems[$this->m_viewdate]);$i++)
      {
        $tmp.=$this->renderItem($this->m_allDayItems[$this->m_viewdate][$i]);
      }
    }

    if(count($this->m_appointmentItems[$this->m_viewdate])>0)
    {
      for($i=0;$i<count($this->m_appointmentItems[$this->m_viewdate]);$i++)
      {
        $tmp.=$this->renderItem($this->m_appointmentItems[$this->m_viewdate][$i]);

      }
    }
    if($tmp=="") return text('pim_scheduler_empty');
    return $tmp;
  }

 /**
  * Render the view
  * @return string A renderd view
  */
  function renderView()
  {
    $this->getItems();
    return $this->_dumpMatrix();
  }

}


?>