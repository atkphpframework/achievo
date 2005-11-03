<?php

  $tablenames = $setup->getTableNames();
  if (!in_array("holidays", $tablenames))
  {
    $setup->installNode("calendar.holidays");
  }

?>