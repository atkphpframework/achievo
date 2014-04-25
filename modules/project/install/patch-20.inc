<?php
// Get the database
$db = &atkGetDb();

// Retrieve the roles that have project.project edit rights
$query = $db->createQuery();
$query->addTable("accessright");
$query->addField("role_id");
$query->addCondition("`node`='project.project'");
$query->addCondition("`action`='edit'");
$roles = $query->executeSelect(true);

// Grant these roles rights to use the planning and financial tab
$actions = array("tab_planning", "tab_finance");
foreach ($roles as $role) {
    // Delete the tab_planning and tab_finance rights from these roles;
    $query = $db->createQuery();
    $query->addTable("accessright");
    $query->addCondition("`node`='project.project'");
    $query->addCondition("(`action`='tab_planning' OR `action`='tab_finance')");
    $query->addCondition(sprintf("`role_id`='%d'", $role["role_id"]));
    $query->executeDelete();

    foreach ($actions as $action) {
        $query = $db->createQuery();
        $query->addTable("accessright");
        $query->addField("node", "project.project");
        $query->addField("action", $action);
        $query->addField("role_id", $role["role_id"]);
        $query->executeInsert();
    }
}

//
?>