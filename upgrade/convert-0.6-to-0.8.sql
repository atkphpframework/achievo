ALTER TABLE hours CHANGE date activitydate DATE DEFAULT '0000-00-00' not null;
ALTER TABLE employee ADD supervisor VARCHAR (15) not null;
ALTER TABLE customer ADD website VARCHAR (100) not null;
ALTER TABLE contact ADD cellular VARCHAR (20) not null;
ALTER TABLE project ADD fixed_price DECIMAL (13,5) not null;
ALTER TABLE project ADD enddate DATE DEFAULT '0000-00-00' not null;
ALTER TABLE project CHANGE startdate startdate DATE DEFAULT '0000-00-00' not null;
ALTER TABLE project ADD contact INT (10) not null;


CREATE TABLE hours_lock 
(
   week varchar(6) NOT NULL,
   userid varchar(15) NOT NULL,
   PRIMARY KEY (week, userid)
);

CREATE TABLE contract (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   billing_period varchar(30) NOT NULL,
   period_price decimal(13,5) DEFAULT '0.00' NOT NULL,
   contracttype int(4) DEFAULT '0' NOT NULL,
   customer int(10) DEFAULT '0' NOT NULL,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE contracttype (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   description varchar(100) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE usercontract (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   description text,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   userid varchar(15) NOT NULL,
   uc_hours int(3) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE todo (
   id int(10) unsigned DEFAULT '0' NOT NULL,
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

CREATE TABLE project_notes (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   projectid int(10) unsigned DEFAULT '0' NOT NULL,
   owner varchar(15) NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   title varchar(50) NOT NULL,
   PRIMARY KEY (id)
);
