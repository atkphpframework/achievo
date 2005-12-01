<?php

	if(!$setup->columnExists("person", "department"))
	{
		$setup->addColumn("person", "department", "int(10)");  
	}

?>
