CREATE TABLE accessright (
   node varchar(25) NOT NULL,
   action varchar(25) NOT NULL,
   entity integer DEFAULT '0' NOT NULL,
   PRIMARY KEY (node, action, entity)
);

CREATE TABLE activity (
   id integer DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   description varchar(50),
   remarkrequired tinyint(1) DEFAULT '0',
   PRIMARY KEY (id)
);

CREATE TABLE bills (
   id integer DEFAULT '0' NOT NULL,
   projectid integer DEFAULT '0' NOT NULL,
   description text,
   create_date date DEFAULT '0000-00-00' NOT NULL,
   userid varchar(15) NOT NULL,
   status varchar(30) NOT NULL,
   specify_hours tinyint(1) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE rate (
   id integer DEFAULT '0' NOT NULL,
   userid varchar(15),
   projectid integer,
   activityid integer,
   customerid integer,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   rate decimal(13,5) DEFAULT '0.00000' NOT NULL,
   priority tinyint(4) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE customer (
   id integer DEFAULT '0' NOT NULL,
   name varchar(100) NOT NULL,
   address varchar(100),
   zipcode varchar(20),
   city varchar(100),
   country varchar(100),
   phone varchar(20),
   fax varchar(20),
   email varchar(50),
   website varchar(100),
   bankaccount varchar(30),
   remark text,
   PRIMARY KEY (id)
);

CREATE TABLE contact (
   id integer DEFAULT '0' NOT NULL,
   lastname varchar(50),
   firstname varchar(50),
   address varchar(100),
   zipcode varchar(20),
   city varchar(100),
   country varchar(100),
   phone varchar(20),
   cellular varchar(20),
   fax varchar(20),
   email varchar(50),
   remark text,
   company integer DEFAULT '0',
   owner varchar(15),
   PRIMARY KEY (id)
);

CREATE TABLE contract (
   id integer DEFAULT '0' NOT NULL,
   billing_period varchar(30) NOT NULL,
   period_price decimal(13,5) DEFAULT '0.00' NOT NULL,
   contracttype smallint DEFAULT '0' NOT NULL,
   customer integer DEFAULT '0' NOT NULL,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE contracttype (
   id integer DEFAULT '0' NOT NULL,
   description varchar(100) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE costregistration (
   id integer DEFAULT '0' NOT NULL,
   userid varchar(15) NOT NULL,
   costdate date DEFAULT '0000-00-00' NOT NULL,
   projectid integer DEFAULT '0' NOT NULL,
   value decimal(13,5) DEFAULT '0.00000' NOT NULL,
   description text,
   paymethod varchar(30) NOT NULL,
   currency varchar(30),
   PRIMARY KEY (id)
);

CREATE TABLE currency (
   name varchar(30) DEFAULT '0' NOT NULL,
   value decimal(13,5) DEFAULT '0.00000' NOT NULL,
   symbol varchar(10) NOT NULL,
   PRIMARY KEY (symbol)
);

CREATE TABLE dependency (
   phaseid_row integer DEFAULT '0' NOT NULL,
   phaseid_col integer DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col)
);

CREATE TABLE employee (
   userid varchar(15) NOT NULL,
   name varchar(40),   
   email varchar(100),
   password varchar(40),
   status varchar(15) NOT NULL,
   theme varchar(50),
   entity integer DEFAULT '0',
   PRIMARY KEY (userid)
);

CREATE TABLE hours (
   id integer DEFAULT '0' NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   activitydate date DEFAULT '0000-00-00' NOT NULL,
   phaseid integer DEFAULT '0' NOT NULL,
   activityid integer DEFAULT '0' NOT NULL,
   remark text,
   time smallint DEFAULT '0' NOT NULL,
   userid varchar(15) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE hours_lock (
   week varchar(6) NOT NULL,
   userid varchar(15) NOT NULL,
   PRIMARY KEY (week, userid)
);

CREATE TABLE phase (
   id integer DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   projectid integer DEFAULT '0' NOT NULL,
   status varchar(15) NOT NULL,
   description text,
   max_phasetime integer DEFAULT '0',
   max_hours integer DEFAULT '0',
   PRIMARY KEY (id)
);

CREATE TABLE phase_activity (
   phaseid integer DEFAULT '0' NOT NULL,
   activityid integer DEFAULT '0' NOT NULL,
   billable tinyint(1) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

CREATE TABLE profile (
   id integer DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE project (
   id integer DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   coordinator varchar(20),
   status varchar(15) NOT NULL,
   description text,
   startdate varchar(20) NULL,
   customer integer DEFAULT '0' NULL,
   fixed_price decimal(13,5) DEFAULT '0.00000' NULL,
   PRIMARY KEY (id)
);

CREATE TABLE project_notes (
   id integer DEFAULT '0' NOT NULL,
   projectid integer DEFAULT '0' NOT NULL,
   owner varchar(15) NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   title varchar(50) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE tpl_dependency (
   phaseid_row integer DEFAULT '0' NOT NULL,
   phaseid_col integer DEFAULT '0' NOT NULL,
   projectid integer DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col, projectid)
);

CREATE TABLE tpl_phase (
   id integer DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE tpl_phase_activity (
   phaseid integer DEFAULT '0' NOT NULL,
   activityid integer DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

CREATE TABLE tpl_project (
   id integer DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE tpl_project_phase (
   projectid integer DEFAULT '0' NOT NULL,
   phaseid integer DEFAULT '0' NOT NULL,
   PRIMARY KEY (projectid, phaseid)
);

CREATE TABLE schedule (
   id integer DEFAULT '0' NOT NULL,
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
   scheduletype integer DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE schedule_attendees (
   scheduleid integer DEFAULT '0' NOT NULL,
   userid varchar(15) NOT NULL,
   PRIMARY KEY (scheduleid, userid)
);

CREATE TABLE schedule_types (
   id integer DEFAULT '0' NOT NULL,
   description varchar(50) NOT NULL,
   bgcolor varchar(7) NOT NULL,
   fgcolor varchar(7) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE todo (
   id integer DEFAULT '0' NOT NULL,
   projectid integer DEFAULT '0' NOT NULL,
   owner varchar(15) NOT NULL,
   assigned_to varchar(100) NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   duedate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   title varchar(50) NOT NULL,
   status tinyint(4) DEFAULT '0' NOT NULL,
   priority tinyint(4) DEFAULT '0' NOT NULL,
   mailnotification tinyint(1) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE usercontract (
   id integer DEFAULT '0' NOT NULL,
   description text,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   userid varchar(15) NOT NULL,
   uc_hours smallint DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE SEQUENCE seq_node;
CREATE SEQUENCE seq_hours;
CREATE SEQUENCE seq_project;
CREATE SEQUENCE seq_activity;
CREATE SEQUENCE seq_customer;
CREATE SEQUENCE seq_contact;
CREATE SEQUENCE seq_employee;
CREATE SEQUENCE seq_phase;
CREATE SEQUENCE seq_profile;
CREATE SEQUENCE seq_tpl_project;
CREATE SEQUENCE seq_tpl_phase;
CREATE SEQUENCE seq_todo;
CREATE SEQUENCE seq_project_notes;
CREATE SEQUENCE seq_bill;
CREATE SEQUENCE seq_schedule;
CREATE SEQUENCE seq_schedule_types;
CREATE SEQUENCE seq_contract;
CREATE SEQUENCE seq_contracttype;
CREATE SEQUENCE seq_usercontract;
CREATE SEQUENCE seq_costregistration;