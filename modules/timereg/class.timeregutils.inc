<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage timereg
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */


  /**
   * Class with timereg utility functions
   *
   * @package achievo
   * @subpackage timereg
   */
class TimeregUtils {

  /**
   * Translate viewdate to unix timestamp, return current date's timestamp if
   * $viewdate is empty.
   *
   * @param string $viewdate  date string of yyyy-mm-dd format
   * @return int              unix timestamp
   */
  function getViewTime($viewdate)
  {
    if ($viewdate !="")
    {
      $viewtime = mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
    }
    else
    {
      $viewtime = time();
    }

    return $viewtime;
  }

  /**
   * Get all days for the week which $viewdate belongs to, plus one day
   * before and after. These extra days are used for easy access to
   * the next and previous week.
   *
   * @param string $viewdate date of yyyy-mm-dd format
   * @return array
   */
  function getWeekDates($viewdate) 
  {
    atkdebug("Viewdate: ".$viewdate);
    $viewtime = TimeregUtils::getViewTime($viewdate);
    atkdebug("Viewdate(2): ".date("Y-m-d",$viewtime));
    atkimport("modules.scheduler.utils.weeknumber");
    $weeknumber = weeknumber::getWeeknumber($viewtime,atkconfig("startday",1),atkconfig("firstweekcontains",4),true);

    $weekdates = array();
    $day = 86400;
    $weekstartday = $weeknumber["firstweekstart"];
    atkdebug("Viewdate(3): ".date("Y-m-d",$weekstartday));
    for ($i=0; $i<=8;$i++)
    {
      $multiplier = ($i-1);
      $day_timestamp = $weekstartday+($day*$multiplier);
      atkdebug("Weekdates (".($i)."): ".date("Y-m-d" ,$day_timestamp));
      $weekdates[$i]['date']    = date("Y-m-d" ,$day_timestamp);
      $weekdates[$i]['day']     = atktext(strtolower(date("D" ,$day_timestamp)));
      $weekdates[$i]['dateday'] = strtolower(date("d" ,$day_timestamp));
      $weekdates[$i]['month']   = atktext(strtolower(date("F" ,$day_timestamp)));
      $weekdates[$i]['stamp']   = $day_timestamp;
      $weekdates[$i]['lock']    = Locking::getLockType(atkArrayNvl(getUser(), "id"),$weekdates[$i]['date']);
    }

    return $weekdates;
  }

}

?>
