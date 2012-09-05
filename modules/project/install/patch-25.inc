<?php
$setup->createIndex('project','name',array('fields'=>array('name'=>array('sorting' => 'ascending',                                           
                                                                         'length' => 25                                                      
                                                                         ))));
                                                                         
$setup->createIndex('phase','project_id',array('fields'=>array('projectid'=>array('sorting' => 'ascending',                                           
                                                                         'length' => ''                                                      
                                                                         ))));
                                                                         
?>