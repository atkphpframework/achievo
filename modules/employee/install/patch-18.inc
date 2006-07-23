<?php

  $setup->addColumn("usercontract", "workstarttime", "time",false);
  $setup->addColumn("usercontract", "workendtime", "time",false);

  $sql = "UPDATE usercontract SET workstarttime='08:30:00', workendtime='17:00:00'";
  $db->query($sql);

  $setup->addColumn("person", "startday", "int(1)",false);

  $sql = "UPDATE person SET startday='1'";
  $db->query($sql);


?>