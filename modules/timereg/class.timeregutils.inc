<?php

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
  function getWeekDates($viewdate) {

    $viewtime = TimeregUtils::getViewTime($viewdate); 
    $weekdates = array();
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
      $weekdates[$i]['date']    = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $weekdates[$i]['day']     = atktext(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
      $weekdates[$i]['dateday'] = strtolower(date("d" ,$viewtime-(86400*($weekday-$i))));
      $weekdates[$i]['month']   = atktext(strtolower(date("F" ,$viewtime-(86400*($weekday-$i)))));
      $weekdates[$i]['stamp']   = $viewtime-(86400*($weekday-$i));
      $weekdates[$i]['lock']    = Locking::getLockType(atkArrayNvl(getUser(), "id"),$weekdates[$i]['date']);
    }

    return $weekdates;  
  }

}

?>
