<?php
$setup->installNode("pim.userpimitems");

// Add default pim items for every user so nothing changes for them
// when achievo is updated, new users will get a message where to add
// pim items.

$pimitems = &atkGetNode("pim.userpimitems");
$employees = $db->getRows("SELECT id FROM person WHERE role='employee'");
foreach ($employees as $key => $value) {
    $pimitems->addDefaultPimItemsForUser($value['id']);
}
?>