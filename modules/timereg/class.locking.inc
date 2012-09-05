<?php

include_once("achievotools.inc");

/**
 * Locking related functions for all modules using the monthly/weekly locking
 * and approval.
 *
 */
class Locking {

  /**
   * Check if this period (week or month) may be booked..
   *
   * @param $userid
   * @param $date     yy-mm-dd
   *
   * @return ""           - not locked
   *         "individual" - locked for the user only
   *         "all"        - locked for everybody (can only be unlocked by administrator)
  **/
  function getLockType($userid, $date)
  {

    static $s_locks = array();

    if (atkConfig::get("timereg", "lockmode", "week") == "week") {
      $period = weekstamp(adodb_mktime(0,0,0,substr($date,5,2),
                                       substr($date,8,2),
                                       substr($date,0,4)));
    } else {
      $period = substr($date,0,4).substr($date,5,2);
    }

    if ($s_locks[$userid][$period]=="")
    {
      $query = "SELECT
                  period, userid
                FROM
                  hours_lock
                WHERE
                  (userid = '$userid' OR userid IS NULL)
                AND
                  period = '$period'";
      $db = &atkGetDb();
      $res = $db->getrows($query);

      if (count($res))
      {
        $all = false;
        for ($i=0;$i<count($res);$i++)
        {
          if (empty($res[$i]["userid"]))
          {
            $s_locks[$userid][$period] = "all";
            $all = true;
          }
        }

        // apparently not locked for everybody.
        if (!$all)
        {
          $s_locks[$userid][$period] = "individual";
        }
      }
      else
      {
        $s_locks[$userid][$period]="none";
      }
    }

    return ($s_locks[$userid][$period]=="none"?"":$s_locks[$userid][$period]);
  } // end getLockType()


  /**
   * Locks the specified period. Will lock month or week according to config.
   *
   * @param String $period  The period we wish to lock
   * @param int $approved   Sets period to approved. 1 = true, 0 = false
  **/
  function lockPeriod($period, $approved=null)
  {

    $record = array();
    $record["userid"]["id"] = atkArrayNvl(getUser(), "id");
    $record["period"] = $period;
    $record["approved"] = $approved;
    $locknode = &atkGetNode("timereg.hours_lock");
    $lockmode = atkConfig::get('timereg','lockmode','week');

    $approveperiodpercoordinator = atkConfig::get('timereg','approve_period_per_coordinator') == true;
    $result = 0;
    if ($approveperiodpercoordinator)
    {
      // Add record for every coordinator
      // First, get all coordinators who have to validate - this are the coordinators
      // for the projects the person has registered time for this week.
      $db = &atkGetDb();

      if ($lockmode == "week") {
        $dateformat = "%Y%u";
      } else {
        $dateformat = "%Y%m";
      }

      $result = $db->getRows("
                SELECT DISTINCT person.id AS coordinator, person.firstname, person.lastname, person.email
                FROM hours
                LEFT JOIN phase ON (hours.phaseid = phase.id)
                LEFT JOIN project ON (project.id = phase.projectid)
                LEFT JOIN person ON (project.coordinator = person.id)
                WHERE hours.userid = '{$record['userid']['id']}'
                AND DATE_FORMAT(activitydate,'{$dateformat}') = '{$record['period']}'
                AND coordinator IS NOT NULL");
    }

    if ($approveperiodpercoordinator && count($result)>0)
    {
      foreach($result as $r)
      {
        $record['coordinator'] = $r['coordinator'];
        $locknode->addDb($record);

        if (atkConfig::get('timereg','mail_coordinators_on_lock') == true)
        {
          // sendout an e-mail to the coordinator
          $userdetails = array_pop($db->getRows("SELECT firstname,lastname,userid FROM person WHERE id='{$record["userid"]["id"]}'"));
          $username = $userdetails['userid'] . " ({$userdetails['firstname']} {$userdetails['lastname']})";

          $to = $r['firstname'] . " " . $r['lastname'] . " <{$r['email']}>";
          $subject = atktext('coordinator_mail_hours_locked_subject');

          $mailbody = atktext('coordinator_mail_hours_locked_bodytemplate');
          $mailbody = sprintf($mailbody,$username,$record["period"]);

          usermail($to,$subject,$mailbody,"From: ".atkConfig::get('timereg','from','Achievo <automailer@achievo.org>'));
        }
      }
    }
    else
    {
      $locknode->addDb($record);
    }
  }


  /**
   * Remove lock from specified period for current user.
   *
   * @param int $period
   * @access public
   * @return void
   */
  function unlockPeriod($period)
  {
    $userid = atkArrayNvl(getUser(), "id");
    $locknode = &atkGetNode("timereg.hours_lock");
    $locknode->deleteDb("hours_lock.period = '$period' AND hours_lock.userid = '$userid'");
  }


  /**
   * Return the period a date belongs to.
   *
   * @param String $date YYYY-MM-DD
   * @access public
   * @return void
   */
  function getPeriod($date) {
    $period = "";

    if (atkConfig::get("timereg", "lockmode", "week") == "week") {
      $period = weekstamp(adodb_mktime(0,0,0,substr($date,5,2),
                                       substr($date,8,2),
                                       substr($date,0,4)));
    } else {
      $period = substr($date,0,4).substr($date,5,2);
    }

    return $period;
  }

  /**
   * Returns lock icon if parameter isn't empty
   *
   * @param string $lock  if not "", function will return html for lock icon. $lock will typically be the calling node's $m_lock
   * @param string $size  sets height attribute of lock image
  */
  function getLockIcon($lock, $size="")
  {
    if ($lock!="")
    {
      // Period is locked
      return ' <img src="'.atkconfig("atkroot").'atk/images/lock.gif" border="0" height="'.$size.'">';
    }
  }


  /**
   * getFirstNonLockedDayOfPeriod
   *
   * @param array $days   Two-dimensional array.
   * @param mixed $offset Which index in $days day to begin looking at
   * @return string       Date of the first non-locked day of this period
  */
  function getFirstNonLockedDayOfPeriod($days, $offset) {

    $retval = $days[$offset]['date'];

    for ($i=$offset; $i<$offset+7; $i++)
    {
      if (Locking::getLockType(atkArrayNvl(getUser(), "id"),
                               $days[$i%7]['date']) == "")
      {
        $retval = $days[$i%7]['date'];
        break;
      }
    }

    return $retval;
  }



} // end class

?>
