<?php

$setup->installNode("pim.userpimitems");

// Add default pim items for every user so nothing changes for them
// when achievo is updated, new users will get a message where to add
// pim items.

$defaultitems = array("timereg_timeregistration","calendar_schedule","todo_todos");
$employees = $db->getRows("SELECT id FROM person WHERE role='employee'");
foreach($employees as $key => $value)
{
  $orderby=10;
  foreach($defaultitems as $pimitem)
  {
    $nextid = $db->nextid("userpimitems");
    $db->query("INSERT INTO userpimitems (id, userid, pimitem,orderby) VALUES ('$nextid','{$value['id']}','$pimitem',$orderby)");
    $orderby+=10;
  }
}
?>