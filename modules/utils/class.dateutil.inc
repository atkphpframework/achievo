<?php

  define("ONEDAY", 24*60*60);
  define("ONEWEEK", ONEDAY*7);


  class dateutil
  {
    /**
     * Return the difference (in number of days) between two dates.
     * If $start is beyond $end, the difference will be negative.
     *
     * @param datestring $start Should be in the format 20040201
     * @param datestring $end   Should be in the format 20040201
     */
    function date_diff($start, $end)
    {
      if ($start==""||$end=="") return 0;

      $d1 = adodb_mktime(12,0,0,substr($start,4,2), substr($start,6,2), substr($start,0,4));
      $d2 = adodb_mktime(12,0,0,substr($end,4,2), substr($end,6,2), substr($end,0,4));
      return floor(($d2-$d1)/(60*60*24));
    }

    function arr2str($arrdate)
    {
      if (!is_array($arrdate)) return "";
      return sprintf("%04d%02d%02d", $arrdate["year"], $arrdate["month"], $arrdate["day"]);
    }

    function str2arr($strdate)
    {
      return array("year"=>substr($strdate,0,4), "month"=>substr($strdate,4,2), "day"=>substr($strdate,6,2));
    }

    function str2stamp($strdate)
    {
      return dateutil::arr2stamp(dateutil::str2arr($strdate));
    }

    function arr2stamp($arrdate)
    {
      return adodb_mktime(12,0,0,$arrdate["month"],$arrdate["day"],$arrdate["year"]);
    }

    function stamp2str($timestamp)
    {
      return date("Ymd", $timestamp);
    }

    function stamp2arr($timestamp)
    {
      return dateutil::str2arr(dateutil::stamp2str($timestamp));
    }

    function monthname($monthnumber)
    {
      $months = array("january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december");
      return $months[$monthnumber-1];
    }

    function short_monthname($monthnumber)
    {
      $months = array("jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec");
      return $months[$monthnumber-1];
    }

    /*
     * Given a date in the form "20010325", this function returns the date of the first
     * day (monday) of that week.
     */
    function startOfWeek($strdate)
    {
      $viewtime = adodb_mktime(0,0,0,substr($strdate,4,2),substr($strdate,6,2),substr($strdate,0,4));
      $weekday = strftime("%w",$viewtime);
      if ($weekday==0) $weekday=7;
      return date("Ymd" ,$viewtime-(86400*($weekday-1)));
    }

    /*
     * Given a date in the form "20010325", this function returns the date of the last
     * day (sunday) of that week.
     */
    function endOfWeek($strdate)
    {
      $viewtime = adodb_mktime(0,0,0,substr($strdate,4,2),substr($strdate,6,2),substr($strdate,0,4));
      $weekday = strftime("%w",$viewtime);
      if ($weekday==0) $weekday=7;
      return date("Ymd" ,$viewtime-(86400*($weekday-7)));
    }
    
    /**
     * Calculates the weeknumber, startdate/enddate of the week for each week 
     * within the passed date range.
     *
     * @return array Array containing arrays like "week"=>weeknumber,"startdate"=>startdate,
     * "enddate"=>enddate
     */
    function weeksBetween($strstart, $strend)
    {      
      // Initialize the weekdata array
      $weekdata = array();

      // First fill the weeks and start/end dates..
      $startstamp = dateUtil::str2stamp($strstart);
      $endstamp = dateUtil::str2stamp($strend);

      // Calculate the number of dates between the selected start and end date
      $nrofdays = dateUtil::date_diff($strstart, $strend);

      // Loop through the days, starting at the day selected as "from"
      for ($i = 0; $i <= $nrofdays; $i++)
      {
        // Calculate the current day depending on the startdate and iterator
        $curstamp = $startstamp + (ONEDAY * $i);

        // If the day is a monday, then register the weekid, startdate and
        // enddate of the week in the weekdata array and jump one week ahead
        if (strftime("%w",$curstamp)==1)
        {
          $key = weekstamp($curstamp);
          $weekdata[$key]["week"] = $key;
          $weekdata[$key]["startdate"] = date("Ymd",$curstamp);
          $weekdata[$key]["enddate"] = date("Ymd",$curstamp+(6*ONEDAY));
          $i+=6;
        }
      }

      // Return the array containing the weekid, start- and enddate for each week within the given from and to dates     
      return $weekdata;    
    }

  }

?>
