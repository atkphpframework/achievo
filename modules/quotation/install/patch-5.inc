<?php
	$setup->installNode("quotation.payment");
	$setup->addColumn("quotation_quotation", "payment_id", "int(10)", true, "0");
?>