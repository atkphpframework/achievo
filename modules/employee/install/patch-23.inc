<?php
if ($setup->columnExists("person", "creditor_id")) {
    $setup->alterColumn("person", "creditor_id", "employer_id", "int(10)");
}
?>