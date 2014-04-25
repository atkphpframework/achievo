<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
userelation("atkmanytoonerelation");

/**
 * class for selecting an employee from employees that you supervise.
 * @package achievo
 * @subpackage employee
 */
class myEmployeesAttribute extends atkManyToOneRelation
{

    function myEmployeesAttribute($name, $any_user_privilege = "", $flags = 0)
    {
        $this->atkManyToOneRelation($name, "employee.employee", $flags);

        $any_user = false;
        if ($any_user_privilege != "") {
            list($module, $node, $action) = explode(".", $any_user_privilege);

            $sm = &atkGetSecurityManager();
            $any_user = $sm->allowed($module . "." . $node, $action);
        }

        $filter = "status='active'";

        if (!$any_user) {
            // set a filter
            $user = getUser();
            $filter.= " AND supervisor=" . $user["id"];
        }
        $this->setDestinationFilter($filter);
    }

}

?>