<?php
atkimport("modules.scheduler.views.scheduler_view");

class scheduler_weekview extends scheduler_view
{
  var $m_weekdates = array();
  var $m_weeknumber = "";


 /**
  * scheduler weekview constructor
  */
  function scheduler_weekview()
  {
    $this->setView("week");
  }

 /**
  * navigate through weeks using prev/next week link
  * @returns html code containing links for previous week, current week and next week
  */
  function _getNav()
  {
    global $PHP_SELF;

    $prevTime = $this->m_weekdates[0]['date'];
    $prevUrl  = $PHP_SELF."?view=week&viewdate=$prevTime&user=".$this->showGlobal;

    $nextTime = $this->m_weekdates[8]['date'];
    $nextUrl  = $PHP_SELF."?view=week&viewdate=$nextTime&user=".$this->showGlobal;

    $currentUrl = $PHP_SELF."?view=week&viewdate=".date("Y-m-d")."&user=".$this->showGlobal;

    $this->m_weeknumber = strftime("%V", mktime(0,0,0,$this->m_month, $this->m_day, $this->m_year));
    $this->m_viewTitle = "<b>".text("week")."&nbsp;".$this->m_weeknumber."</b>";

    return href($prevUrl, text('previousweek'))."&nbsp;".href($currentUrl,text("currentweek"))."&nbsp;".href($nextUrl, text('nextweek'));

  }
  /**
  * Renders a calendar weekdays
  * @param array $weekday Calendar items for a weekday
  * @return string HTML code with a renderd calendar weekday
  */
  function renderItems($weekday)
  {
    $tmp.= "";
    $holiday = &getNode("scheduler.scheduler_holidays");
    if($holiday->isHoliday($weekday["date"]))
    {
      $holidayInfo = $holiday->getHolidayInfo($weekday["date"]);
      $tmp.=$holidayInfo["name"]."<br>";
    }
    for($i=0;$i<count($weekday["alldayitems"]);$i++)
    {
      $tmp.=$this->renderItem($weekday["alldayitems"][$i],true,$weekday["date"]);
    }

    for($i=0;$i<count($weekday["appointmentitems"]);$i++)
    {
      $tmp.=$this->renderItem($weekday["appointmentitems"][$i],true,$weekday["date"]);
    }

    if($tmp=="") $tmp="&nbsp;";
    return $tmp;
  }

  /**
  * the actual displaying function
  * @return string Returns a renderd week matrix
  */
  function _dumpMatrix()
  {
    $buff   = $this->_view_header();
    $weekday = array();

    for ($i=1;$i<=7;$i++)
    {
      $text  = text($this->m_weekdates[$i]["day"])." (".substr($this->m_weekdates[$i]["date"],8,2)."-".substr($this->m_weekdates[$i]["date"],5,2).")";
      $href  = $_SERVER["PHP_SELF"]."?view=day&viewdate=".$this->m_weekdates[$i]["date"]."&dayview=".$this->m_showGlobal;
      $weekday[$i]["title"] = href($href,$text);

      //$this->getItems(substr($this->m_weekdates[$i]["date"],8,2),substr($this->m_weekdates[$i]["date"],5,2),substr($this->m_weekdates[$i]["date"],0,4));
      // filter on allday events / other appointment events
      //$this->filterItems(substr($this->m_weekdates[$i]["date"],8,2),substr($this->m_weekdates[$i]["date"],5,2),substr($this->m_weekdates[$i]["date"],0,4));
      $weekday[$i]["date"] = $this->m_weekdates[$i]["date"];
      $weekday[$i]["alldayitems"] = $this->m_allDayItems[$this->m_weekdates[$i]["date"]];
      $weekday[$i]["appointmentitems"] = $this->m_appointmentItems[$this->m_weekdates[$i]["date"]];

    }
    $buff.= '<table width="90%">
             <tr>
              <td valign="top">
              <table border="0" width="100%" cellpadding=2 cellspacing=1>
              <tr bgcolor="#DDDDDD">
                <td width="45%">
                  '.$weekday[1]['title'].'
                </td>
                <td width="45%">
                  '.$weekday[4]['title'].'
                </td>
              </tr>
              <tr bgcolor="white">
                <td>
                  '.$this->renderItems($weekday[1]).'
                </td>
                <td>
                 '.$this->renderItems($weekday[4]).'
                </td>
              </tr>

              <tr bgcolor="#dddddd">
                <td>
                  '.$weekday[2]['title'].'
                </td>
                <td>
                  '.$weekday[5]['title'].'
                </td>
              </tr>
              <tr bgcolor="#ffffff">
                <td>
                  '.$this->renderItems($weekday[2]).'
                </td>
                <td>
                  '.$this->renderItems($weekday[5]).'
                </td>
              </tr>
              <tr >
                <td bgcolor="#dddddd">
                  '.$weekday[3]['title'].'
                </td>
                <td bgcolor="#eeeeee">
                  '.$weekday[6]['title'].'
                </td>
              </tr>
              <tr bgcolor="#ffffff">
                <td rowspan=3 valign="top">
                  '.$this->renderItems($weekday[3]).'
                </td>
                <td>
                  '.$this->renderItems($weekday[6]).'
                </td>
              </tr>
              <tr>
                <td bgcolor="#eeeeee">
                  '.$weekday[7]['title'].'
                </td>
              </tr>
              <tr bgcolor="#ffffff">
                <td>
                  '.$this->renderItems($weekday[7]).'
                </td>
              </tr>


              </table>
              </td>
              <td width=200 valign="top">';
    $buff.=$this->_getSmallCalendar(substr($this->m_small_viewdate,8,2),substr($this->m_small_viewdate,5,2),substr($this->m_small_viewdate,0,4));
    $buff.="<br>";
    $next_date = date("Y-m-d",mktime(12,0,0,substr($this->m_small_viewdate,5,2)+1,substr($this->m_small_viewdate,8,2),substr($this->m_small_viewdate,0,4)));
    $buff.=$this->_getSmallCalendar(substr($next_date,8,2),substr($next_date,5,2),substr($next_date,0,4),false);


    $buff.='  </td>
             </tr>
             </table>
             <br><br>
             '.$this->m_legend;


    return $buff;
  }

 /**
  * Render the view
  * @return string A renderd view
  */
  function renderView()
  {
    $this->_setUserSettings();
    if ($this->m_viewdate!="")
    {
      $viewtime = mktime(12,12,0,$this->m_month,$this->m_day,$this->m_year);
    }
    else
    {
      $this->m_viewdate = date("Y-m-d");
      $viewtime = time();
    }
    $weekday  = strftime("%w",$viewtime);

    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
      $this->m_weekdates[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $this->m_weekdates[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
      $this->m_weekdates[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
    }
    $this->getItems($this->m_weekdates[1]['date'],$this->m_weekdates[7]['date']);
    //$this->m_allItems = $this->getItems();

    // filter on allday events / other appointment events
    //$this->filterItems();

    // get scheduler Legend and Item colors
    $this->getLegend();

    return $this->_dumpMatrix();
  }

}

?>