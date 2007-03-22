<?php

/**
 *
 * Calculate the week number for a know date
 * 
 * Monday is the first day of week.
 * Week containing Jan 7th is the first week of that year.
 *
 * Monday is the first day of week. (ISO)
 * Week containing Jan 4th is the first week of that year.
 *
 * Monday is the first day of week.
 * Week containing Jan 1st is the first week of that year.
 *
 * Wednesday is the first day of week.
 * Week containing Jan 1st is the first week of that year.
 *
 * Saturday is the first day of week.
 * Week containing Jan 1st is the first week of that year.
 *
 * Sunday is the first day of week.
 * Week containing Jan 1st is the first week of that year.
 */

class weeknumber
{
  /**
   * Get the weeknumber
   *
   * @param int $timestamp Timestamp of the given date
   * @param int $startday Startday of the week (0 = sunday)
   * @param int $firstweekcontains Can be 1,4,7 januari
   * @param bool $full Returns an array if full with the start and end date of the week
   * @return int|array Weeknumber or array with weeknumber,startdate,enddate
   */
  function getWeeknumber($timestamp,$startday,$firstweekcontains,$full=false)
  {
    //atkdebug("Search: ".date("Y-m-d",$timestamp)." - $startday - $firstweekcontains");
    $enddate = weeknumber::getLastDayFromFirstWeek($timestamp,$startday,$firstweekcontains);
    $enddate_nextyear = weeknumber::getLastDayFromFirstWeek(strtotime("+1 year",$timestamp),$startday,$firstweekcontains);
    $weeknumber = weeknumber::searchWeekNumber($timestamp,$enddate,$enddate_nextyear);

    if($full)
    {
      return $weeknumber;
    }
    //atkdebug("Weeknumber: ".$weeknumber["weeknumber"]);
    return $weeknumber["weeknumber"];

  }
  /**
   * Calculate the lastday of the first week
   *
   * @param int $timestamp Timestamp of the given date
   * @param int $startday Startday of the week (0 = sunday)
   * @param int $firstweekcontains Can be 1,4,7 januari
   * @return int Timestamp of the lastday of the week
   */
  function getLastDayFromFirstWeek($timestamp,$startday,$firstweekcontains)
  {
    $day = 86400;
    $year = date("Y",$timestamp);
    $tmp_time = mktime(12,0,0,1,$firstweekcontains,$year);
    $weekday = date("w",$tmp_time);
    //atkdebug("startDate: ".date("d-m-Y",$tmp_time));
    //atkdebug("Weekday: $weekday - Startday: $startday");
    if($weekday==$startday)
    {
      return $tmp_time+(7*$day);
    }
    else
    {
      $wk_array = array();
      $daynumber = $startday;
      for($i=0;$i<7;$i++)
      {
        if($daynumber==7) $daynumber=0;
        $wk_array[$daynumber]=$i;
        $daynumber++;
      }
      //atk_var_dump($wk_array,"WEEK ARRAY, $weekday - $startday");
      //atkdebug("Add ".(6-$wk_array[$weekday])." days");
      //atkdebug("$tmp_time + ".((6-$wk_array[$weekday])*$day)." = ".($tmp_time+((6-$wk_array[$weekday])*$day)));
      //atkdebug("EndDate: ".date("d-m-Y",($tmp_time+((6-$wk_array[$weekday])*$day))));
      return $tmp_time+((6-$wk_array[$weekday])*$day);
    }
  }
  /**
   * Search weeknumber
   *
   * @param int $timestamp Given timestamp
   * @param int $enddate Timestamp of the first week lastday
   * @param int $enddate_nextyear Timestamp of the first week lastday next year
   * @return array Array with weeknumber, startday, endday
   */
  function searchWeekNumber($timestamp,$enddate,$enddate_nextyear)
  {
    //atkdebug("Firstweek enddate:".date("Y-m-d",$enddate));
    //atkdebug("Firstweek enddate nextyear:".date("Y-m-d",$enddate_nextyear));
    $startdate_nextyear = 
    $weeknumber=0;
    $day = 86400;
    $week = 7*$day;
    $startdate_nextyear = ($enddate_nextyear-(6*$day));
    if($timestamp>=$startdate_nextyear && $timestamp<=$enddate_nextyear)
    {
      //atkdebug("Search in (1) ".date("Y-m-d",$startdate_nextyear)." - ".date("Y-m-d",$enddate_nextyear));
     
      return array("weeknumber"=>1,
                  "firstweekstart"=>$startdate_nextyear,
                  "firstweekend"=>$enddate_nextyear);
    }

    $startyear = date("Y",$timestamp);
    $found=false;
    for($i=$enddate;date("Y",$i)==$startyear&&!$found;$i+=$week)
    {
      $weeknumber++;
      $weekstart = ($i-(6*$day));
      $weekend = $i;
      //atkdebug("Search in ($weeknumber) ".date("Y-m-d",$weekstart)." - ".date("Y-m-d",$weekend));
      if($timestamp>=$weekstart && $timestamp<=$weekend)
      {
        //atkdebug("Search in ($weeknumber) ".date("Y-m-d",$weekstart)." - ".date("Y-m-d",$weekend));
        $found=true;
      }
    }
    $tmp_array = array("weeknumber"=>$weeknumber,
                       "firstweekstart"=>$weekstart,
                       "firstweekend"=>$weekend);
    if(!$found) $tmp_array=array("weeknumber"=>1,"firstweekstart"=>NULL,"firstweekend"=>NULL);
    return $tmp_array;
  }
}

?>