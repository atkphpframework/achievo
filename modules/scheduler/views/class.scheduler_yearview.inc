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
 * Scheduler year view
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */class scheduler_yearview extends scheduler_view
{

    /**
     * scheduler yearview constructor
     */
    function scheduler_yearview()
    {
        $this->setView("year");
    }

    /**
     * navigate through years using prev/next year link
     * @returns html code containing links for previous year, current year and next year
     */
    function _getNav()
    {
        $prevTime = date("Y-m-d", mktime(0, 0, 0, 1, 1, $this->m_year - 1));
        $prevUrl = atkSelf() . "?view=year&viewdate=$prevTime&dayview=" . $this->showGlobal;

        $thisTime = date("Y-m-d");
        $thisUrl = atkSelf() . "?view=year&viewdate=$thisTime&dayview=" . $this->showGlobal;

        $nextTime = date("Y-m-d", mktime(0, 0, 0, 1, 1, $this->m_year + 1));
        $nextUrl = atkSelf() . "?view=year&viewdate=$nextTime&dayview=" . $this->showGlobal;

        $this->title = "<b>" . atktext(atk_strtolower(date("F", mktime(0, 0, 0, $this->m_month, 1, $this->m_year)))) . " " . $this->m_year . "</b>";

        return href($prevUrl, atktext("previous_year")) . "&nbsp;" . href($thisUrl, atktext("this_year")) . "&nbsp;" . href($nextUrl, atktext("next_year"));
    }

    /**
     * the actual displaying function
     * @return string Returns a renderd year matrix
     */
    function _dumpMatrix()
    {
        $tmp = $this->_view_header();
        $tmp.= '<table cellpadding=5>
            <tr>
              <td valign="top">' . $this->_getSmallCalendar(1, 1, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 2, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 3, $this->m_year, false) . '</td>
            </tr>
            <tr>
              <td valign="top">' . $this->_getSmallCalendar(1, 4, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 5, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 6, $this->m_year, false) . '</td>
            </tr>
            <tr>
              <td valign="top">' . $this->_getSmallCalendar(1, 7, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 8, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 9, $this->m_year, false) . '</td>
            </tr>
            <tr>
              <td valign="top">' . $this->_getSmallCalendar(1, 10, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 11, $this->m_year, false) . '</td>
              <td valign="top">' . $this->_getSmallCalendar(1, 12, $this->m_year, false) . '</td>
            </tr>
            </table>';


        return $tmp;
    }

    /**
     * Render the view
     * @return string A renderd view
     */
    function renderView()
    {
        $this->_setUserSettings();
        $this->getItems('2003-01-01', '2003-12-31');
        return $this->_dumpMatrix();
    }

}

?>