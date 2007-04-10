<?php
atkimport("modules.scheduler.views.scheduler_view");

class scheduler_yearview extends scheduler_view
{
 /**
  * scheduler yearview constructor
  */
  function scheduler_yearview()
  {
    $this->setView("year");
  }

 /**
  * navigate through years using prev/next year link
  * @returns html code containing links for previous year, current year and next year
  */
  function _getNav()
  {
    $prevTime = date("Y-m-d", mktime(0,0,0,1, 1, $this->m_year-1));
    $prevUrl  = $_SERVER["PHP_SELF"]."?view=year&viewdate=$prevTime&dayview=".$this->showGlobal;

    $thisTime = date("Y-m-d");
    $thisUrl  = $_SERVER["PHP_SELF"]."?view=year&viewdate=$thisTime&dayview=".$this->showGlobal;

    $nextTime = date("Y-m-d", mktime(0,0,0,1, 1, $this->m_year+1));
    $nextUrl  = $_SERVER["PHP_SELF"]."?view=year&viewdate=$nextTime&dayview=".$this->showGlobal;

    $this->title = "<b>".atktext(strtolower(date("F", mktime(0,0,0,$this->m_month,1,$this->m_year))))." ".$this->m_year."</b>";

    return href($prevUrl, atktext("previous_year"))."&nbsp;".href($thisUrl, atktext("this_year"))."&nbsp;".href($nextUrl, atktext("next_year"));

  }

 /**
  * the actual displaying function
  * @return string Returns a renderd year matrix
  */
  function _dumpMatrix()
  {
    $tmp = $this->_view_header();
    $tmp.= '<table cellpadding=5>
            <tr>
              <td valign="top">'.$this->_getSmallCalendar(1,1,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,2,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,3,$this->m_year,false).'</td>
            </tr>
            <tr>
              <td valign="top">'.$this->_getSmallCalendar(1,4,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,5,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,6,$this->m_year,false).'</td>
            </tr>
            <tr>
              <td valign="top">'.$this->_getSmallCalendar(1,7,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,8,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,9,$this->m_year,false).'</td>
            </tr>
            <tr>
              <td valign="top">'.$this->_getSmallCalendar(1,10,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,11,$this->m_year,false).'</td>
              <td valign="top">'.$this->_getSmallCalendar(1,12,$this->m_year,false).'</td>
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
    $this->getItems('2003-01-01','2003-12-31');
    return $this->_dumpMatrix();
  }

}




?>