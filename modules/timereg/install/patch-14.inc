<?php

  $setup->addColumn("hours","virtual_time","TINYINT(1)",false,0);
  $setup->renameTable("hours","hoursbase");
  $setup->createView("hours","SELECT * FROM hoursbase WHERE virtual_time=0", true);

?>