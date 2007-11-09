<?php

  // This variable configures if the cronjob has to run in debug mode or not.
  // Possible values:
  //    debug mode: 0 -> No debugging.
  //                      Use this only mode on a live machine.
  //    debug mode: 1 -> Debugging on, databaserecords are added or updated, emails are not sent    
  //                      Can be used on a test database.
  //    debug mode: 2 -> Debugging on, databaserecords are only read, not added or updated
  //                     and email is not sent.
  //
  // If omitted, debug mode is 2.
  // Example:
  // $config["organization_debug_mode"] = 0;  

  // This variable configures if a mail has to be sent to specific users to indicate wether
  // a specific contract status and/or linked project has been set to another status.
  // if omitted, no mail is sent.
  // Example:
  // $config["organization_contracts_mailto"] = array("test@somewhere.zz");    
  
  // This variable configures if a mail should be sent to the coordinator of a changed
  // contract or project.  
  // If omitted, no mail is sent.
  // Example:
  // $config["organization_contracts_mailtocoordinator"] = true;    
  
  // This variable configures if a duplicatemail has to be sent to a substitute user in case of absence. It indicates wether
  // a specific contract status and/or linked project has been set to another status.
  // if omitted, no mail is sent.
  // Example:
  // $config["organization_contracts_duplicatemail"] = array("user@somewhere.zz" => "substituteuser@somewhere.zz");
  $config["organization_contracts_duplicatemail"] = array();

  // This variable configures to which status the cronjob contracts sets
  // the linked projects when it finds an active project that is linked to an
  // expired or ended contract.
  // If omitted it uses the default value of 'nonactive'
  // Example:
  // $config["organization_contracts_projects_newstatus"] = 'nonactive';  
  
  // This variable configures to which status the cronjob expiredcontracts sets
  // the contracts  when it finds an active contract that has expired.
  // If omitted it uses the default value of 'archived'
  // Example:
  // $config["organization_expiredcontracts_contracts_newstatus"] = 'nonactive';  
  
  // This variable configures that only records that have a column with the specified value
  // should be checked when looking for expired contracts. 
  // The first element must be the column name, the second element must be the value.
  // Beware that the column MUST exist in the database or this script will fail.
  // If omitted, no extra column is checked
  // Example:
  // $config["organization_expiredcontracts_hascolumnandvalue"] = array("has_enddate","1");  
    
  // This variable configures which ended contract types should be checked. 
  // If omitted contracts that have a status of "nonactive" or "archived" will be checked.
  // Example:
  // $config["organization_endedcontracts_status"] = array("nonactive","archived");  
  
?>
