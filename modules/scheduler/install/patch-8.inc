<?php
if (!$setup->columnExists("scheduler_attendees", "status")) {
    $setup->addColumn("scheduler_attendees", "status", "varchar(20)");
    $sql = "UPDATE scheduler_attendees SET status='accepted'";
    $db = &atkGetDb();
    $db->query($sql);
}
?>