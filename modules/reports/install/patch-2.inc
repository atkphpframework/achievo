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
  $setup->executeSQL();
}

function addNewColumn($setup)
{
  $setup->addColumn("atk_searchcriteria","userid","INT(11)");
  $setup->addColumn("atk_searchcriteria","is_for_all","INT(1)");
}