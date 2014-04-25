<?php
$setup->renameTable("document", "docmanager_document");
$setup->alterColumn("docmanager_document", "project_id", "master_id", "int(10)", false);
$setup->addColumn("docmanager_document", "master", "varchar(100)", false, "project.project");
?>