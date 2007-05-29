<?php

$setup->alterColumn("scheduler_attendees", "schedule_id", "scheduler_id", "int(10)");
$setup->alterColumn("scheduler_notes", "schedule_id", "scheduler_id", "int(10)");

?>