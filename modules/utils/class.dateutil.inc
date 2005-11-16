<?php

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

  }

?>
