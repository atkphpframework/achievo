<?php
$setup->executeSQL("UPDATE person SET lng='" . atkconfig("language") . "' WHERE lng IS NULL");
?>