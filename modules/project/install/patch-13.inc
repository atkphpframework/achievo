<?php
$setup->alterColumn("project", "enddate", "enddate", "date", true);
$setup->alterColumn("project", "startdate", "startdate", "date", true);

$db->query("UPDATE project SET startdate = NULL WHERE startdate = '0000-00-00'");
$db->query("UPDATE project SET enddate = NULL WHERE enddate = '0000-00-00'");
?>
