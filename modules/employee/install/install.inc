<?php
  $setup->installNode("employee.employee");
  $setup->installNode("employee.profile");
  
  // TODO FIXME: the fields for userprefs cannot be installed. 
  $setup->setVersion(2);
?>