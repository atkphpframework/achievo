<?php

$setup->addColumn("contract", "contractname","varchar(200)",false);
// Update contract name with first 199 chars of the description
$sql = "UPDATE contract SET contractname=SUBSTRING(description,1,199)";
$db->query($sql);
?>