<?php

  $setup->installNode("scheduler.scheduler_group");
  $setup->installNode("scheduler.scheduler_group_member");
  $setup->alterColumn("scheduler_userpreferences", "default_category", "default_category", "int(10)",true);

?>