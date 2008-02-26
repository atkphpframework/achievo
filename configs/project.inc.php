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

?>
