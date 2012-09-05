<?php

/*@var $setup setup*/
$db = &atkGetDb();
if($db->tableExists("atk_searchcriteria"))
{
  addNewColumn($setup);
}
elseif($db->tableExists("atk_smartsearchcriteria"))
{
  $setup->renameTable("atk_smartsearchcriteria","atk_searchcriteria");
  addNewColumn($setup);
}
else 
{
  $setup->executeSQL("
  CREATE TABLE `atk_searchcriteria` (
  `nodetype` varchar(100) NOT NULL,
  `name` varchar(100) NOT NULL,
  `criteria` text NOT NULL,
  `handlertype` varchar(100) default NULL,
  `userid` int(11) default NULL,
  `is_for_all` smallint(1) default NULL,
  PRIMARY KEY  (`nodetype`,`name`)
)");
}

function addNewColumn($setup)
{
  $setup->addColumn("atk_searchcriteria","userid","INT(11)");
  $setup->addColumn("atk_searchcriteria","is_for_all","SMALLINT(1)");
}