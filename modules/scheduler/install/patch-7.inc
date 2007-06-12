<?php

if ($setup->columnExists("scheduler_attendees", "schedule_id") && !$setup->columnExists("scheduler_attendees", "scheduler_id"))
  $setup->alterColumn("scheduler_attendees", "schedule_id", "scheduler_id", "int(10)");
if ($setup->columnExists("scheduler_notes", "schedule_id") && !$setup->columnExists("scheduler_notes", "scheduler_id"))
  $setup->alterColumn("scheduler_notes", "schedule_id", "scheduler_id", "int(10)");

?>