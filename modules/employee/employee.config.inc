<?php
// ----------------------------------------------------------
//            EXTERNAL MODULE EMPLOYEE CONFIGURATION
// ----------------------------------------------------------
// This variable configures the moments when a mail should be send to a supervisor, indicating
// that a usercontract expires within a specified amount of time.
// When a contract expires on more than one entry, only one mail is sent using the closest
// expiration date.
// 
// Example:
// $config_employee_usercontracts_expiration_values[] = array('day'=>0,'month'=>2,'year'=>0);
// $config_employee_usercontracts_expiration_values[] = array('day'=>0,'month'=>1,'year'=>0);
//
// For a usercontract which expires in 12 days, it will send a mail to the supervisor this contract
// expires within 1 month.
//$config_employee_usercontracts_expiration_values[] = array('day'=>0,'month'=>2,'year'=>0);
//$config_employee_usercontracts_expiration_values[] = array('day'=>0,'month'=>1,'year'=>0);  
// This variable configures the supervisors for usercontracts. A warning mail will be send
// to each of these mail-addresses indicating that a specified usercontract expires.  
// Example:
// $config_employee_usercontracts_expiration_supervisors = array("somename@somedomain.nl", "anyone@otherdomain.com");
$config_employee_usercontracts_expiration_supervisors = array();

// This variable configures the maximum amount of days that can exist between the expireing date
// of a contract and the extended contract, above this amount a mail will also be sent.
// (for instance if a contract ends on friday and a new contract starts on monday)
$config_employee_usercontracts_expiration_max_expired_extended_days = 0;

// This variable configures wether a warning mail should also be send to the user who's contract expires  
$config_employee_usercontracts_expiration_send_warning_to_user = false;
?>