<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage scheduler
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

atkimport("modules.scheduler.views.scheduler_view");

/**
 * scheduler week view
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_weekview extends scheduler_view
{
  var $m_weekdates = array();
  var $m_weeknumber = "";
  var $m_view = "week";


 /**
  * navigate through weeks using prev/next week link
  * @returns html code containing links for previous week, current week and next week
  */
  function _getNav()
  {

    $theme = &atktheme::getInstance();
    $prevTime = $this->m_weekdates[0]['date'];
    $prevUrl  = atkSelf()."?view=week&viewdate=$prevTime";
    $prevImg  ='<img src="'.$theme->imgPath("arrow_left.gif","scheduler").'" border=0 width=16 height=16 title="'.atktext("previousweek","scheduler").'" alt="'.atktext("previousweek","scheduler").'">';

    $nextTime = $this->m_weekdates[8]['date'];
    $nextUrl  = atkSelf()."?view=week&viewdate=$nextTime";
    $nextImg  ='<img src="'.$theme->imgPath("arrow_right.gif","scheduler").'" border=0 width=16 height=16 title="'.atktext("nextweek","scheduler").'" alt="'.atktext("nextweek","scheduler").'">';

    $currentUrl = atkSelf()."?view=week&viewdate=".date("Y-m-d");

		atkimport('modules.utils.dateutil');
    $weeknumberFull = $this->m_model->getWeekNumber($this->m_model->getDay(),$this->m_model->getMonth(),$this->m_model->getYear(),true);
    $this->m_weeknumber = $this->m_model->getWeekNumber($this->m_model->getDay(),$this->m_model->getMonth(),$this->m_model->getYear());
    $this->m_viewTitle = "<b>";
    if(dateutil::isISO8601()) $this->m_viewTitle.= atktext("week")."&nbsp;".$this->m_weeknumber.", ";
		$this->m_viewTitle.= dateutil::stamp2str($this->m_weekdates[1]['stamp'],atktext('date_format_view')). ' - '.
		                     dateutil::stamp2str($this->m_weekdates[7]['stamp'],atktext('date_format_view')).'</b>';
		
    return href($prevUrl, $prevImg)."&nbsp;".href($nextUrl, $nextImg)."&nbsp;".href($currentUrl,atktext("currentweek","scheduler"));

  }
  /**
  * Renders a calendar weekdays
  * @param array $weekday Calendar items for a weekday
  * @return string HTML code with a renderd calendar weekday
  */
  function renderItems($weekday)
  {
    $tmp= "";
    $schedulerItemObj = null;
    for($i=0;$i<count($weekday["alldayitems"]);$i++)
    {
      if(is_null($schedulerItemObj)) $schedulerItemObj = &atkinstance("module.scheduler.utils.scheduleritem");
      $tmp.=$schedulerItemObj->render($weekday["alldayitems"][$i],true,$weekday["date"],true);
    }

    for($i=0;$i<count($weekday["appointmentitems"]);$i++)
    {
       if(is_null($schedulerItemObj)) $schedulerItemObj = &atkinstance("module.scheduler.utils.scheduleritem");
       $tmp.=$schedulerItemObj->render($weekday["appointmentitems"][$i],true,$weekday["date"],true);
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
    $alldayItems = $this->m_model->getAlldayItems();
    $appointmentItems = $this->m_model->getAppointmentItems();

    for ($i=1;$i<=7;$i++)
    {
      $text  = atktext($this->m_weekdates[$i]["day"])." (".substr($this->m_weekdates[$i]["date"],8,2)."-".substr($this->m_weekdates[$i]["date"],5,2).")";
      $href  = atkSelf()."?view=day&viewdate=".$this->m_weekdates[$i]["date"];
      $weekday[$i]["title"] = href($href,$text);

      //$this->getItems(substr($this->m_weekdates[$i]["date"],8,2),substr($this->m_weekdates[$i]["date"],5,2),substr($this->m_weekdates[$i]["date"],0,4));
      // filter on allday events / other appointment events
      //$this->filterItems(substr($this->m_weekdates[$i]["date"],8,2),substr($this->m_weekdates[$i]["date"],5,2),substr($this->m_weekdates[$i]["date"],0,4));
      $weekday[$i]["date"] = $this->m_weekdates[$i]["date"];
      $weekday[$i]["alldayitems"] = (array_key_exists($this->m_weekdates[$i]["date"],$alldayItems)?$alldayItems[$this->m_weekdates[$i]["date"]]:array());
      $weekday[$i]["appointmentitems"] = (array_key_exists($this->m_weekdates[$i]["date"],$appointmentItems)?$appointmentItems[$this->m_weekdates[$i]["date"]]:array());

    }
    $buff.= '<table width="100%">
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
                <td valign="top">
                  '.$this->renderItems($weekday[1]).'
                </td>
                <td valign="top">
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
                <td valign="top">
                  '.$this->renderItems($weekday[2]).'
                </td>
                <td valign="top">
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
                <td valign="top">
                  '.$this->renderItems($weekday[6]).'
                </td>
              </tr>
              <tr>
                <td bgcolor="#eeeeee">
                  '.$weekday[7]['title'].'
                </td>
              </tr>
              <tr bgcolor="#ffffff">
                <td valign="top">
                  '.$this->renderItems($weekday[7]).'
                </td>
              </tr>


              </table>
              <br>
              '.$this->m_model->renderLegend().'
              </td>
              <td width=150 valign="top">';
    $buff.=$this->m_model->_getSmallCalendar(substr($this->m_model->m_small_viewdate,8,2),substr($this->m_model->m_small_viewdate,5,2),substr($this->m_model->m_small_viewdate,0,4));
    $buff.="<br>";
    $next_date = date("Y-m-d",mktime(12,0,0,substr($this->m_model->m_small_viewdate,5,2)+1,substr($this->m_model->m_small_viewdate,8,2),substr($this->m_model->m_small_viewdate,0,4)));
    $buff.=$this->m_model->_getSmallCalendar(substr($next_date,8,2),substr($next_date,5,2),substr($next_date,0,4),false);
    $buff.="<br>";
    $buff.=$this->m_model->_getEmployeesCheckboxesForm();

    $buff.='  </td>
             </tr>
             </table>
             <br><br>
             ';


    return $buff;
  }

 /**
  * Render the view
  * @return string A renderd view
  */
  function _render()
  {
    if (empty($this->m_model->m_viewdate))
    {
      $this->m_model->m_viewdate = date("Y-m-d");
    }
		atkimport("module.timereg.timeregutils");
    $this->m_weekdates = TimeregUtils::getWeekDates($this->m_model->m_viewdate,false);		
    $this->m_model->getItems($this->m_weekdates[1]['date'],$this->m_weekdates[7]['date']);

    // get scheduler Legend and Item colors
    $this->m_model->getLegend();

    return $this->_dumpMatrix();
  }

}

?>