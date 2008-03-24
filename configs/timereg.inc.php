<?php
  /**
   * When the setting below is set to 'true', only employees
   * that are projectmembers of the projects the current user
   * is projectmanager of will be displayed.
   */
  $config["hours_approve_projectteam_members_only"] = false;

  /**
   * When the setting below is set to 'true', the user will
   * only see hours to approve that are not already approved.
   */
  $config["hours_approve_only_show_not_approved"] = false;

  /**
   * When the setting below is set to 'true', the week that
   * is disaproved will automatically get unlocked.
   */
  $config["hours_approve_unlock_on_disaprove"] = false;
  
  /**
   * When the setting below is set to true, the week
   * is not automaticly approved
   */
  $config["lock_period_approval_required"] = false;

  /**
   * Decides if locking should be based on months or
   * weeks. Valid entries are 'week' and 'month'
  */
  $config["lockmode"] = "week";

  /**
   * When this setting is set to 'true', each coordinator
   * approves hours for his/her projects.
  */
  $config["approve_period_per_coordinator"] = false;

  /**
   * Setting related to approve_period_per_coordinator.
   * Will mail coordinators when hours are locked.
   *
  */
  $config["mail_coordinators_on_lock"] = true;

  /**
   * The mail address from which the mails will be
   * sent.
  */
  $config["from"] = "Dummy <dummy@dummy.com>";

  /**
   * Set a date from when the hoursnotblocked cron should
   * monitor the blocked weeks (usefull if you use this
   * functionality on a existing project and you don't want
   * to block all weeks before a certain date).
   *
   * Enter as string in the format YYYY-MM-DD
   * Leave empty to monitor all project weeks
   */

  $config["hoursnotblocked_from_date"] = "";

  /**
   * This variable configures wether hour registrations should be have option
   * to link time-registrations to specific contacts
   */
  $config["timereg_contact_link"] = false;
  
  // Default view (day or week)
  $config['timereg_defaultview']='day';
  
  // Number of lines for the timereg remark field
  $config['timereg_remark_lines']=1;
  
  // Use duration dropdown by time registration
  $config['use_duration']=true;
  
  // Max bookable hours for a time registration
  $config['max_bookable']=10;
  
  // Time resolution in minutes
  $config['timereg_resolution']="15m";
  
  // Overtime threshold
  $config['overtimethreshold']=10;
  
  // Is it possible to lock incomplete weeks ?
  $config['timereg_incompleteweeklock']=false;
  
  // Allow future time registrations
  $config['timereg_allowfuture']=false;
  
  
  // Confirm hours are saved
  $config['hours_confirm_save']=false;
  
  // Week bookable
  $config['timereg_week_bookable']=true;
  
  // Lock week approval required ?
  $config['lock_week_approval_required']=false;
  
  // Use startpoint for the overtime balance
  $config['timereg_overtime_balance_use_startingpoint']=false;

  // Turn Overtime debug on and it will write
  // a log file into achievotmp/overtime.log
  $config['overtimebalancedebugging']=false;
  
  // Check weeks in the reminder cron
  $config['timereg_checkweeks']=true;
  
?>