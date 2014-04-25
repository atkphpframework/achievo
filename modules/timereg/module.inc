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
 * Module file for the menu items and security
 *
 * @package achievo
 * @subpackage timereg
 */
class mod_timereg extends achievoModule
{

    function getNodes()
    {
        registerNode("timereg.hours", array("admin", "add", "edit", "delete", "lock", "unlock", "any_user", "any_project"));
        registerNode("timereg.hours_approve", array("admin", "add", "delete", "approve", "disapprove", "any_user")); // niet
        registerNode("timereg.hours_lock", array("admin", "add", "delete")); // niet
        registerNode("timereg.workperiod", array("admin", "add", "edit", "delete"));
        registerNode("timereg.overtime_balance", array("edit", "delete"));
    }

    function getMenuItems()
    {
        $lockmode = atkConfig::get("timereg", "lockmode", "week");

        // Build the timereg menu
        $this->menuitem("time_registration", dispatch_url("timereg.hours", "admin"), "main", array("timereg.hours", "admin"), 100);
        $this->menuitem("hours_admin", dispatch_url("timereg.hours", "admin"), "time_registration", array("timereg.hours", "admin"));
        $this->menuitem("-", "", "time_registration");
        $this->menuitem("hours_lock_{$lockmode}", dispatch_url("timereg.hours_lock", "admin"), "time_registration", array("timereg.hours_lock", "admin"));
        $this->menuitem("hours_approve_{$lockmode}", dispatch_url("timereg.hours_approve", "admin"), "time_registration", array("timereg.hours_approve", "admin"));
        $this->menuitem("-", "", "main", true, 120);

        // Add a workperiod admin link to the setup menu
        $this->menuitem("workperiod_admin", dispatch_url("timereg.workperiod", "admin"), "setup", array("timereg.workperiod", "admin"));
    }

    function getPimItems()
    {
        return array("timeregistration" => "getTimeRegistration");
    }

    function getTimeregistration()
    {
        include_once(moduleDir("timereg") . "pim.inc");
        return pimTimeReg();
    }

    function getDependencies($mode)
    {
        return array("project", "employee");
    }

    function pdaNodes()
    {
        return array("time_registration_add" => dispatch_url("timereg.hourspda", "add"), "time_registration_admin" => dispatch_url("timereg.hourspda", "admin"));
    }

}

?>
