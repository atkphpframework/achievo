<?php
//Add missing columns to database in organization table

$setup->addColumn('organization', 'bankaccount2', 'varchar(30)');
$setup->addColumn('organization', 'bankaccount3', 'varchar(30)');
$setup->addColumn('organization', 'bankaccount4', 'varchar(30)');
$setup->addColumn('organization', 'member_of', 'int(11)');
$setup->addColumn('organization', 'status', 'int(11)');
$setup->addColumn('organization', 'source', 'int(11)');
$setup->addColumn('organization', 'industry', 'int(11)');
$setup->addColumn('organization', 'employees', 'int(11)');
$setup->addColumn('organization', 'revenue', 'varchar(5)');
$setup->addColumn('organization', 'rate', 'int(11)');
$setup->addColumn('organization', 'assigned_to', 'int(11)');
$setup->addColumn('organization', 'created_by', 'int(11)');
$setup->addColumn('organization', 'created_on', 'datetime');
$setup->addColumn('organization', 'last_modified_by', 'int(11)');
$setup->addColumn('organization', 'last_modified_on', 'datetime');
?>