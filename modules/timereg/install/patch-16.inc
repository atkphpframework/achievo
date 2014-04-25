<?php
$setup->createIndex('hoursbase', 'activitydate', array('fields' => array('activitydate' => array('sorting' => 'ascending',
            'length' => ''
))));

$setup->createIndex('hoursbase', 'userid', array('fields' => array('userid' => array('sorting' => 'ascending',
            'length' => ''
))));

$setup->createIndex('overtime_balance', 'userid', array('fields' => array('userid' => array('sorting' => 'ascending',
            'length' => ''
))));
?>