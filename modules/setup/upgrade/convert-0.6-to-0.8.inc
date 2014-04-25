<?php
$setup->alterColumn("hours", "date", "activitydate", "date", false);
$setup->addColumn("employee", "supervisor", "varchar(15)");
$setup->addColumn("customer", "website", "varchar(100)");
$setup->addColumn("customer", "currency", "varchar(10)");
$setup->addColumn("contact", "cellular", "varchar(20)");
$setup->addColumn("project", "fixed_price", "decimal(13,5)");
$setup->addColumn("project", "enddate", "date");
$setup->alterColumn("project", "startdate", "startdate", "date");
$setup->alterColumn("project", "status", "status", "varchar(15)", false, "active");
$setup->addColumn("project", "contact", "int(10)");

$db->query("CREATE TABLE hours_lock (
                   week varchar(6) NOT NULL,
                   userid varchar(15) NOT NULL,
                   PRIMARY KEY (week, userid)
                )");

$db->query("CREATE TABLE contract (
                   id int(10) unsigned DEFAULT '0' NOT NULL,
                   billing_period varchar(30) NOT NULL,
                   period_price decimal(13,5) DEFAULT '0.00' NOT NULL,
                   contracttype int(4) DEFAULT '0' NOT NULL,
                   customer int(10) DEFAULT '0' NOT NULL,
                   startdate date DEFAULT '0000-00-00' NOT NULL,
                   enddate date DEFAULT '0000-00-00' NOT NULL,
                   description text,
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE contracttype (
                   id int(10) unsigned DEFAULT '0' NOT NULL,
                   description varchar(100) NOT NULL,
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE usercontract (
                   id int(10) unsigned NOT NULL,
                   description text,
                   startdate date NOT NULL,
                   enddate date NOT NULL,
                   userid varchar(15) NOT NULL,
                   uc_hours int(3) DEFAULT '0' NOT NULL,
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE todo (
                   id int(10) unsigned NOT NULL,
                   projectid int(10) unsigned NOT NULL,
                   owner varchar(15) NOT NULL,
                   assigned_to varchar(15),
                   entrydate date NOT NULL,
                   duedate date NOT NULL,
                   description text,
                   title varchar(50) NOT NULL,
                   status tinyint(4) DEFAULT '0' NOT NULL,
                   priority tinyint(4) DEFAULT '0' NOT NULL, 
                   PRIMARY KEY (id)
                );");

$db->query("CREATE TABLE project_notes (
                   id int(10) unsigned DEFAULT '0' NOT NULL,
                   projectid int(10) unsigned DEFAULT '0' NOT NULL,
                   owner varchar(15) NOT NULL,
                   entrydate date DEFAULT '0000-00-00' NOT NULL,
                   description text,
                   title varchar(50) NOT NULL,
                   PRIMARY KEY (id)
                )");
?>