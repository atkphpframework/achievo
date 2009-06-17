<?php
$setup->createIndex('todo','owner',array('fields'=>array('owner'=>array('sorting' => 'ascending',                                           
                                                                         'length' => ''                                                      
                                                                         ))));
                                                                         
$setup->createIndex('todo','assigned_to',array('fields'=>array('assigned_to'=>array('sorting' => 'ascending',                                           
                                                                         'length' => ''                                                      
                                                                         ))));
                                                                         
$setup->createIndex('todo','status',array('fields'=>array('status'=>array('sorting' => 'ascending',                                           
                                                                         'length' => ''                                                      
                                                                         ))));
?>