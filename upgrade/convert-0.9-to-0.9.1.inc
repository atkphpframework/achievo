<?php
  atkdebug("In convert-0.9-to-0.9.1.inc");
  
  $setup->addColumn("project", "abbreviation", "varchar(10)");  
  
  $setup->alterColumn("accessright", "node", "node", "varchar(50)", false);
  $setup->addColumn("todo", "updated", "datetime");

  $g_db->query("CREATE TABLE employee_project (
                   employeeid varchar(10) NOT NULL default '0',
                   projectid int(10) unsigned NOT NULL default '0',
                   PRIMARY KEY  (employeeid,projectid)
                )");

  $g_db->query("CREATE TABLE todo_history (
                   id int(10) unsigned DEFAULT '0' NOT NULL,
                   todoid int(10) unsigned DEFAULT '0' NOT NULL,
                   updated datetime DEFAULT '0000-00-00 00:00' NOT NULL,
                   projectid int(10) unsigned DEFAULT '0' NOT NULL,
                   owner varchar(15) NOT NULL,
                   assigned_to varchar(100) NOT NULL,
                   entrydate date DEFAULT '0000-00-00' NOT NULL,
                   duedate date DEFAULT '0000-00-00' NOT NULL,
                   description text,
                   title varchar(50) NOT NULL,
                   status tinyint(4) DEFAULT '0' NOT NULL,
                   priority tinyint(4) DEFAULT '0' NOT NULL,
                   PRIMARY KEY (id)
                )");

?>