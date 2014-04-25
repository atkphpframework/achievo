<?php
if (!$setup->columnExists("deliverable", "emailstatus")) {
    $setup->addColumn("deliverable", "emailstatus", "int(10)", true, "0");
}
?>