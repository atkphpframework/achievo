<?php

  $setup->dropView("hours");
  $setup->createView("hours","SELECT * FROM hoursbase WHERE virtual_time=0 OR virtual_time IS NULL", false);

?>