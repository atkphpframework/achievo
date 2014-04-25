<?php
// In the default behaviour all organizations are customers
$db = &atkGetDb();
$db->query("UPDATE organization SET type='customer' WHERE type IS NULL OR type=''");
?>