<?php

  // Delete all non-manual overtime balance calculations
  $db->query("DELETE FROM overtime_balance WHERE (manual <> 1 OR manual IS NULL)");

?>