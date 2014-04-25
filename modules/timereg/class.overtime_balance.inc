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
userelation("atkmanytoonerelation");
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
useattrib("timereg.manualattribute");
useattrib("atknumberattribute");
atkImport("modules.utils.dateutil");

/**
 * Node for keeping track of the overtime balance
 *
 * @package achievo
 * @subpackage timereg
 */
class overtime_balance extends atkNode
{

    /**
     * Constructor
     */
    function overtime_balance()
    {
        $this->atkNode("overtime_balance");
        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new atkManyToOneRelation("userid", "employee.employeeselector", AF_OBLIGATORY | AF_SEARCHABLE | AF_READONLY));
        $this->add(new atkDateAttribute("day", "", "", 0, date("Y-m-d"), AF_SEARCHABLE | AF_OBLIGATORY));
        $this->add(new atkNumberAttribute("balance", AF_OBLIGATORY, 10, 2));
        $this->add(new manualAttribute("manual", AF_HIDE, 1, 0));
        $this->add(new atkTextAttribute("remark"));

        $this->setTable("overtime_balance", "overtime_balance");
        $this->m_securityMap["add"] = "edit";
        $this->m_securityMap["save"] = "edit";
    }

    /**
     * Delete overtime balance after an update
     *
     * @param array $rec
     * @return boolean
     */
    function postUpdate($rec)
    {
        $this->deleteBalance($rec["userid"]["id"], dateutil::arr2str($rec["day"], "Y-m-d"));
        return true;
    }

    /**
     * Delete overtime balance after adding a record
     *
     * @param array $rec
     * @return boolean
     */
    function postAdd($rec)
    {
        $this->deleteBalance($rec["userid"]["id"], dateutil::arr2str($rec["day"], "Y-m-d"));
        return true;
    }

    /**
     * Delete overtime balance after deleting a record
     *
     * @param array $rec
     * @return boolean
     */
    function postDel($rec)
    {
        $this->deleteBalance($rec["userid"]["id"], dateutil::arr2str($rec["day"], "Y-m-d"));
        return true;
    }

    /**
     * Create overtime link
     *
     * @param array $rec recorc
     * @return string
     */
    function getOvertimeLink($rec)
    {
        if ($rec["balance"]["balance"] != atktext("not_inserted"))
            $res = time_format(60 * $rec["balance"]["balance"], true);
        else
            $res = $rec["balance"]["balance"];

        $manual = $this->getLatestManualCorrection($rec["balance"]["userid"]["id"], 0, dateutil::arr2str($rec["balance"]["day"], "Y-m-d"));
        $manualid = $manual["id"];
        if ($manual != 0) {
            // check if this manualcorrection was made in the current week
            $enddate = dateutil::arr2str($rec["balance"]["day"], "Y-m-d");
            $startdate = startOfWeek($enddate);
            $manualdate = dateutil::arr2str($manual["day"], "Y-m-d");
            if ($manualdate <= $enddate && $manualdate >= $startdate)
                $manual = 1;
            else
                $manual = 0;
        }

        if ($this->allowed("edit") && $rec["balance"]["outputtype"] == 0) {
            if ($rec["balance"]["id"] != "" && $manual) {
                $url = dispatch_url("timereg.overtime_balance", "edit", array("atkselector" => "overtime_balance.id = " . $manualid));
            } elseif ($rec["balance"]["id"] != "") {
                $url = dispatch_url("timereg.overtime_balance", "edit", array("atkselector" => "overtime_balance.id = " . $rec["balance"]["id"]));
            } else {
                $url = dispatch_url("timereg.overtime_balance", "add", array("userid[id]" => $rec["balance"]["userid"]["id"], "day[day]" => $rec["balance"]["day"]["day"], "day[month]" => $rec["balance"]["day"]["month"], "day[year]" => $rec["balance"]["day"]["year"], "balance" => "0.00"));
            }
            $res = href($url, $res, SESSION_NESTED);
        }

        if ($this->allowed("delete") && $manual) {
            $deletelink = href(dispatch_url("timereg.overtime_balance", "delete", array("atkselector" => "overtime_balance.id = " . $manualid)), atktext("manualovertime_delete"), SESSION_NESTED);
        } else
            $deletelink = "";
        return $res . ($manual ? ' *' : '') . ' ' . $deletelink;
    }

    /**
     * Get balance of a user on a specific day
     *
     * @param int $day Day
     * @param int $userid User id
     * @return int
     */
    function getBalance($day, $userid)
    {
        $node = &atkGetNode("timereg.overtime_balance");
        $result = $node->selectDb("day <= '$day' AND userid.id = '$userid'", "day DESC", 1);

        $cnt_result = count($result);
        if (!$cnt_result) {
            $startingpoint = atkconfig::get("timereg", "timereg_overtime_balance_use_startingpoint", false);
            if ($startingpoint === "")
                return $this->getDummyBalance($day, $userid);
            else {
                $date = "";
                if ($startingpoint === true) {
                    $date = date("Y-m-d", adodb_mktime(0, 0, 0, 12, 31, date("Y") - 1));
                } else {
                    //assume the date is correctly formatted.
                    $date = $startingpoint;
                }

                $result[0] = $this->getDummyBalance($date, $userid, 0);
            }
        }
        $endtime = dateUtil::arr2stamp($result[0]["day"]);
        $starttime = dateUtil::str2stamp($day);
        if ($endtime < $starttime) {
            //recalculate
            return $this->recalculateBalance(date("Y-m-d", adodb_mktime(0, 0, 0, $result[0]["day"]["month"], $result[0]["day"]["day"] + 1, $result[0]["day"]["year"])), $day, $userid, $result[0]["balance"]);
        } else if (count($result))
            return $result[0];
        return 0;
    }

    /**
     * Get Dummy balance
     *
     * @param int $day Day
     * @param int $userid User id
     * @param int $balance Balance
     * @param int $id id
     * @return int
     */
    function getDummyBalance($day, $userid, $balance = "", $id = 0)
    {
        if ($balance === "")
            $res["balance"] = atktext("not_inserted");
        else
            $res["balance"] = $balance;
        $res["userid"]["id"] = $userid;
        $res["day"]["day"] = substr($day, 8, 2);
        $res["day"]["month"] = substr($day, 5, 2);
        $res["day"]["year"] = substr($day, 0, 4);
        if ($id != 0)
            $res["id"] = $id;
        $res["manual"] = 0;
        return $res;
    }

    function recalculateBalance($begin, $end, $userid, $oldbalance)
    {
        // calculate new balance
        $newbalance = $this->calculateNewBalance($userid, $begin, $end, $oldbalance);
        // update database with new balance
        $id = $this->updateOvertimeBalance($userid, $end, $newbalance);
        return $this->getDummyBalance($end, $userid, $newbalance, $id);
    }

    function getLatestManualCorrection($userid, $default, $day = "")
    {
        $node = &atkGetNode("timereg.overtime_balance");
        $result = $node->selectDb("userid.id = '$userid' AND manual = 1 " . ($day != ""
                    ? "AND day <= '$day' " : ""), "day DESC", 1);
        return (count($result[0]) ? $result[0] : $default);
    }

    function deleteBalance($userid, $day)
    {
        $db = &atkGetDb();
        $db->query("DELETE FROM overtime_balance WHERE (manual <> 1 OR manual IS NULL) AND day >= '$day' AND userid = '$userid'");
    }

    function addOvertimeBalance($userid, $day, $balance)
    {
        $this->debuglog(array("function" => "addOvertimeBalance", "userid" => $userid, "day" => $day, "balance" => $balance));
        if (strftime("%w", dateutil::str2stamp($day)) != 0)
            return;
        $db = &atkGetDb();
        $newid = $db->nextid("overtime_balance");
        $db->query("INSERT INTO overtime_balance (id, userid, day, balance, manual)
                  VALUES ('$newid', '$userid', '$day', '$balance', 0)");
    }

    function saveOvertimeBalance($userid, $day, $balance)
    {
        $this->debuglog(array("function" => "saveOvertimeBalance", "userid" => $userid, "day" => $day, "balance" => $balance));
        if (strftime("%w", dateutil::str2stamp($day)) != 0)
            return;
        $db = &atkGetDb();
        $sql = "UPDATE overtime_balance SET balance = $balance WHERE userid = '$userid' AND day = '$day'";
        $db->query($sql);
    }

    function getPercentage($id)
    {
        return atkArrayNvl($this->getPercentages(), $id, 100);
    }

    function updateOvertimeBalance($userid, $day, $balance)
    {
        $this->debuglog(array("function" => "updateOvertimeBalance", "userid" => $userid, "day" => $day, "balance" => $balance));
        if (strftime("%w", dateutil::str2stamp($day)) != 0)
            return;
        $db = &atkGetDb();
        $recs = $db->getrows("SELECT id FROM overtime_balance WHERE userid = '$userid' AND day = '$day'");
        if (!count($recs)) {
            $newid = $db->nextid("overtime_balance");
            $db->query("INSERT INTO overtime_balance (id, userid, day, balance, manual)
                  VALUES ('$newid', '$userid', '$day', '$balance', 0)");
            return $newid;
        } else {
            $db->query("UPDATE overtime_balance SET balance = $balance WHERE userid = '$userid' AND day = '$day'");
            return $recs[0]["id"];
        }
    }

    function calculateNewBalance($userid, $begin, $end, $oldbalance)
    {
        $percentages = $this->getPercentages();
        $overtimeids = $this->getOvertimeCompensationActivities();
        $contracthours = $this->getContractHours($begin, $end, $userid);
        $hours = $this->getUserHours($begin, $end, $userid);

        $total = 0;
        for ($i = 0; $i < count($hours); $i++) {
            $time = $hours[$i]["time"] / 60;

            if (count($percentages) && $hours[$i]["workperiod"] != 0)
                $percentage = $percentages[$hours[$i]["workperiod"]];
            else
                $percentage = 1;

            if (!$this->isOvertimeCompensationActivity($hours[$i]["activityid"], $overtimeids))
                $total += $percentage * $time;
        }

        return ($oldbalance + ($total - $contracthours));
    }

    function getUserHours($begin, $end, $userid)
    {
        $db = &atkGetDb();
        $sql = "SELECT * FROM hours WHERE userid = '$userid' AND activitydate >= '$begin' AND activitydate <= '$end'";
        return $db->getRows($sql);
    }

    function getPercentages()
    {
        static $s_percentages;

        if (isset($s_percentages))
            return $s_percentages;

        $db = &atkGetDb();
        $res = $db->getRows("SELECT id, percentage FROM workperiod");
        $s_percentages = array();
        for ($i = 0; $i < count($res); $i++) {
            $s_percentages[$res[$i]["id"]] = $res[$i]["percentage"] / 100;
        }
        return $s_percentages;
    }

    function getOvertimeCompensationActivities()
    {
        static $s_overtimecompact;

        if (isset($s_overtimecompact))
            return $s_overtimecompact;

        $db = &atkGetDb();
        $s_overtimecompact = $db->getrows("SELECT id FROM activity WHERE overtimecompensation = 1");
        return $s_overtimecompact;
    }

    function isOvertimeCompensationActivity($id, $overtimeactivities)
    {
        foreach ($overtimeactivities as $item) {
            if ($item["id"] == $id)
                return true;
        }
        return false;
    }

    /**
     * Get contract hours for an employee within given time frame
     *
     * @param string $begin Period begin in "yyyy-mm-dd" format
     * @param string $end Period end in "yyyy-mm-dd" format
     * @param int $userid Employee id
     * @return int Amount of hours
     */
    function getContractHours($begin, $end, $userid)
    {
        static $user_contracts = array();
        // Convert the given begindate and enddate to "yyyymmdd" format so we can run calculations on them
        // in order to determine if a given date is earlier than, equal to or later than another date
        $begindate = dateutil::str2str($begin);
        $enddate = dateutil::str2str($end);

        // Get a list of contracts that intersects with the given begin and end dates. The list of contracts
        // includes the amount of hours, the start- and enddate and the list of workdays for each contract.
        if (!array_key_exists($userid, $user_contracts)) {
            $db = &atkGetDb();
            $sql = "SELECT uc_hours, startdate, enddate, WorkingDays
                FROM usercontract
                WHERE userid = '$userid'";
            /*
              AND startdate <= '$end'
              AND (enddate >= '$begin' OR enddate IS NULL)";
             */
            $contracts = $db->getRows($sql);
            $user_contracts[$userid] = $contracts;
        } else {
            $contracts = $user_contracts[$userid];
        }

        // Start with an empty total
        $total = 0;

        // If we have found contract(s), we will loop through the list of it and start accumulating the total
        // amount of hours
        for ($i = 0, $_i = count($contracts); $i < $_i; $i++) {
            if ($contracts[$i]['startdate'] >= $end || ($contracts[$i]['enddate'] <= $begin && $contracts[$i]['enddate'] != ''))
                continue;

            // Get an array of working days that are specified with this contract
            $workdays = $this->getWorkDays($contracts[$i]["WorkingDays"]);

            // Convert the contract start and enddate to "yyyymmdd" format so we can use it in calculations
            $contractstartdate = dateutil::str2str($contracts[$i]["startdate"]);
            if (is_null($contracts[$i]["enddate"])) {
                $contractenddate = $enddate;
            } else {
                $contractenddate = dateutil::str2str($contracts[$i]["enddate"]);
            }

            // If no workingdays are specified, assume the working hours are spread evenly over 7 days
            $numworkdays = (count($workdays) > 0) ? count($workdays) : 7;

            // The contractend should be changed to the end of the period if it spans over it and the
            // contractstart should be changed to the start of the period if it starts before the periodstart
            if ($contractenddate > $enddate)
                $contractenddate = $enddate;
            if ($contractstartdate < $begindate)
                $contractstartdate = $begindate;

            // Calculate the amount of worked days within the period
            $numdays = $this->getNumDays($contractstartdate, $contractenddate, $workdays);

            // Calculate the amount of weeks (number of working days in the period divided by
            // the number of weekly working days) and multiply it with the amount of weekly hours
            $total += ($numdays / $numworkdays) * $contracts[$i]["uc_hours"];
        }
        // If no contracts are found, we just return 0
        return $total;
    }

    function getNumDays($begin, $end, $workdays)
    {
        $has_workdays = (count($workdays) > 0);

        $total = 0;

        foreach (dateutil::daysBetween($begin, $end) as $currentday => $date) {
            $dayofweek = dateUtil::str2str($currentday, "w");

            if (!$has_workdays || in_array($dayofweek, $workdays)) {
                $total++;
            }
        }
        return $total;
    }

    /**
     * Function retrieves the workingdays from the string strWorkingdays in an array.
     *
     * The format of strWorkindays is "1|2|3" (for mo, tue, we).     *
     *
     * @param string $strWorkingdays
     * @return array
     */
    function getWorkDays($strWorkingdays)
    {
        $strWorkingdays = trim($strWorkingdays);
        if (empty($strWorkingdays))
            return array();

        return explode("|", $strWorkingdays);
    }

    function debuglog($params)
    {
        if (!atkconfig::get("timereg", "overtimebalancedebugging", false))
            return;
        $logarr = $params;
        foreach (array("atknodetype", "atkaction", "atkselector") as $item)
            $logarr[$item] = $this->m_postvars[$item];
        foreach ($logarr as $key => $value)
            $log .= "$key=$value ";
        $log = "\n" . trim($log) . "\n" . atk_get_trace() . "\n\n\n";
        $fp = @fopen("achievotmp/overtime.log", "a");
        if ($fp) {
            fwrite($fp, $log);
            fclose($fp);
        }
    }

}

?>
