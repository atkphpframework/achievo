<?php
	$setup->installNode("quotation_payment");
	$setup->addColumn("quotation_quotation", "payment_id", "int(10)", false, "0");
?>