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

/**
 * Default view in time registration
 * "week" or "day"
 */
$config['timereg_defaultview'] = "day";

/**
 * Number of lines of the 'remark' field for time entry
 * Defaults to a single line. Regardless of the setting here,
 * the actual amount of text that can be entered is unlimited.
 */
$config['timereg_remark_lines'] = 1;

/**
 * Use duration dropdown by time registration
 */
$config['use_duration'] = true;

/**
 * Max bookable hours for a time registration
 */
$config['max_bookable'] = 10;

/**
 * Resolution for time registration.
 * By default, time can be registered in increments of 15
 * minutes, which can be increased or decreased.
 * You can specify the resolution in minutes or in hours.
 * Examples: 1m, 5m, 10m, 20m, 30m, 1h, 2h etc.
 */
$config['timereg_resolution'] = "15m";

/**
 * Overtime threshold in minutes (default 10h = 600m)
 * The amount of time that a user can book on a day before it is
 * considered overtime (visualization only, true overtime is
 * calculated based on employee contracts)
 */
$config['overtimethreshold'] = 600;

/**
 * Allow locking incomplete weeks
 * This variable determines whether users may lock their time
 * registration for weeks in which they have not entered
 * all hours.
 */
$config['timereg_incompleteweeklock'] = false;

/**
 * Allow registration of time in the future. This is false by
 * default, so that only time actually spent can be registered.
 * Possible reasons to change are to allow registering vacations
 * or medical appointments in advance.
 */
$config['timereg_allowfuture'] = false;

/**
 * Confirm hours are saved
 */
$config['hours_confirm_save'] = false;

/**
 * Allow time registration on each day in a week in weekview.
 * If set to false, the user must go to the dayview of a
 * day first before they may register time.
 */
$config['timereg_week_bookable'] = true;

/**
 * Require lock week approval
 */
$config['lock_week_approval_required'] = false;

/**
 * Use startpoint for the overtime balance
 * Specify if a fake startingpoint should be used by
 * the overtime_balance node if no balance-records can be found to
 * determine a new balance record.
 *
 * Possible values:
 *    String date:   date in format YYYY-MM-DD.
 *    Boolean true:  date is set to Dec 31 in former year
 *    Boolean false: no startingpoint is used.
 */
$config['timereg_overtime_balance_use_startingpoint'] = false;

/**
 * Enable Overtime Debug
 * Enable to write log file to achievotmp/overtime.log
 */
$config['overtimebalancedebugging'] = false;

/**
 * Check weeks in the reminder cron
 * Specify how many weeks back the timereg_check cron
 * script searches for incomplete time registrations.
 */
$config['timereg_checkweeks'] = true;
?>