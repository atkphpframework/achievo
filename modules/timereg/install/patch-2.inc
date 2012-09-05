<?php

  $setup->installNode("timereg.overtime_balance");  
  $setup->installNode("timereg.workperiod");  
  $setup->addColumn("hours", "workperiod", "int(10)", false, 0);

?>
