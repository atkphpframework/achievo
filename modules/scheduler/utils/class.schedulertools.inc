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

/**
 * Library of functions used for the scheduler
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class schedulertools
{

    /**
     * Convert weekday number to flag
     *
     * @param int $weekday_number Weekday number (0 sunday, 6 saturday)
     * @return int Weekday flag
     */
    function weekdaynumbertoflag($weekday_number)
    {
        if (($weekday_number >= 0) && ($weekday_number <= 6))
            $weekday = pow(2, $weekday_number); //2^$weekday_number;
        else
            $weekday = false;

        return $weekday;
    }

    /**
     * Determine whether or not the passed year is a leap year.
     *
     * @param   int     $y      The year as a four digit integer
     * @return  boolean         True if the year is a leap year, false otherwise
     */
    function isLeapYear($y)
    {
        return $y % 4 == 0 && ($y % 400 == 0 || $y % 100 != 0);
    }

    /**
     * Calculate day of week (1=sunday, 2=monday etc...)
     *
     * @param int $day Day
     * @param int $month Month
     * @param int $year Year
     * @return int Weekday
     */
    function dayOfWeek($day, $month, $year)
    {
        return date("w", mktime(12, 0, 0, $month, $day, $year)) + 1;
    }

    /**
     * Calculate the nth day
     *
     * @param int $nth 1,2,3,4,5,-1 (last)
     * @param int $weekday Day number (1=sunday, 2=monday etc...)
     * @param int $month Month
     * @param int $year Year
     * @return int Day number
     */
    function nthDay($nth, $weekday, $month, $year)
    {
        $daysOfMonthY = array(31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31); // Leapyear numbers
        $daysOfMonth = array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31); // Non leapyear numbers
        if ($nth > 0)
            return ($nth - 1) * 7 + 1 + (7 + $weekday - schedulertools::dayOfWeek(($nth - 1) * 7 + 1, $month, $year)) % 7;
        if (schedulertools::isLeapYear($year))
            $days = $daysOfMonthY[$month - 1];
        else
            $days = $daysOfMonth[$month - 1];
        return $days - (schedulertools::dayOfWeek($days, $month, $year) - $weekday + 7) % 7;
    }

    /**
     * Returns the startdate of the item
     *
     * @param Array $item Scheduler item
     * @return int Timestamp
     */
    function getItemStartDate($item)
    {
        return mktime(12, 0, 0, intval($item["startdate"]["month"]), intval($item["startdate"]["day"]), intval($item["startdate"]["year"]));
    }

    /**
     * Returns the enddate of the item
     *
     * @param array $item Scheduler item
     * @return int Timestamp or NULL
     */
    function getItemEndDate($item)
    {
        if ($item['recur'] == 'once') {
            $enddate = mktime(12, 0, 0, intval($item["enddate"]["month"]), intval($item["enddate"]["day"]), intval($item["enddate"]["year"]));
        } else {
            switch ($item['cyclus']['end_choice']) {
                case 1: $enddate = mktime(12, 0, 0, 12, 31, 2037); // No enddate, so we use max date
                    break;
                case 2: $enddate = mktime(12, 0, 0, $item["cyclus"]["cyclus_enddate"]["month"], $item["cyclus"]["cyclus_enddate"]["day"], $item["cyclus"]["cyclus_enddate"]["year"]);
                    break;
                case 3: $enddate = NULL;  // After x times
                    break;
            }
        }
        return $enddate;
    }

    /**
     * Get the dates for a recurring item for the given period
     *
     * @param array $item
     * @param string $rangeStartDate
     * @param string $rangeEndDate
     * @return array Array with dates for the given item
     */
    function getDates($item, $rangeStartDate = NULL, $rangeEndDate = NULL)
    {
        $dates = array();
        $datecounter = 0;
        $validdates = 0;
        $startdate = schedulertools::getItemStartDate($item);
        $enddate = schedulertools::getItemEndDate($item);

        if ($rangeStartDate != NULL && $rangeEndDate != NULL) {
            if (!is_null($enddate) && $rangeStartDate > date('Y-m-d', $enddate))
                return array();
            if ($rangeStartDate > date('Y-m-d', $startdate) && $rangeStartDate <= $enddate)
                $startdate = mktime(12, 0, 0, substr($rangeStartDate, 5, 2), substr($rangeStartDate, 8, 2), substr($rangeStartDate, 0, 4));
            if (($item['cyclus']['end_choice'] == 2 && $rangeEndDate < date('Y-m-d', $enddate) && $rangeEndDate > $startdate) || $item['cyclus']['end_choice'] != 2)
                $enddate = mktime(12, 0, 0, substr($rangeEndDate, 5, 2), substr($rangeEndDate, 8, 2), substr($rangeEndDate, 0, 4));
        }

        if ($item["recur"] == "once")
            $maxtimes = 1;
        elseif ($item["cyclus"]["end_choice"] == 3)
            $maxtimes = $item["cyclus"]["cyclus_times"] - intval($item["times"]);
        else
            $maxtimes = false;

        atkdebug("MAXTIMES: $maxtimes");
        while (($maxtimes === false) || ($validdates < $maxtimes)) {
            $checkdate = $startdate + ($datecounter * 86400);
            if ($checkdate > $enddate)
                break;
            // Skip all months that we don't need for the yearly day/date events
            if ($item['recur'] == "yearly" && date("n", $checkdate) != $item["cyclus"]["yearly_month"] && date("n", $checkdate) != $item["cyclus"]["yearly_month2"]
            ) {
                $current_day = (date("d", $checkdate) - 1);
                $number_of_days = date("t", $checkdate);
                $datecounter+=($number_of_days - $current_day);
                continue;
            }

            if (schedulertools::showItem($item, $checkdate, $startdate, $enddate)) {
                $dates[] = date("Y-m-d", $checkdate);
                $validdates++;
            }
            $datecounter++;
        }

        return $dates;
    }

    /**
     * Show the current item
     *
     * @param array $item Scheduler item
     * @param int $checkdate Day timestamp
     * @param int $startdate Startdate timestamp
     * @param int $enddate Enddate timestamp
     * @return boolean Show the item or not
     */
    function showItem($item, $checkdate, $startdate, $enddate)
    {
        $day = date("j", $checkdate);
        $month = date("n", $checkdate);
        $year = date("Y", $checkdate);
        $weekday_number = date("w", mktime(12, 0, 0, $month, $day, $year));
        $weekday = schedulertools::weekdaynumbertoflag($weekday_number);
        $show_item = true;
        switch ($item["recur"]) {
            case 'once':
                if ($checkdate >= $startdate && $checkdate <= $enddate) {
                    $show_item = true;
                } else {
                    $show_item = false;
                }
                break;
            case 'daily':
                if ($item['cyclus']['daily_choice'] == 1) {
                    $every = $item['cyclus']['daily_every'];
                    $interval = $checkdate - $startdate;
                    $interval_days = floor($interval / (60 * 60 * 24));
                    $devided = $interval_days / $every;
                    $match = (int) $devided;
                    if ($devided != $match) {
                        $show_item = false;
                    }
                } else {
                    $workdays = array(2, 3, 4, 5, 6);
                    if (in_array(schedulertools::dayOfWeek($day, $month, $year), $workdays)) {
                        $show_item = true;
                    } else {
                        $show_item = false;
                    }
                }
                break;
            case 'weekly':
                if (hasFlag(array_sum($item["cyclus"]["weekly_weekday"]), $weekday)) {
                    $show_item = true;
                } else {
                    $show_item = false;
                }
                $every = $item['cyclus']['weekly_every'];
                $interval = $checkdate - $startdate;
                $interval_weeks = floor($interval / (60 * 60 * 24 * 7));
                $devided = $interval_weeks / $every;
                $match = (int) $devided;
                if ($devided != $match) {
                    $show_item = false;
                }
                break;
            case 'monthly':
                if ($item['cyclus']['monthly_choice'] == 1) {
                    // Day x of every x months
                    if ($item["cyclus"]["monthly_day"] == $day) {
                        $show_item = true;
                    } else {
                        $show_item = false;
                    }
                    $every = $item['cyclus']['monthly_every'];
                } else {
                    // xth weekday of x months
                    $every = $item['cyclus']['monthly_every2'];
                    $xth_day = schedulertools::nthDay($item["cyclus"]["monthly_month_time"], $item["cyclus"]["monthly_weekday_list"], $month, $year);
                    if ($xth_day == $day) {
                        $show_item = true;
                    } else {
                        $show_item = false;
                    }
                }
                $start_month = $item["startdate"]["month"];
                $this_month = strftime('%m', $checkdate);
                $interval = $this_month - $start_month;
                $devided = $interval / $every;
                $match = (int) $devided;
                if ($devided != $match) {
                    $show_item = false;
                }
                break;
            case 'yearly':
                if ($item['cyclus']['yearly_choice'] == 1) {
                    if ($item["cyclus"]["yearly_day"] == $day && $item["cyclus"]["yearly_month"] == $month) {
                        $show_item = true;
                    } else {
                        $show_item = false;
                    }
                } else {
                    /* xth weekday of month */
                    $xth_day = schedulertools::nthDay($item["cyclus"]["yearly_month_time"], $item["cyclus"]["yearly_weekday_list"], $month, $year);
                    if ($item["cyclus"]["yearly_month2"] == $month && $day == $xth_day) {
                        $show_item = true;
                    } else {
                        $show_item = false;
                    }
                }
                break;
        }
        return $show_item;
    }

    /**
     * Check for which users the user is an assistant
     *
     * @param int $userid
     * @return array Array with user id's
     */
    function assistantFor($userid)
    {
        static $assistantFor = null;

        if (is_null($assistantFor)) {
            $db = &atkGetDb();
            $sql = "SELECT userid FROM scheduler_userassistants WHERE employeeid='{$userid}'";
            $nrows = $db->getrows($sql);
            $assistantFor = array();
            for ($i = 0; $i < count($nrows); $i++) {
                $assistantFor[] = $nrows[$i]["userid"];
            }
        }
        return $assistantFor;
    }

    /**
     * Check if a user is an attendee for the given item
     *
     * @param int $item_id Id of the scheduler item
     * @param int $userid Id of the user
     * @return boolean True if it's an attendee
     */
    function isAttendee($item_id, $userid)
    {
        $db = &atkGetDb();
        $sql = "SELECT person_id FROM scheduler_attendees WHERE scheduler_id='{$item_id}' AND person_id='{$userid}'";
        $nrows = $db->getrows($sql);
        if (count($nrows) == 1)
            return true;
        return false;
    }

    /**
     * Update recurring events for a given period
     *
     * @param string $startdate Start date
     * @param string $enddate End date
     */
    function updateRecurringEvents($startdate, $enddate, $rec = null)
    {
        $db = &atkGetDb();
        atkimport("module.scheduler.utils.schedulertools");
        $tmp_startdate = mktime(0, 0, 0, substr($startdate, 5, 2), substr($startdate, 8, 2), substr($startdate, 0, 4));
        $tmp_enddate = mktime(0, 0, 0, substr($enddate, 5, 2), substr($enddate, 8, 2), substr($enddate, 0, 4));

        if (!is_null($rec)) {
            $nrows = array($rec);
        } else {
            $scheduler = &atkGetNode("scheduler.scheduler");
            $condition = "scheduler_scheduler.recur!='once'
                    AND (scheduler_scheduler.lastdate IS NULL OR  (scheduler_scheduler.lastdate > $tmp_startdate AND scheduler_scheduler.lastdate < $tmp_enddate))
                    AND ((cyclus.end_choice=1) OR
                         (cyclus.end_choice=2 AND cyclus.cyclus_enddate >='$startdate') OR
                         (cyclus.end_choice=3 AND (scheduler_scheduler.times is null OR cyclus.cyclus_times > scheduler_scheduler.times)))";

            $nrows = $scheduler->selectDb($condition, "", "", array('attendees', 'location', 'description', 'category', 'priority', 'private', 'nonblocking', 'notes', 'owner', 'ownerattendee', 'scheduler_cyclus.exceptions'));
        }
        for ($i = 0, $_i = count($nrows); $i < $_i; $i++) {
            $rec = $nrows[$i];
            $id = $rec["id"];
            if (!is_null($rec)) {
                $startdate = date("Y-m-d", ($rec["lastdate"]));
            } else {
                $startdate = date("Y-m-d", ($rec["lastdate"] + DAY));
            }
            $endstamp = $rec["lastdate"] + (32 * DAY);
            if ($endstamp < $tmp_enddate)
                $endstamp = $tmp_enddate;
            $enddate = date("Y-m-t", $endstamp);
            $endstamp = strtotime($enddate);
            $dates = schedulertools::getDates($rec, $startdate, $enddate);
            foreach ($dates as $date) {
                $tmp_start = mktime($rec["starttime"]["hours"], $rec["starttime"]["minutes"], $rec["starttime"]["seconds"], $rec["startdate"]["month"], $rec["startdate"]["day"], $rec["startdate"]["year"]);
                $tmp_end = mktime($rec["endtime"]["hours"], $rec["endtime"]["minutes"], $rec["endtime"]["seconds"], $rec["enddate"]["month"], $rec["enddate"]["day"], $rec["enddate"]["year"]);
                $duration = $tmp_end - $tmp_start;

                $startdate = mktime($rec["starttime"]["hours"], $rec["starttime"]["minutes"], $rec["starttime"]["seconds"], substr($date, 5, 2), substr($date, 8, 2), substr($date, 0, 4));
                $enddate = $startdate + $duration;

                $sql = "INSERT IGNORE INTO scheduler_dates (scheduler_id,startdate,enddate) VALUES ('$id','$startdate','$enddate')";
                $db->query($sql);
            }
            $sql = "UPDATE scheduler_scheduler SET lastdate = '$endstamp', times =  COALESCE(times,0)+" . count($dates) . " WHERE id='$id'";
            $db->query($sql);
            $db->commit();
        }
        return true;
    }

}

?>