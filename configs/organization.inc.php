<?php

  // This variable configures to which status the cronjob expiredcontracts sets
  // the contracts  when it finds an active contract that has expired.
  // If omitted it uses the default value of 'archived'
  // Example:
  // $config["organization_expiredcontracts_contracts_newstatus"] = 'nonactive';
  
  // This variable configures to which status the cronjob expiredcontracts sets
  // the linked projects when it finds an active project that is linked to an expired contract.
  // if omitted it uses the default value of 'nonactive'
  // Example:
  // $config["organization_expiredcontracts_projects_newstatus"] = 'nonactive';
  
  // This variable configures if a mail has to be sent to specific users to indicate wether
  // that a specific contract and which linked projects have expired and have been 
  // automatically set to another status.
  // if omitted, no mail is sent.
  // Example:
  // $config["organization_expiredcontracts_mailto"] = array();
  
  // This variable configures that only records that have a column with the specified value
  // should be checked. 
  // The first element must be the column name, the second element must be the value.
  // Beware that the column MUST exist in the database.
  // If omitted, no extra column is checked
  // Example:
  // $config["organization_expiredcontracts_hascolumnandvalue"] = array("has_enddate","yes");
?>