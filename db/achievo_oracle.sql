CREATE TABLE accessright (
   node varchar2(25) NOT NULL,
   action varchar2(25) NOT NULL,
   entity number(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (node, action, entity)
);

CREATE TABLE activity (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(50) NOT NULL,
   description varchar2(50),
   remarkrequired number(1) DEFAULT '0',
   PRIMARY KEY (id)
);

CREATE TABLE bills (
   id number(10) DEFAULT '0' NOT NULL,
   projectid number(10) DEFAULT '0' NOT NULL,
   description text,
   create_date date DEFAULT '0000-00-00' NOT NULL,
   userid varchar2(15) NOT NULL,
   status varchar2(30) NOT NULL,
   specify_hours number(1) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE rate (
   id number(10) DEFAULT '0' NOT NULL,
   userid varchar2(15),
   projectid number(10),
   activityid number(10),
   customerid number(10),
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   rate number(13,5) DEFAULT '0.00000' NOT NULL,
   priority number(4) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE customer (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(100) NOT NULL,
   address varchar2(100),
   zipcode varchar2(20),
   city varchar2(100),
   country varchar2(100),
   phone varchar2(20),
   fax varchar2(20),
   email varchar2(50),
   website varchar2(100),
   bankaccount varchar2(30),
   remark text,
   PRIMARY KEY (id)
);

CREATE TABLE contact (
   id number(10) DEFAULT '0' NOT NULL,
   lastname varchar2(50),
   firstname varchar2(50),
   address varchar2(100),
   zipcode varchar2(20),
   city varchar2(100),
   country varchar2(100),
   phone varchar2(20),
   cellular varchar2(20),
   fax varchar2(20),
   email varchar2(50),
   remark text,
   company number(10) DEFAULT '0',
   owner varchar2(15),
   PRIMARY KEY (id)
);

CREATE TABLE contract (
   id number(10) DEFAULT '0' NOT NULL,
   billing_period varchar2(30) NOT NULL,
   period_price number(13,5) DEFAULT '0.00' NOT NULL,
   contracttype number(4) DEFAULT '0' NOT NULL,
   customer number(10) DEFAULT '0' NOT NULL,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE contracttype (
   id number(10) DEFAULT '0' NOT NULL,
   description varchar2(100) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE costregistration (
   id number(10) DEFAULT '0' NOT NULL,
   userid varchar2(15) NOT NULL,
   costdate date DEFAULT '0000-00-00' NOT NULL,
   projectid number(10) DEFAULT '0' NOT NULL,
   value number(13,5) DEFAULT '0.00000' NOT NULL,
   description text,
   paymethod varchar2(30) NOT NULL,
   currency varchar2(30),
   PRIMARY KEY (id)
);

CREATE TABLE currency (
   name varchar2(30) DEFAULT '0' NOT NULL,
   value number(13,5) DEFAULT '0.00000' NOT NULL,
   symbol varchar2(10) NOT NULL,
   PRIMARY KEY (symbol)
);

CREATE TABLE dependency (
   phaseid_row number(10) DEFAULT '0' NOT NULL,
   phaseid_col number(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col)
);

CREATE TABLE employee (
   userid varchar2(15) NOT NULL,
   name varchar2(40),   
   email varchar2(100),
   password varchar2(40),
   status varchar2(15) NOT NULL,
   theme varchar2(50),
   entity number(10) DEFAULT '0',
   PRIMARY KEY (userid)
);

CREATE TABLE hours (
   id number(10) DEFAULT '0' NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   activitydate date DEFAULT '0000-00-00' NOT NULL,
   phaseid number(10) DEFAULT '0' NOT NULL,
   activityid number(10) DEFAULT '0' NOT NULL,
   remark text,
   time number(4) DEFAULT '0' NOT NULL,
   userid varchar2(15) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE hours_lock (
   week varchar2(6) NOT NULL,
   userid varchar2(15) NOT NULL,
   PRIMARY KEY (week, userid)
);

CREATE TABLE phase (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(50) NOT NULL,
   projectid number(10) DEFAULT '0' NOT NULL,
   status varchar2(15) NOT NULL,
   description text,
   max_phasetime number(10) DEFAULT '0',
   max_hours number(10) DEFAULT '0',
   PRIMARY KEY (id)
);

CREATE TABLE phase_activity (
   phaseid number(10) DEFAULT '0' NOT NULL,
   activityid number(10) DEFAULT '0' NOT NULL,
   billable number(1) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

CREATE TABLE profile (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(50) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE project (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(50) NOT NULL,
   coordinator varchar2(20),
   status varchar2(15) NOT NULL,
   description text,
   startdate varchar2(20) NULL,
   customer number(10) DEFAULT '0' NULL,
   fixed_price number(13,5) DEFAULT '0.00000' NULL,
   PRIMARY KEY (id)
);

CREATE TABLE project_notes (
   id number(10) DEFAULT '0' NOT NULL,
   projectid number(10) DEFAULT '0' NOT NULL,
   owner varchar2(15) NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   title varchar2(50) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE tpl_dependency (
   phaseid_row number(10) DEFAULT '0' NOT NULL,
   phaseid_col number(10) DEFAULT '0' NOT NULL,
   projectid number(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col, projectid)
);

CREATE TABLE tpl_phase (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(50) NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE tpl_phase_activity (
   phaseid number(10) DEFAULT '0' NOT NULL,
   activityid number(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

CREATE TABLE tpl_project (
   id number(10) DEFAULT '0' NOT NULL,
   name varchar2(50) NOT NULL,
   description text,
   PRIMARY KEY (id)
);

CREATE TABLE tpl_project_phase (
   projectid number(10) DEFAULT '0' NOT NULL,
   phaseid number(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (projectid, phaseid)
);

CREATE TABLE schedule (
   id number(10) DEFAULT '0' NOT NULL,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   starttime time DEFAULT '00:00:00' NOT NULL,
   endtime time DEFAULT '00:00:00' NOT NULL,
   title varchar2(50) NOT NULL,
   description text,
   location varchar2(50) DEFAULT '0',
   allday number(1) DEFAULT '0' NOT NULL,
   publicitem number(1) DEFAULT '0' NOT NULL,
   owner varchar2(15) NOT NULL,
   showowner number(1) DEFAULT '0' NOT NULL,
   scheduletype number(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE schedule_attendees (
   scheduleid number(10) DEFAULT '0' NOT NULL,
   userid varchar2(15) NOT NULL,
   PRIMARY KEY (scheduleid, userid)
);

CREATE TABLE schedule_types (
   id number(10) DEFAULT '0' NOT NULL,
   description varchar2(50) NOT NULL,
   bgcolor varchar2(7) NOT NULL,
   fgcolor varchar2(7) NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE todo (
   id number(10) DEFAULT '0' NOT NULL,
   projectid number(10) DEFAULT '0' NOT NULL,
   owner varchar2(15) NOT NULL,
   assigned_to varchar2(100) NOT NULL,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   duedate date DEFAULT '0000-00-00' NOT NULL,
   description text,
   title varchar2(50) NOT NULL,
   status number(4) DEFAULT '0' NOT NULL,
   priority number(4) DEFAULT '0' NOT NULL,
   mailnotification number(1) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE TABLE usercontract (
   id number(10) DEFAULT '0' NOT NULL,
   description text,
   startdate date DEFAULT '0000-00-00' NOT NULL,
   enddate date DEFAULT '0000-00-00' NOT NULL,
   userid varchar2(15) NOT NULL,
   uc_hours number(3) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

CREATE SEQUENCE seq_node INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_hours INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_project INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_activity INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_customer INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_contact INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_employee INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_phase INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_profile INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_tpl_project INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_tpl_phase INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_todo INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_project_notes INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_bill INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_schedule INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_schedule_types INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_contract INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_contracttype INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_usercontract INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE seq_costregistration INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;