ALTER TABLE project ADD abbreviation varchar(10) NOT NULL;
ALTER TABLE accessright CHANGE node node varchar(50) NOT NULL;

CREATE TABLE planning (
  id int(10) NOT NULL default '0',
  phaseid int(10) NOT NULL default '0',
  employeeid varchar(15) NOT NULL default '0',
  plandate date NOT NULL default '0000-00-00',
  time varchar(20) NOT NULL default '',
  description varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
);

CREATE TABLE employee_project (
  employeeid varchar(10) NOT NULL default '0',
  projectid int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (employeeid,projectid)
);

CREATE TABLE todo_history (
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
);

ALTER TABLE todo ADD updated datetime DEFAULT '0000-00-00 00:00' NOT NULL;