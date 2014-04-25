<?php

/**
 * One line description of the class
 *
 * Full description of the class
 *
 * @author Dennis-Jan Broerse <dennisjan@ibuildings.nl>
 * @package package
 * @subpackage subpackage
 *
 */
class reportutils
{

    public function get_employees($user_id, $allemployees = false)
    {
        $user = getUser();

        if (is_array($user)) {
            if ($allemployees)
                $records = $this->getEmployeesArray();
            else
                $records = $this->getEmployeesArray($user['id']);

            $employee_code = $this->getEmployeeCode($records, $user_id, $user);

            return $employee_code;
        }
    }

    /**
     * Returns all employees, optionally filtered by supervisor.
     *
     * @param integer $managerid Optional manager id
     * @return array List of employees
     */
    public function getEmployeesArray($managerid = '')
    {
        // If no supervisor parameter is given, return just all active employees
        if (empty($managerid))
            return $this->getEmployeesFromDb();

        // Otherwise, get the employees indexed by id and filter out all that are supervised by the given manager
        $employees = $this->getEmployeesById();
        $result = array();
        foreach ($employees as $employee) {
            if ($this->hasManager($employees, $employee['id'], $managerid))
                $result[] = $employee;
        }
        return $result;
    }

    /**
     * Tells whether an employee has a given other employee as his/her (in)direct manager
     *
     * @param array $employees List of employees indexed by id
     * @param integer $employeeid Id of the subemployee
     * @param integer $managerid Id of the proposed supervisor employee
     * @return boolean True if the the other person is indeed the (in)direct manager
     */
    private function hasManager($employees, $employeeid, $managerid, $excludeids = null)
    {
        $excludeids[] = $employeeid;
        $supervisorid = $employees[$employeeid]['supervisor'];
        if (empty($supervisorid) || in_array($supervisorid, $excludeids))
            return false;
        elseif ($supervisorid == $managerid)
            return true;
        else
            return $this->hasManager($employees, $supervisorid, $managerid, $excludeids);
    }

    /**
     * Returns an array containing all employees indexed by id
     *
     * @return array Employees indexed by id
     */
    private function getEmployeesById()
    {
        $result = array();
        foreach ($this->getEmployeesFromDb() as $employee) {
            $result[$employee['id']] = $employee;
        }
        return $result;
    }

    /**
     * Get all active employees (id, lastname, userid, firstname) from the database in a numerical array
     *
     * @return array All active employees
     */
    private function getEmployeesFromDb()
    {
        $sql = "SELECT person.id,person.lastname,person.userid, person.firstname, person.supervisor
              FROM person
              WHERE person.status='active'
              AND person.role='employee'
              ORDER BY person.lastname
             ";
        return atkGetDb()->getrows($sql);
    }

    function getEmployeeCode($records, $selected_userid, $user)
    {
        // Nobody selected, select 'all users'
        if ($selected_userid == "new" || !$selected_userid || $selected_userid == "")
            $sel = ' selected="selected"';
        else
            $sel = '';

        $employee_code = '<option value="all"' . $sel . '>' . atktext("allusers");

        // Logged-in user selected, select this option
        if ($selected_userid == $user['id'])
            $sel = ' selected="selected"';
        else
            $sel = '';

        // Add logged in-user, because default not present in dropdown
        $employee_code .= '<option value="' . addslashes($user["id"]) . '" ' . $sel . '>' . $user["lastname"] . ', ' . $user["firstname"] . '</option>';

        // Check if other user is selected
        for ($i = 0, $_i = count($records); $i < $_i; $i++) {
            if ($selected_userid == $records[$i]["id"])
                $sel = ' selected="selected"';
            else
                $sel = '';

            $employee_code.='<option value="' . addslashes($records[$i]["id"]) . '"' . $sel . '>' . $records[$i]["lastname"] . ', ' . $records[$i]["firstname"] . '</option>';
        }

        return $employee_code;
    }

    function getLevelEmployees($selected_userid)
    {
        atkdebug("getLevelEmployees function called.");
        $user = getUser();

        if (is_array($user)) {
            $harvest = atkHarvestModules("getEmpArray", '', true);

            if (is_array($harvest) && count($harvest) > 0) {
                if (moduleExists('advancedsecurity'))
                    $empArray = $harvest['advancedsecurity'];

                $empCode = $this->getEmployeeCode($empArray, $selected_userid, $user);

                return $empCode;
            }
            else {
                atkwarning("Harvester getEmpArray is empty. Returning only himself.");

                $empCode = $this->getEmployeeCode(array(), $selected_userid, $user);

                return $empCode;
            }
        }
    }

    function get_functionlevels($selected_level_id, $lowerlevels)
    {
        $params = array("selected_level_id" => $selected_level_id,
            "lower_levels" => $lowerlevels);

        $harvest = atkHarvestModules("getFuncLevelDropDown", $params, true);

        if (is_array($harvest) && count($harvest) > 0) {
            if (moduleExists('advancedsecurity'))
                $func_code = $harvest['advancedsecurity'];
        }
        else {
            $db = &atkGetDb();
            $sql = "  SELECT DISTINCT functionlevel.id as id, functionlevel.name as name
                FROM functionlevel
                LEFT JOIN person ON (person.functionlevel = functionlevel.id)";
            $records = $db->getrows($sql);

            $sel = (!$selected_level_id || $selected_level_id == "all") ? ' selected="selected"'
                    : '';
            $func_code = '<select name="functionlevelswitch"><option value="all"' . $sel . '>' . atktext("allfunctionlevels") . '</option>';
            foreach ($records as $record) {
                $sel = ($selected_level_id == $record['id']) ? ' selected="selected"'
                        : '';
                $func_code .= '<option value="' . $record['id'] . '"' . $sel . '>' . $record['name'] . '</option>';
            }
            $func_code .= '</select>';
        }
        return $func_code;
    }

    /**
     * Get where-clause for functionlevel
     * This depends on the state of the functionlevelswitch and lowerlevels-checkbox
     *
     * @param string $functionlevelswitch
     * @param string $lowerlevels
     * @return string $condition where-clause
     */
    function getFunctionLevelCondition($functionlevelswitch, $lowerlevels)
    {
        $condition = "";

        if ($functionlevelswitch && $functionlevelswitch != "all") {
            if (moduleExists('advancedsecurity') && $lowerlevels == 'on') {
                require_once(moduleDir('advancedsecurity') . 'utils/class.functionlevelsutils.inc');
                $flutils = &atknew('advancedsecurity.utils.functionlevelutils');

                $levels = $flutils->getFunctionLevelBelow($functionlevelswitch, true);

                if (!empty($levels)) {
                    // foreach: uit $level de $level_ids halen
                    if (count($levels) == 0)
                        $ids = 'null';
                    else {
                        foreach ($levels as $level)
                            $ids .= "'" . $level['id'] . "',";

                        $ids = substr($ids, 0, -1); // strip the last comma
                    }

                    $condition = " person.functionlevel IN (" . $ids . ")";
                } else {
                    $condition = " person.functionlevel='$functionlevelswitch'";
                }
            } else {
                $condition = " person.functionlevel='$functionlevelswitch'";
            }
        }

        atkdebug("reportutils::getFunctionLevelCondition | Condition = $condition");
        return $condition;
    }

    /**
     * Get contracthours for seleted users
     * This depends on the state of the functionlevelswitch and lowerlevels-checkbox
     *
     * @param string $employeeswitch
     * @param string $userid
     * @param string $enddate
     * @param string $startdate
     * @param string $functionlevelswitch
     * @return array $contracts
     */
    function getContractHours($employeeswitch, $userid, $enddate, $startdate, $functionlevelswitch)
    {
        // determine contracthours per week..
        $query = "SELECT
                uc_hours, startdate, enddate, person.id, lastname, firstname
              FROM
                 usercontract, person
              WHERE ";

        if ($employeeswitch == "supervisor") {
            if ($userid != "all") {
                $query.= " person.supervisor = " . $userid . " AND ";
            }
        } else {
            if ($userid != "all") {
                $query.= " usercontract.userid =" . $userid . " AND ";
            }
        }

        $functionlevel_condition = $this->getFunctionLevelCondition($functionlevelswitch, $functionlevel_condition);
        if (!empty($functionlevel_condition))
            $functionlevel_condition = "AND " . $functionlevel_condition;

        $query.= "  startdate <= '$enddate'
                AND (enddate >= '$startdate' OR enddate IS NULL)
                AND usercontract.userid = person.id
                $functionlevel_condition
                ORDER BY startdate";

        $db = &atkGetDb();
        $contracts = $db->getrows($query);

        return $contracts;
    }

    /**
     * Get IDs from project and his subprojects
     *
     * @param integer $projectid - master project id
     * @return array with project and his subprojects
     */
    static function getProjectAndSubProjectId($projectid)
    {
        $id = array();
        if (!empty($projectid)) {
            $db = &atkGetDb();
            $rows = $db->getrows("SELECT id FROM project WHERE master_project='$projectid'");

            $id[] = $projectid;

            foreach ($rows as $row) {
                $id[] = $row['id'];
            }
        }
        return $id;
    }

}

?>