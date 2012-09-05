<?php
	
	$setup->addColumn("contract", "contract_date","date", 					true);
	$setup->addColumn("contract", "extrefnum", 		"varchar(20)", 		true);
	$setup->addColumn("contract", "price", 				"decimal(13,2)", 	true);
	$setup->addColumn("contract", "pricepayed", 	"tinyint(1)", 		true);
	$setup->addColumn("contract", "editedby", 		"int(10)", 				true);
	$setup->addColumn("contract", "comment", 			"text", 					true);
				
?>