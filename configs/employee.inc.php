<?php
// ----------------------------------------------------------
//            EXTERNAL MODULE EMPLOYEE CONFIGURATION
// ----------------------------------------------------------
// This variable configures the moments when a mail should be send to a supervisor, indicating
// that a usercontract expires within the specified amount of time. More then one entry is allowed.
// Mode 1:  One email is send on each specified expiration value.  
// Mode 2:  Only one email is send to indicate that the closest expiration value is reached  
// 
// Example :
// $config["employee_usercontracts_expiration_values"][] = array('day'=>0,'month'=>2,'year'=>0);
// $config["employee_usercontracts_expiration_values"][] = array('day'=>0,'month'=>1,'year'=>0); 
//
// Explanation:
// When in mode 1:  
// An email will be sent to each users whos usercontract which expires in exactly one month.  
// An email will be sent to each users whos usercontract which expires in exactly two months.  
//
// When in mode 2:	
// Every time the script runs, an email is sent to a user whos usercontract expires within two months
// When a usercontract expires within 1 month (and then it logically also expires within two months) 
// the expiration value of '2 months' is ignored.    
$config["employee_usercontracts_expiration_values"][] = array('day' => 0, 'month' => 2, 'year' => 0);
$config["employee_usercontracts_expiration_values"][] = array('day' => 0, 'month' => 1, 'year' => 0);

// This variable configures the mode in which the usercontracts cron-job should work.
// Mode 1:	
// Only send an email when a usercontract expires in EXACTLY the amount of time specified in
// config["employee_usercontracts_expiration_values"]. 
//
// Mode 2:
// Send an email each time a usercontract's expiredate is within the period specified between the current
// date and the date specified in config["employee_usercontracts_expiration_values"]. 
// When a contract expires on more than one entry, only one mail is sent using the closest
// expiration date.
$config["employee_usercontracts_expiration_mode"] = 1;

// This variable configures the supervisors for usercontracts. A warning mail will be send
// to each of these mail-addresses indicating that a specified usercontract expires.  
// Example:
// $config["employee_usercontracts_expiration_supervisors"] = array("someone@somewhere.nl", "anyone@otherdomain.com");
$config["employee_usercontracts_expiration_supervisors"] = array();

// This variable configures the maximum amount of days that can exist between the expireing date
// of a contract and the extended contract, above this amount a mail will also be sent.
// (for instance if a contract ends on friday and a new contract starts on monday)
// Example:
// $config_employee_usercontracts_expiration_max_expired_extended_days = 0;
$config["employee_usercontracts_expiration_max_expired_extended_days"] = 0;

// This variable configures wether a warning mail should also be send to the user who's contract expires  
// $config_employee_usercontracts_expiration_send_warning_to_user = true;
$config["employee_usercontracts_expiration_send_warning_to_user"] = false;

// This variable configures the default amount of hours that an employee works in one week
$config["default_weekly_contract_hours"] = 40;
