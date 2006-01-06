<?php

  // This variable configures if a mail has to be sent to specific users to indicate wether
  // a specific contract status and/or linked project has been set to another status.
  // if omitted, no mail is sent.
  // Example:
  // $config["organization_contracts_mailto"] = array();
  
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
  // $config["organization_expiredcontracts_hascolumnandvalue"] = array("has_enddate","yes");
    
  // This variable configures which ended contract types should be checked. 
  // If omitted contracts that have a status of "nonactive" or "archived" will be checked.
  // Example:
  // $config["organization_endedcontracts_statussen"] = array("nonactive","archived");  
?>