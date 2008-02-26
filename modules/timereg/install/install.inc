<?php

  $setup->installNode("timereg.hours");
  $setup->installNode("timereg.hours_lock");
  $setup->installNode("timereg.hours_approve");
  $setup->installNode("timereg.overtime_balance");
  $setup->installNode("timereg.workperiod");

  $setup->renameTable("hours","hoursbase");
  $setup->createView("hours","SELECT * FROM hoursbase WHERE virtual_time=0 OR virtual_time IS NULL", false);

  $setup->setVersion(15);

?>
