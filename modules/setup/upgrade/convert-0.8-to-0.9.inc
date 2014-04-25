<?php
$db->query("CREATE TABLE schedule (
                   id int(10) NOT NULL,
                   startdate date DEFAULT '0000-00-00' NOT NULL,
                   enddate date DEFAULT '0000-00-00' NOT NULL,
                   starttime time,                   
                   endtime time,
                   allday int(1),
                   title varchar(50) NOT NULL,
                   description text,
                   scheduletype int(10) NOT NULL,
                   location varchar(50),
                   publicitem int(1),
                   owner varchar(15) NOT NULL,                   
                   PRIMARY KEY (id)   
                )");

$db->query("CREATE TABLE schedule_attendees (
                   scheduleid int(10) DEFAULT '0' NOT NULL,
                   userid varchar(15) NOT NULL,
                   PRIMARY KEY (scheduleid, userid)   
                )");

$db->query("CREATE TABLE schedule_types (
                   id int(10) NOT NULL,
                   description varchar(50) NOT NULL,
                   bgcolor varchar(7) NOT NULL,
                   fgcolor varchar(7) NOT NULL,
                   PRIMARY KEY (id)
                )");

// The currency field was not created in all situations. 
// Most notably, an upgrade from 0.6 to 0.8 did not contain the field,
// whereas a fresh 0.8 database did contain it.
if ($setup->columnExists("customer", "currency")) {
    $setup->dropColumn("customer", "currency"); // no longer used
}
?>