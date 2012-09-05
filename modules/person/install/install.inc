<?php

  $setup->installNode("person.person");
  $setup->installNode("person.title");
  
  $setup->addColumn('person', 'created_by','int(11)'); 
  $setup->addColumn('person', 'created_on','datetime'); 
  $setup->addColumn('person', 'last_modified_by','int(11)'); 
  $setup->addColumn('person', 'last_modified_on','datetime'); 
 
  $setup->setVersion(7);
?>