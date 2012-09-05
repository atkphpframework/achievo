<?php

  $setup->addColumn("organization", "invoice_address", "varchar(100)", true, "");
  $setup->addColumn("organization", "invoice_address2", "varchar(100)", true, "");
  $setup->addColumn("organization", "invoice_zipcode", "varchar(20)", true, "");
  $setup->addColumn("organization", "invoice_city", "varchar(100)", true, "");
  $setup->addColumn("organization", "invoice_state", "varchar(100)", true, "");
  $setup->addColumn("organization", "invoice_country", "varchar(100)", true, "");
    
  $db->query("UPDATE organization SET invoice_address=address, 
                                      invoice_address2=address2,
                                      invoice_zipcode=zipcode, 
                                      invoice_city=city,
                                      invoice_state=state,
                                      invoice_country=country");
    
?>
