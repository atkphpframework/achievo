<?php
$setup->installNode("project.activity");
$setup->installNode("project.phase");
$setup->installNode("project.phase_activity");
$setup->installNode("project.phaseplanning");
$setup->installNode("project.project");
$setup->installNode("project.project_personcontact");
$setup->installNode("project.project_personemployee");
$setup->installNode("project.role");
$setup->installNode("project.dependency");
$setup->installNode("project.tpl_phase");
$setup->installNode("project.tpl_phase_activity");
$setup->installNode("project.tpl_project");
$setup->installNode("project.tpl_project_phase");
$setup->installNode("project.tpl_dependency");
$setup->installNode("project.deliverable");
$setup->installNode("project.mastergantt_colorconfig");
$setup->installNode("project.project_category");

$setup->createIndex('project', 'name', array('fields' => array('name' => array('sorting' => 'ascending',
            'length' => 25
))));

$setup->createIndex('phase', 'project_id', array('fields' => array('projectid' => array('sorting' => 'ascending',
            'length' => ''
))));

$setup->setVersion(25);
?>
