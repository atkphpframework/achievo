<?php

  // This variable configures wether hour registrations should be automatically deleted when
  // a phase is deleted. If omitted, false is default.
  $config["project_cascading_delete_hours"] = false;

  // Set a prefix for your initial projectcodes
  // Example: $config["projectcode_prefix"] = "[startdate.year][startdate.month]";
  $config["projectcode_prefix"] = "";

  // If you want an automatic incrementing number to be added to the projectcodes, set this to true
  // Example: $config["projectcode_autonumber"] = true;
  $config["projectcode_autonumber"] = false;

  // Give the automatic incrementing a specific number of digits (the number will be prefixed by zero's)
  // Example: $config["projectcode_autonumberdigits"] = 3;
  $config["projectcode_autonumberdigits"] = 1;

  // If you have a custom module that implements projectcodes, configure it's name here
  // Example: $config["projectcode_module"] = "mymodule";
  $config["projectcode_module"] = "";
  
  // Use autocomplete field instead of a project dropdown
  $config['project_selection_autocomplete']=false;
  
  // The number of recents projects we show in the dropdown
  $config['numberofrecentprojects']=10;
  
  // When contacts needs to be obligatory, set this option to true
  $config['project_contact_obligatory']=false;
  
  // The following configs are for copying a skel project directory
  // to a new location.
  
  // Project skel directory
  $config['project_dir_skel']='';
  
  // Project destination directory
  $config['project_dir_destination']='';

  // Project directory template, here you can use attributes from the
  // project node. 
  // Example: $config['project_dir_name_template']='[id]_[abbreviation]';
  $config['project_dir_name_template']='';
  
  // Project mail format, valid values are 'html' and 'htmlplain'
  $config['project_formatmail']='html';
  
  // Send of the project dir creation mail
  $config['mail_sender']='';
  
  // Receiver of the project dir creation mail
  $config['project_sendto']='';
  

?>
