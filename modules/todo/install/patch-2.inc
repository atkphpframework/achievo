<?

$setup->addColumn("todo", "closedate", "date");
$setup->addColumn("todo", "completed", "int(10)");
$setup->addColumn("todo", "private", "int(1)");
$setup->addColumn("todo", "contactid", "int(11)");


$setup->addColumn("todo_history", "closedate", "date");
$setup->addColumn("todo_history", "completed", "int(10)");
$setup->addColumn("todo_history", "private", "int(1)");
$setup->addColumn("todo_history", "contactid", "int(11)");
?>