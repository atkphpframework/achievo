<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage utils
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
  define("ONEDAY", 24*60*60);
  define("ONEWEEK", ONEDAY*7);
  require_once(atkConfig("atkroot")."atk/utils/adodb-time.inc.php");


/**
 * Static class with date functions
 *
 * @package achievo
 * @subpackage utils
 */
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

    /**
     * Convert date array to string
     *
     * @param array $arrdate Date array
     * @param string $format Date format
     * @return string
     */
    function arr2str($arrdate, $format="Ymd")
    {
      if (!is_array($arrdate)) return "";
      return dateutil::stamp2str(dateutil::arr2stamp($arrdate), $format);
    }

    /**
     * Date string to an array with the elements: year, month, day
     *
     * @param string $strdate Date string in the format yyyymmdd or yyyy-mm-dd
     * @return array
     */
    function str2arr($strdate)
    {
      // parse dates in the common formats yyyymmdd and yyyy-mm-dd
      if ( strlen($strdate) == 8 ) 
			{
        return array("year"=>substr($strdate,0,4), "month"=>substr($strdate,4,2), "day"=>substr($strdate,6,2));
      } 
			elseif ( strlen($strdate) == 10 ) 
			{
        return array("year"=>substr($strdate,0,4), "month"=>substr($strdate,5,2), "day"=>substr($strdate,8,2));
      } 
			else 
			{
        return array(); 
			}
    }
		
    /**
     * Date string to formatted string
     *
     * @param string $strdate Date string
     * @return formatted string
     */
    function str2str($strdate, $format="Ymd")
    {
      return dateutil::arr2str(dateutil::str2arr($strdate), $format);
     } 

    /**
     * Date string to timestamp
     *
     * @param string $strdate Date string in the format yyyymmdd
     * @return int
     */
    function str2stamp($strdate)
    {
      return dateutil::arr2stamp(dateutil::str2arr($strdate));
    }

    /**
     * Date array to timestamp
     *
     * @param array $arrdate Date array with elements: year, month, day
     * @return int
     */
    function arr2stamp($arrdate)
    {
      return adodb_mktime(12,0,0,$arrdate["month"],$arrdate["day"],$arrdate["year"]);
    }

    /**
     * Timestamp to date string
     *
     * @param int $timestamp Timestamp
     * @param string $format Date format
     * @return string
     */
    function stamp2str($timestamp, $format="Ymd")
    {
      return date($format, $timestamp);
    }

    /**
     * Timestamp to array with elements: day, month, year
     *
     * @param int $timestamp Timestamp
     * @return array
     */
    function stamp2arr($timestamp)
    {
      return dateutil::str2arr(dateutil::stamp2str($timestamp));
    }

    /**
     * Return array with all month names
     *
     * @return array
     */
    function monthlist()
    {
      return array(1=>"january", 2=>"february", 3=>"march", 4=>"april", 5=>"may", 6=>"june", 7=>"july", 8=>"august", 9=>"september", 10=>"october", 11=>"november", 12=>"december");
    }

    /**
     * Give month name for a given number
     *
     * @param int $monthnumber
     * @return string
     */
    function monthname($monthnumber)
    {
      $months = dateutil::monthlist();
      return $months[$monthnumber];
    }

    /**
     * Return short month list
     *
     * @return array
     */
    function short_monthlist()
    {
      return array(1=>"jan", 2=>"feb", 3=>"mar", 4=>"apr", 5=>"may", 6=>"jun", 7=>"jul", 8=>"aug", 9=>"sep", 10=>"oct", 11=>"nov", 12=>"dec");
    }

    /**
     * Return short daylist (3 letter days)
     *
     * @return array
     */
    function short_daylist()
    {
      return array(0=>"sun",1=>"mon",2=>"tue",3=>"wed",4=>"thu",5=>"fri",6=>"sat");
    }

    /**
     * Return short month name for a given number
     *
     * @param int $monthnumber
     * @return string
     */
    function short_monthname($monthnumber)
    {
      $months = dateutil::short_monthlist();
      return $months[$monthnumber];
    }

    /**
     * Given a date in the form "20010325", this function returns the date of the first
     * day (monday) of that week.
     *
     * @param string $strdate Date string
     * @return string
     */
    function startOfWeek($strdate)
    {
      $viewtime = adodb_mktime(0,0,0,substr($strdate,4,2),substr($strdate,6,2),substr($strdate,0,4));
      $weekday = strftime("%w",$viewtime);
      if ($weekday==0) $weekday=7;
      return date("Ymd" ,$viewtime-(86400*($weekday-1)));
    }

    /**
     * Given a date in the form "20010325", this function returns the date of the last
     * day (sunday) of that week.
     *
     * @param string $strdate Date string
     * @return string
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
     * @param string $strstart Start date
     * @param string $strend End date
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

    /**
     * Calculates the the date for each of the days between start- and enddate.
     * @param string $strstart Startdate in the format yyyymmdd
     * @param string $strend Enddate in the format yyyymmdd
     * @return array Array containing arrays like "date"=>date
     */
    function daysBetween($strstart, $strend)
    {
      // Initialize the weekdata array
      $daydata = array();

      // First fill the weeks and start/end dates..
      $startstamp = dateUtil::str2stamp($strstart);
      $endstamp   = dateUtil::str2stamp($strend);

      // Calculate the number of days between the selected start and end date
      $nrofdays = dateUtil::date_diff($strstart, $strend);

      // Loop through the days, starting at the day selected as "from"
      for ($i = 0; $i <= $nrofdays; $i++)
      {
        // Calculate the current day depending on the startdate and iterator
        $curstamp = $startstamp + (ONEDAY * $i);

        $key = date("Ymd",$curstamp);
        $daydata[$key]["date"] = $key;
      }

      // Return the array containing the dayid, and date for each day within the given from and to dates
      return $daydata;
    }

    /**
     * LastdayOfMonth returns the timestamp of the date
     * of the last $day of $month
     *
     * @example lastDayOfMonth(0,3,2007) returns
     * the date of the last Sunday of March 2007
     *
     * @param int $day Weekday (must be the day number, 0 = Sunday)
     * @param int $month Month
     * @param int $year Year
     * @return int Timestap of the date
     */
    function lastDayOfMonth($day, $month, $year)
    {
      // Get the timestamp for the 1. day of the month
      $firstOfMonth = mktime(0,0,0,$month,1,$year);
      // Get the timestamp for the last day of the month
      $lastOfMonth = ($firstOfMonth - 86400) + date("t", $firstOfMonth) * 86400;
      // Get the day of the week for the last day in the month
      $lastOfMonth_DayOfWeek = date("w", $lastOfMonth);

      // Is the last of the month on the same day of day of week?
      if ($lastOfMonth_DayOfWeek == $day) {
      // If so, return the timestamp
      return $lastOfMonth;
      } else {
      // Iterate from the last of the month until we have a match
      for ($tempDate = $lastOfMonth; date("w", $tempDate) <> $day; $tempDate = $tempDate - 86400) {
      }
      return $tempDate;
      }
    }
		
		/**
		 * Check if the given year is a leap year
		 * @return boolean
		 * @param object $year[optional]
		 */
		function isLeapYear ( $year = '' ) 
		{
      if ( empty ( $year ) )
      $year = strftime ( '%Y', time () );

      if ( strlen ( $year ) != 4 || preg_match ( '/\D/', $year ) )
        return false;

       return ( ( $year % 4 == 0 && $year % 100 != 0 ) || $year % 400 == 0 );
    }
		
		/**
		 * Check if we have an ISO8601 week
		 * @return boolean
		 */
		function isISO8601()
		{
			return (atkconfig("startday",1)==1 &&
		          atkconfig("firstweekcontains",4)==4);
		}
		
	/**
	 * Calculate the weeknumber for an ISO8601 date
	 * based on pear/Date/Calc.php 
	 * @return int
	 * @param int $day
	 * @param int $month
	 * @param int $year
	 */
  function getWeeknumber($day,$month,$year)
	{
		 $mnth = array (0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334);
        $y_isleap = dateutil::isLeapYear($year);
        $y_1_isleap = dateutil::isLeapYear($year - 1);
        $day_of_year_number = $day + $mnth[$month - 1];
        if ($y_isleap && $month > 2) {
            $day_of_year_number++;
        }
        // find Jan 1 weekday (monday = 1, sunday = 7)
        $yy = ($year - 1) % 100;
        $c = ($year - 1) - $yy;
        $g = $yy + intval($yy / 4);
        $jan1_weekday = 1 + intval((((($c / 100) % 4) * 5) + $g) % 7);
        // weekday for year-month-day
        $h = $day_of_year_number + ($jan1_weekday - 1);
        $weekday = 1 + intval(($h - 1) % 7);
        // find if Y M D falls in YearNumber Y-1, WeekNumber 52 or
        if ($day_of_year_number <= (8 - $jan1_weekday) && $jan1_weekday > 4){
            $yearnumber = $year - 1;
            if ($jan1_weekday == 5 || ($jan1_weekday == 6 && $y_1_isleap)) {
                $weeknumber = 53;
            } else {
                $weeknumber = 52;
            }   
        } else {
        	     $yearnumber = $year;
        }
        // find if Y M D falls in YearNumber Y+1, WeekNumber 1
        if ($yearnumber == $year) {
            if ($y_isleap) {
                $i = 366;   
            } else {
                $i = 365;
            }
            if (($i - $day_of_year_number) < (4 - $weekday)) {
                $yearnumber++;
                $weeknumber = 1;
            }
        }
        // find if Y M D falls in YearNumber Y, WeekNumber 1 through 53
        if ($yearnumber == $year) {
            $j = $day_of_year_number + (7 - $weekday) + ($jan1_weekday - 1);
            $weeknumber = intval($j / 7);
            if ($jan1_weekday > 4) {
                $weeknumber--;
            }
        }
        // put it all together
        if ($weeknumber < 10) {
            $weeknumber = '0'.$weeknumber;
        }
        return $weeknumber;		
	  }
		

  }

?>
