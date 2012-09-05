<?php

	if(!$setup->columnExists("person", "department"))
	{
		$setup->addColumn("person", "department", "varchar(100)");  
	}

?>
