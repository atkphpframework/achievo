<?php
  
  $setup->alterColumn("accessright", "entity", "role_id", "int(10)", false);  
  
  $profiles = $db->getrows("SELECT id, entity FROM person WHERE role='employee' AND entity IS NOT NULL");
  foreach ($profiles as $profile)
  {
    $db->query("INSERT INTO employeerole (employee_id, role_id) VALUES (".
                                       $profile["id"].", ".$profile["entity"].")");
  }
  
?>