ALTER TABLE hours ADD registered tinyint(1) DEFAULT '0' NOT NULL;
ALTER TABLE hours ADD onbill tinyint(1) DEFAULT '0' NOT NULL;
ALTER TABLE hours ADD bill_line_id int(10) DEFAULT '0' NOT NULL;
ALTER TABLE hours ADD hour_rate decimal(13,5) DEFAULT '0.00000' NOT NULL;

CREATE TABLE bill (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   projectid int(10) DEFAULT '0' NOT NULL,
   description text,
   create_date date DEFAULT '0000-00-00' NOT NULL,
   userid varchar(15) NOT NULL,
   status varchar(30) NOT NULL,
   specify_hours tinyint(1) DEFAULT '0' NOT NULL,
   contactperson int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE bill_line (
  id int(10) unsigned DEFAULT '0' NOT NULL,
  billid int(10) DEFAULT '0' NOT NULL,
  shortdescription varchar(100),
  description text,
  calcoption varchar(10),
  fixed_billed varchar(10),
  PRIMARY KEY (id)
);

CREATE TABLE discount (
  id int(10) NOT NULL auto_increment,
  type tinyint(1) NOT NULL default '0',
  amount decimal(13,5) NOT NULL default '0.00',
  bill_line_id int(10) NOT NULL default '0',
  apply_on varchar(10) NOT NULL default '',
  PRIMARY KEY (id)
);

CREATE TABLE rate (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   userid varchar(15),
   projectid int(10),
   activityid int(10),
   customerid int(10),
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   rate decimal(13,5) DEFAULT '0.00000' NOT NULL,
   priority tinyint(4) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE costregistration (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   userid varchar(15) NOT NULL,
   costdate date DEFAULT '0000-00-00' NOT NULL,
   projectid int(10) unsigned DEFAULT '0' NOT NULL,
   value decimal(13,5) DEFAULT '0.00000' NOT NULL,
   description text,
   paymethod varchar(30) NOT NULL,
   currency varchar(30),
   bill_line_id int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE currency (
   name varchar(30) DEFAULT '0' NOT NULL,
   value decimal(13,5) DEFAULT '0.00000' NOT NULL,
   symbol varchar(10) NOT NULL,
   PRIMARY KEY (symbol)
);

CREATE TABLE schedule (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   starttime time DEFAULT '00:00:00' NOT NULL,
   endtime time DEFAULT '00:00:00' NOT NULL,
   title varchar(50) NOT NULL,
   description text,
   location varchar(50) DEFAULT '0',
   allday tinyint(1) DEFAULT '0' NOT NULL,
   publicitem tinyint(1) DEFAULT '0' NOT NULL,
   owner varchar(15) NOT NULL,
   showowner tinyint(1) DEFAULT '0' NOT NULL,
   scheduletype int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE schedule_attendees (
   scheduleid int(10) DEFAULT '0' NOT NULL,
   userid varchar(15) NOT NULL,
   PRIMARY KEY (scheduleid, userid)
);

CREATE TABLE schedule_types (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   description varchar(50) NOT NULL,
   bgcolor varchar(7) NOT NULL,
   fgcolor varchar(7) NOT NULL,
   PRIMARY KEY (id)
);

ALTER TABLE project ADD abbreviation VARCHAR(10) NOT NULL;
ALTER TABLE accessright CHANGE node node VARCHAR(50) NOT NULL;

UPDATE accessright SET action = 'admin' WHERE node = 'project' AND action = '*admin';
