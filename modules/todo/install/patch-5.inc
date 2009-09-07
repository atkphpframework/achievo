<?php
// In some old systems the owner field is still a varchar with some login names instead of an id
$sql = "SELECT id,userid FROM person WHERE role='employee'";
$rows = $db->getrows($sql);
$emp_lookup = array();
foreach($rows as $emp)
{
  $emp_lookup[$emp['userid']] = $emp['id'];
}

$db2 = clone($db);
foreach(array('todo','todo_history') as $table)
{
  $sql = "SELECT id,owner FROM $table WHERE SUBSTRING(LOWER(owner),1,1) NOT BETWEEN '0' AND '9'";
  $db->query($sql);
  while ($db->next_record())
  {
     $rec = $db->m_record;
     if(array_key_exists($rec['owner'],$emp_lookup))
     {
       $ownerid = $emp_lookup[$rec['owner']];
     }
     else
     {
       $ownerid = 0;
       atkwarning('User ('.$rec['owner'].') not found in person table for converting to id in todo-patch5');
     }
     $sql = "UPDATE $table SET owner = '".$ownerid."' WHERE id=".$rec['id'];
     $db2->query($sql);
  }
  $setup->alterColumn($table,"owner","owner","int(10)");
}

$setup->createIndex('todo','owner',array('fields'=>array('owner'=>array('sorting' => 'ascending',
                                                                         'length' => ''
                                                                         ))));

$setup->createIndex('todo','assigned_to',array('fields'=>array('assigned_to'=>array('sorting' => 'ascending',
                                                                         'length' => ''
                                                                         ))));

$setup->createIndex('todo','status',array('fields'=>array('status'=>array('sorting' => 'ascending',
                                                                         'length' => ''
                                                                         ))));
?>