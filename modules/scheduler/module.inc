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
define('HOUR', 3600);
define('DAY', 86400);

$modifiers[] = "employee.userprefs";

/**
 * Module file for the menu items and security
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class mod_scheduler extends achievoModule
{

    function getNodes()
    {
        registerNode("scheduler.scheduler", array("admin", "add", "edit", "delete", "all_non_private"));
        registerNode("scheduler.scheduler_category", array("admin", "add", "edit", "delete"));
        registerNode("scheduler.scheduler_holidays", array("admin", "add", "edit", "delete" /* ,"import","export","view_holidays" */));
        registerNode("scheduler.scheduler_email_template", array("admin", "edit"));
    }

    function getMenuItems()
    {
        $this->menuitem("scheduler", dispatch_url("scheduler.scheduler", "admin"), "main", array("scheduler.scheduler", "admin"), 500);
        $this->menuitem("scheduler_category", dispatch_url("scheduler.scheduler_category", "admin"), "setup", array("scheduler.scheduler_category", "admin"));
        $this->menuitem("scheduler_holidays", dispatch_url("scheduler.scheduler_holidays", "admin"), "setup", array("scheduler.scheduler_holidays", "admin"));
        $this->menuitem("scheduler_email_templates", dispatch_url("scheduler.scheduler_email_template", "admin"), "setup", array("scheduler.scheduler_email_template", "admin"));
    }

    function getPimItems()
    {
        return array("schedule" => "getPimListView");
    }

    function getPimListView()
    {
        include_once(moduleDir("scheduler") . "pim.inc");
        return pimScheduler();
    }

    function getDependencies($mode)
    {
        return array("person", "email_template");
    }

    function userprefs_modifier(&$node)
    {
        userelation("atkOneToOneRelation");
        $node->add(new atkOneToOneRelation("scheduler", "scheduler.scheduler_userpreferences", "userid", AF_ONETOONE_INTEGRATE | AF_ONETOONE_ERROR), "scheduler");
    }

    function search($expression)
    {
        $db = &atkGetDb();
        $res = array();
        $expression = $db->escapeSQL($expression);
        $sql = "SELECT scheduler_scheduler.*,scheduler_dates.startdate as realstartdate,scheduler_dates.enddate as realenddate FROM scheduler_scheduler,scheduler_dates WHERE scheduler_scheduler.id=scheduler_dates.scheduler_id AND (title LIKE '%$expression%' OR description LIKE '%$expression%') ORDER BY realstartdate";
        $nrows = $db->getrows($sql);
        $res["scheduler"] = $this->recLinks($nrows, "scheduler.scheduler");
        return $res;
    }

    function recLinks($recordset, $nodetype)
    {
        $node = &atkGetNode($nodetype);

        $res = array();

        $node = &atkGetNode($nodetype);
        if ($node->allowed("view")) {
            for ($i = 0, $_i = count($recordset); $i < $_i; $i++) {
                $item["title"] = date("d-m-Y H:i", $recordset[$i]["realstartdate"]) . " " . date("H:i", $recordset[$i]["realenddate"]) . " " . $recordset[$i]["title"];
                $item["url"] = dispatch_url($nodetype, "view", array("atkselector" => $node->primaryKey($recordset[$i])));
                $res[] = $item;
            }
        }

        return $res;
    }

}

?>