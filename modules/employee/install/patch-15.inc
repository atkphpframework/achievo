<?php
// Add id field and change description into text field;
// if the id doens't exist yet
$department_lookup = NULL;

if(!$setup->columnExists("employee_department", "id"))
{
  $setup->addColumn("employee_department", "id", "int(11)");
  $setup->alterColumn("employee_department", "description", "description", "text");

  // Add id's to all department records
  $department = &atkGetNode("employee.department");
  
  $nrows = $department->selectDb();
  $department_lookup = array();
  
  for($i=0,$_i=count($nrows);$i<$_i;$i++)
  {
    $nextid = $db->nextid("employee_department");
    $sql = "UPDATE employee_department SET id = $nextid WHERE name='".escapeSQL($nrows[$i]["name"])."'";
    $db->query($sql);
    $department_lookup[$nrows[$i]["name"]]=$nextid;
  }
  // Change primary key
  $sql = "ALTER TABLE employee_department DROP PRIMARY KEY , ADD PRIMARY KEY ( id )";
  $db->query($sql);
}
 

if(!$setup->columnExists("person", "department","number"))
{
  // Check if the department lookup array is filled
  if($department_lookup==NULL)
  {
    $department = &atkGetNode("employee.department");
    
    $nrows = $department->selectDb();
    $department_lookup = array();
    for($i=0,$_i=count($nrows);$i<$_i;$i++)
    {
      $department_lookup[$nrows[$i]["name"]]=$nrows[$i]["id"];
    }
  }
  // Update all employees
  $employee = &atkGetNode("employee.employee");
  $nrows = $db->getrows("SELECT userid,department FROM person");
  $setup->alterColumn("person", "department", "department", "int(11)");
  for($i=0,$_i=count($nrows);$i<$_i;$i++)
  {
    if(isset($department_lookup[$nrows[$i]["department"]["name"]]))
    {
      $sql = "UPDATE person SET department=".$department_lookup[$nrows[$i]["department"]["name"]]." WHERE userid='".escapeSQL($nrows[$i]["userid"])."'";
      $db->query($sql);
    }
  }

}

?>