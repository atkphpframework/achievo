<?php

  $setup->installNode("timereg.hours");
  $setup->installNode("timereg.hours_lock");
  $setup->installNode("timereg.hours_approve");
  $setup->installNode("timereg.overtime_balance");
  $setup->installNode("timereg.workperiod");

  $db->setSequenceValue("hours",1);
  $db->commit();
  $setup->renameTable("hours","hoursbase");
  $setup->createView("hours","SELECT * FROM hoursbase WHERE virtual_time=0 OR virtual_time IS NULL", false);

  // Create indexes
  $setup->createIndex('hoursbase','activitydate',array('fields'=>array('activitydate'=>array('sorting' => 'ascending',                                           
                                                                         'length' =>''                                                      
                                                                         ))));

  $setup->createIndex('hoursbase','userid',array('fields'=>array('userid'=>array('sorting' => 'ascending',                                           
																																							'length' => ''                                                      
                                                                              ))));
                                                                              
  $setup->createIndex('overtime_balance','userid',array('fields'=>array('userid'=>array('sorting' => 'ascending',                                           
																																							'length' => ''                                                      
                                                                              ))));
                                                                              
  
  
  $setup->setVersion(16);

?>
