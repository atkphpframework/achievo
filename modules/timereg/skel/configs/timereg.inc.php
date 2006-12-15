<?

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
   * Set a date from when the hoursnotblocked cron should
   * monitor the blocked weeks (usefull if you use this
   * functionality on a existing project and you don't want
   * to block all weeks before a certain date).
   * 
   * Enter as string in the format YYYY-MM-DD
   * Leave empty to monitor all project weeks
   */
  
  $config["hoursnotblocked_from_date"] = "";
  
?>