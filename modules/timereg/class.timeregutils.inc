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
class TimeregUtils
{

    /**
     * Translate viewdate to unix timestamp, return current date's timestamp if
     * $viewdate is empty.
     *
     * @param string $viewdate  date string of yyyy-mm-dd format
     * @return int              unix timestamp
     */
    function getViewTime($viewdate)
    {
        if ($viewdate != "") {
            $viewtime = mktime(12, 12, 0, substr($viewdate, 5, 2), substr($viewdate, 8, 2), substr($viewdate, 0, 4));
        } else {
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
     * @param boolean $checklock Check for locks
     * @return array
     */
    function getWeekDates($viewdate, $checklock = true)
    {
        $viewtime = TimeregUtils::getViewTime($viewdate);

        $weekdates = array();
        $day = 86400;
        $weekday = date("w", $viewtime);
        $startday = atkconfig("startday", 1);
        if ($weekday == $startday) {
            $weekstartday = $viewtime;
        } elseif ($weekday < $startday) {
            $diff = ($startday - $weekday) + (6 - $startday);
            $weekstartday = $viewtime - ($diff * $day);
        } elseif ($weekday > $startday) {
            $diff = $weekday - $startday;
            $weekstartday = $viewtime - ($diff * $day);
        }

        //$weekstartday = $weeknumber["firstweekstart"];
        for ($i = 0; $i <= 8; $i++) {
            $multiplier = ($i - 1);
            $day_timestamp = $weekstartday + ($day * $multiplier);
            $weekdates[$i]['date'] = date("Y-m-d", $day_timestamp);
            $weekdates[$i]['day'] = atktext(atk_strtolower(date("D", $day_timestamp)));
            $weekdates[$i]['dateday'] = atk_strtolower(date("d", $day_timestamp));
            $weekdates[$i]['month'] = atktext(atk_strtolower(date("F", $day_timestamp)));
            $weekdates[$i]['stamp'] = $day_timestamp;
            if ($checklock)
                $weekdates[$i]['lock'] = Locking::getLockType(atkArrayNvl(getUser(), "id"), $weekdates[$i]['date']);
        }
        return $weekdates;
    }

}

?>
