<?php

  $setup->installNode("calendar.schedule_attendee");
  $setup->installNode("calendar.schedule_notes");
  $setup->installNode("calendar.schedule_types");
  $setup->installNode("calendar.schedule");      
  $setup->addColumn("person", "calendar_defaultuserview", "char(1)", true);
  $setup->setVersion(2);

?>
