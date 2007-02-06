<?php
  /**
   * Hours listener
   *
   * Sends an e-mail to a project coordinator if hours are added for a phase
   * that hasn't started yet, or on a phase that depends on a non-complete
   * deliverable.
   *
   * @package achievo
   * @subpackage modules
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atktriggerlistener");

  /**
   * The hoursListener class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo
   * @subpackage modules
   */
  class hoursListener extends atkTriggerListener
  {

    /**
     * Searches for an incomplete deliverable on which the given phase depends
     *
     * @param integer $phaseid Phase in which we should search for an incomplete deliverable
     * @return array empty array if incomplete deliverable is not found for given phase, or deliverable record if found
     */
    function getIncompleteDeliverableForPhase($phase)
    {
      // If the phase doesn't depend on a deliverable, return empty array
      if ($phase["dependsondeliverable"] == "")
        return array();

      // Try to get the incomplete delverable form the database
      $deliverablenode = &atkGetNode("project.deliverable");
      $deliverablerecords = $deliverablenode->selectDb("deliverable.id='".($phase["dependsondeliverable"]["id"]!=""?$phase["dependsondeliverable"]["id"]:0)."' AND deliverable.status<>'complete'");

      // Return records
      return $deliverablerecords;
    }

    /**
     * Determines whether a phase is started or not
     *
     * @param array $phase Phase record for which the start should be evaluated
     * @return boolean True if the phase is started, false if not
     */
    function isPhaseStarted($phase)
    {
      // Determine the phase starttime and the current time
      $phasestart = dateutil::arr2stamp($phase["startdate"]);
      $now = time();

      // Return true if the phasestart is today or earlier
      return $phasestart <= $now;
    }

    /**
     * Converts a date array into a user-friendly string
     *
     * @param array $arrdate Array containing a day, month and year element
     * @return String converted representation of the given date array
     */
    function arr2str($arrdate)
    {
      if (!is_array($arrdate)) return "";
      return sprintf("%02d-%02d-%04d", $arrdate["day"], $arrdate["month"], $arrdate["year"]);
    }

    function postAdd(&$record)
    {
      return $this->handleTrigger($record);
    }
    
    function postUpdate(&$record)
    {
      return $this->handleTrigger($record);
    }
    
    /**
     * Notification handler function to be triggered when the action we're listening to is executed
     *
     * @param Array $record The record being updated
     */
    function handleTrigger(&$record)
    {
      // This handler should only be called when saving or updating hours
      if ($this->m_node->m_type != "hours")
      {
        atkerror("Cannot call the hoursListener when performing an action on the '" . $this->m_node->m_type . "' node");
        return false;
      }

      // Get the phase record from the database and throw an error if not found
      $phasenode = &atkGetNode("project.phase");
      $phaserecords = $phasenode->selectDb("phase.id='".$record["phaseid"]["id"]."'");      
      if (count($phaserecords) == 0)
      {
        atkerror("Cannot find phase to which the hours were registered.");
        return false;
      }

      // Get the incomplete deliverable from the database
      $deliverablerecords = $this->getIncompleteDeliverableForPhase($phaserecords[0]);

      // Determine whether an incomplete deliverable is found or not
      $deliverableincomplete = (count($deliverablerecords) != 0);

      // Is the phase not started yet?
      $phasenotstarted = !$this->isPhaseStarted($phaserecords[0]);

      // If there is a reason to send a mail (the deliverable is not complete when adding the
      if ($deliverableincomplete || $phasenotstarted)
      {

        // Get the project and throw an error if not found
        $projectnode = &atkGetNode("project.project");
        $projectrecords = $projectnode->selectDb("project.id='".$phaserecords[0]["projectid"]["id"] ."'");
        if (count($projectrecords) == 0)
        {
          atkerror("Cannot find project to which the phase belongs on which the hours were registered.");
          return false;
        }

        // Try to find the person who registered the hours and throw an error if not found
        $employeenode = &atkGetNode("employee.employee");
        $employeerecords = $employeenode->selectDb("person.id='".$projectrecords[0]["coordinator"]["id"]."'");
        if (count($employeerecords) == 0)
        {
          atkerror("Cannot find employee who registered the hours.");
          return false;
        }

        // Try to find the projectcoordinator and exit (with debug messag) if not found
        $coordinatorrecords = $employeenode->selectDb("person.id='".$projectrecords[0]["coordinator"]["id"]."'");
        if (count($coordinatorrecords) == 0)
        {
          atkdebug("Cannot find the coordinator of the project to which the hours were registered.");
          return false;
        }

        // Initialize the subject and recipient for the mail
        $subject = sprintf(atktext("problems_occured_while_registering_hours_on_phase_%s", "project"), $phaserecords[0]["name"]);
        $to = $coordinatorrecords[0]["email"];

        // Compose the body of the mail
        $translatedline = atktext("the_following_problems_occured_while_employee_%s_registered_time_on_phase_%s_of_project_%s", "project");
        $body = sprintf($translatedline, $employeenode->descriptor($employeerecords[0]), $phaserecords[0]["name"], $projectrecords[0]["name"]) . ":\r\n\r\n";

        if ($deliverableincomplete)
        {
          $body.= atktext("the_deliverable_on_which_this_phase_depense_is_not_complete_yet", "project") . ":\r\n";
          $body.= atktext("deliverable", "project") . ": " . $deliverablerecords[0]["name"] . "\r\n";
          $body.= atktext("duedate", "project") . ": " . $this->arr2str($deliverablerecords[0]["duedate"]) . "\r\n";
          $body.= "\r\n";
        }

        if ($phasenotstarted)
        {
          $body.= atktext("the_phase_is_not_started_yet", "project") . ":\r\n";
          $body.= atktext("name", "project") . ": " . $phaserecords[0]["name"] . "\r\n";
          $body.= atktext("startdate", "project") . ": " . $this->arr2str($phaserecords[0]["startdate"]) . "\r\n";
          $body.= atktext("enddate", "project") . ": " . $this->arr2str($phaserecords[0]["enddate"]) . "\r\n";
        }

        // Send the mail
        usermail($to, $subject, $body);
      }
      
      return true;
    }
  }
?>