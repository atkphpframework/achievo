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
 * Scheduler month view
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_monthview extends scheduler_view
{
    var $m_view = "month";

    /**
     * Navigate through months using prev/next month link
     *
     * @return html code containing links for previous month, current month and next month
     */
    function _getNav()
    {
        $theme = &atktheme::getInstance();
        $prevTime = date("Y-m-d", mktime(0, 0, 0, $this->m_model->getMonth() - 1, 1, $this->m_model->getYear()));
        $prevUrl = atkSelf() . "?view=month&viewdate=$prevTime";
        $prevImg = '<img src="' . $theme->imgPath("arrow_left.gif", "scheduler") . '" border=0 width=16 height=16 title="' . atktext("previous_month", "scheduler") . '" alt="' . atktext("previous_month", "scheduler") . '">';

        $thisTime = date("Y-m") . "-1";
        $thisUrl = atkSelf() . "?view=month&viewdate=$thisTime";

        $nextTime = date("Y-m-d", mktime(0, 0, 0, $this->m_model->getMonth() + 1, 1, $this->m_model->getYear()));
        $nextUrl = atkSelf() . "?view=month&viewdate=$nextTime";
        $nextImg = '<img src="' . $theme->imgPath("arrow_right.gif", "scheduler") . '" border=0 width=16 height=16 title="' . atktext("next_month", "scheduler") . '" alt="' . atktext("next_month", "scheduler") . '">';

        $this->m_viewTitle = "<b>" . atktext(atk_strtolower(date("F", mktime(0, 0, 0, $this->m_model->getMonth(), 1, $this->m_model->getYear())))) . " " . $this->m_model->getYear() . "</b>";
        $theme = &atktheme::getInstance();

        return href($prevUrl, $prevImg) . "&nbsp;" . href($nextUrl, $nextImg) . "&nbsp;" . href($thisUrl, atktext("current_month", "scheduler"));
    }

    /**
     * The actual displaying function
     *
     * @return string Returns a renderd day matrix
     */
    function _dumpMatrix()
    {
        $month = $this->m_model->getMonth();
        $year = $this->m_model->getYear();
        $user = &atkGetUser();

        atkimport("modules.utils.dateutil");
        $days = dateutil::short_daylist();
        for ($i = 0; $i <= 6; $i++)
            $days[$i] = atkText($days[$i], "atk");
        // On what day does our calendar start (0 sunday or 1 monday)
        $daystart = ($user["startday"] != "" ? $user["startday"] : 0);

        $months = array();
        $months[0] = atkText("january");
        $months[1] = atkText("february");
        $months[2] = atkText("march");
        $months[3] = atkText("april");
        $months[4] = atkText("may");
        $months[5] = atkText("june");
        $months[6] = atkText("july");
        $months[7] = atkText("august");
        $months[8] = atkText("september");
        $months[9] = atkText("october");
        $months[10] = atkText("november");
        $months[11] = atkText("december");


        $days_in_month = strftime("%d", mktime(0, 0, 0, $month + 1, 0, $year));
        $first_day_of_month = date("w", mktime(0, 0, 0, $month, 1, $year));

        $tmp = $this->_view_header();

        $tmp.='<table width="100%">
             <tr>
              <td valign="top">';
        $tmp.='<table border="0" cellpadding="0" cellspacing="0" width="100%">';
        $tmp.='<tr style="background-color: #DDDDDD;"><td width="40">' . atktext('week', 'scheduler') . '</td>';

        $day_number = $daystart;
        for ($i = 0; $i < 7; $i++) {
            if ($day_number == 7)
                $day_number = 0;
            $tmp.='<td align="center">' . $days[$day_number] . '</td>';
            $day_number++;
        }
        $tmp.='</tr>';

        $cellcount = $first_day_of_month - $daystart;
        if ($cellcount < 0)
            $cellcount = 7 + $cellcount;
        $bg_color = "#FFFFFF";
        $tmp.='<tr style="background-color: ' . $bg_color . '; height: 100px;">';

        $week_number = $this->m_model->getWeekNumber(1, $month, $year);

        $date = $year . "-" . sprintf("%02d", $month) . "-" . sprintf("%02d", 1);
        $tmp.='<td style="border: 1px solid #000000;"><i>' . href(atkSelf() . "?view=week&viewdate=" . $date, $week_number, SESSION_DEFAULT, false, 'title="' . atktext("weekview", "scheduler") . '"') . '</i>&nbsp;</td>';
        for ($i = 1; $i <= $cellcount; $i++) {
            $tmp.='<td>&nbsp;</td>';
        }
        $theme = &atktheme::getInstance();
        $current_day = date("d");
        $current_month = date("m");
        $current_year = date("Y");
        $schedulerItemObj = atkInstance("module.scheduler.utils.scheduleritem");
        for ($i = 1; $i <= $days_in_month; $i++) {
            $date = $year . "-" . sprintf("%02d", $month) . "-" . sprintf("%02d", $i);

            if ($cellcount == 7) {
                if ($bg_color == "#FFFFFF") {
                    $bg_color = "#EEEEEE";
                } else {
                    $bg_color = "#FFFFFF";
                }
                $tmp.="</tr><tr style=\"background-color: $bg_color; height: 100px;\">\n";
                $week_number = $this->m_model->getWeekNumber($i, $month, $year);
                $tmp.='<td style="border: 1px solid #000000;"><i>' . href(atkSelf() . "?view=week&viewdate=" . $date, sprintf("%02d", $week_number), SESSION_DEFAULT, false, 'title="' . atktext("weekview", "scheduler") . '"') . '</i>&nbsp;</td>';
                $cellcount = 0;
            }
            $cellcount++;
            if (($i == $current_day) && ($year == $current_year) && ($month == $current_month))
                $class = "HoverLinkSelected";
            else
                $class = "HoverLink";

            $date = $year . "-" . sprintf("%02d", $month) . "-" . sprintf("%02d", $i);
            if ($date == date("Y-m-d"))
                $class = "calendarToday";
            else
                $class = "calendarNormal";

            // Todo: Get HolidayTitle
            $holiday_title = "";

            $tmp.='<td align="left" valign="top" style="border: 1px solid #000000;">[&nbsp;' . href(atkSelf() . "?view=day&viewdate=" . $date, $i, '', '', 'class="' . $class . '"' . $holiday_title) . '&nbsp;]&nbsp;';
            $addicon = '<img src="' . $theme->imgPath("add.gif", "scheduler") . '" border="0" width="15" height="15" title="' . atktext("additem", "scheduler") . '" alt="' . atktext("additem", "scheduler") . '">';
            $tmp.=href(atkSelf() . "?atkaction=add&atknodetype=scheduler.scheduler&viewdate=" . $date, $addicon, SESSION_NESTED) . '<br>';
            if ($this->m_userPrefs["showlunarphases"]) {
                $timestamp = mktime(12, 0, 0, $month, $i, $year);
                $moonphase = $this->getMoonPhaseIcon($timestamp);
                $tmp.=($moonphase != "" ? "$moonphase" : "");
            }
            // Allday appointments
            if (array_key_exists($date, $this->m_model->m_allDayItems) && is_array($this->m_model->m_allDayItems[$date])) {
                for ($j = 0; $j < count($this->m_model->m_allDayItems[$date]); $j++) {
                    $tmp.=$schedulerItemObj->render($this->m_model->m_allDayItems[$date][$j], true, $date, true);
                }
            }
            // Other appointments
            if (array_key_exists($date, $this->m_model->m_appointmentItems) && is_array($this->m_model->m_appointmentItems[$date])) {
                for ($j = 0; $j < count($this->m_model->m_appointmentItems[$date]); $j++) {
                    $tmp.=$schedulerItemObj->render($this->m_model->m_appointmentItems[$date][$j], true, $date, true);
                }
            }
            $tmp.='</td>';
        }
        $remaining_cells = 7 - $cellcount;

        for ($i = 0; $i < $remaining_cells; $i++) {
            $tmp.='<td>&nbsp;</td>';
        }

        $tmp.='</tr>';
        $tmp.='</table>';
        $tmp.='<br>' . $this->m_model->renderLegend();
        $tmp.='</td><td width=150 valign="top">';
        $tmp.=$this->m_model->_getSmallCalendar(substr($this->m_model->m_small_viewdate, 8, 2), substr($this->m_model->m_small_viewdate, 5, 2), substr($this->m_model->m_small_viewdate, 0, 4));
        $tmp.="<br>";
        $next_date = date("Y-m-d", mktime(12, 0, 0, substr($this->m_model->m_small_viewdate, 5, 2) + 1, substr($this->m_model->m_small_viewdate, 8, 2), substr($this->m_model->m_small_viewdate, 0, 4)));
        $tmp.=$this->m_model->_getSmallCalendar(substr($next_date, 8, 2), substr($next_date, 5, 2), substr($next_date, 0, 4), false);
        $tmp.="<br>";
        $tmp.=$this->m_model->_getEmployeesCheckboxesForm();

        $tmp.='  </td>
             </tr>
             </table><br><br>
             ';
        return $tmp;
    }

    /**
     * Return moonPhase icon for a given date
     *
     * @param int $timestamp
     * @return string An html image string or an empty string
     */
    function getMoonPhaseIcon($timestamp)
    {
        atkimport("modules.scheduler.utils.moonphases");

        $moonphase = moonphases::getMoonPhase($timestamp);
        if ($moonphase == "")
            return "";
        switch ($moonphase) {
            case "ne": $text = atktext("full_moon", "scheduler");
                break;
            case "ny": $text = atktext("no_moon", "scheduler");
                break;
            case "ne2": $text = atktext("half_moon", "scheduler");
                break;
            case "ny2": $text = atktext("half_moon", "scheduler");
                break;
        }
        $theme = &atktheme::getInstance();
        return '<img src="' . $theme->imgPath($moonphase . ".gif", "scheduler") . '" width=15 height=15 title="' . $text . '" alt="' . $text . '">';
    }

    /**
     * Render the view
     *
     * @return string A renderd view
     */
    function _render()
    {
        $startdate = date("Y-m-d", mktime(12, 0, 0, $this->m_model->getMonth(), 1, $this->m_model->getYear()));
        $enddate = date("Y-m-d", mktime(12, 0, 0, $this->m_model->getMonth() + 1, 0, $this->m_model->getYear()));
        $this->m_model->getItems($startdate, $enddate);
        $this->m_model->getLegend();

        return $this->_dumpMatrix();
    }

}

?>