--  --------------------------------------------------------
-- 
--  Table structure for table 'accessright'
-- 
CREATE TABLE accessright (
node varchar2(25) NOT NULL,
action varchar2(25) NOT NULL,
entity number(11) DEFAULT '0' NOT NULL,
PRIMARY KEY (node, action, entity)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'activity'
-- 
CREATE TABLE activity (
id number(10)  DEFAULT '0' NOT NULL,
name varchar2(50) NOT NULL,
description varchar2(50),
remarkrequired number(4) DEFAULT '0',
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'customer'
-- 
CREATE TABLE customer (
name varchar2(100) NOT NULL,
address varchar2(100),
zipcode varchar2(20),
city varchar2(100),
country varchar2(100),
phone varchar2(20),
fax varchar2(20),
email varchar2(50),
bankaccount varchar2(30),
id number(11) DEFAULT '0' NOT NULL,
remark varchar2(4000),
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'contact'
-- 
CREATE TABLE contact (
lastname varchar2(50) NOT NULL,
firstname varchar2(50),
address varchar2(100),
zipcode varchar2(20),
city varchar2(100),
country varchar2(100),
phone varchar2(20),
fax varchar2(20),
email varchar2(50),
id number(11) DEFAULT '0' NOT NULL,
remark varchar2(4000),
company number(11) DEFAULT '0' NOT NULL,
owner varchar2(15),
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'dependency'
-- 
CREATE TABLE dependency (
phaseid_row number(10) DEFAULT '0' NOT NULL,
phaseid_col number(10) DEFAULT '0' NOT NULL,
PRIMARY KEY (phaseid_row, phaseid_col)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'employee'
-- 
CREATE TABLE employee (
name varchar2(40) NOT NULL,
userid varchar2(15) NOT NULL,
email varchar2(100),
password varchar2(40) NOT NULL,
status varchar2(40) DEFAULT 'active' check (status in ('active','nonactive')) NOT NULL,  
theme varchar2(50),
entity number(11) DEFAULT '0',
PRIMARY KEY (userid)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'hours'
-- 
CREATE TABLE hours (
id number(10)  DEFAULT '0' NOT NULL ,
entrydate date DEFAULT sysdate NOT NULL,
activitydate date DEFAULT sysdate NOT NULL,
phaseid number(10)  DEFAULT '0',
activityid number(10)  DEFAULT '0' NOT NULL,
remark varchar2(4000),
time number(4) DEFAULT '0' NOT NULL,
userid varchar2(15) NOT NULL,
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'phase'
-- 
CREATE TABLE phase (
id number(10)  DEFAULT '0' NOT NULL,
name varchar2(50) NOT NULL,
projectid number(10) DEFAULT '0' NOT NULL,
status varchar2(40) DEFAULT 'active' check (status in ('active','nonactive')) NOT NULL,  
description varchar2(4000) NOT NULL,
max_phasetime number(10) DEFAULT '0' NOT NULL,
max_hours number(10) DEFAULT '0' NOT NULL,
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'phase_activity'
-- 
CREATE TABLE phase_activity (
phaseid number(10)  DEFAULT '0' NOT NULL,
activityid number(10)  DEFAULT '0' NOT NULL,
billable number(4) DEFAULT '0' NOT NULL,
PRIMARY KEY (phaseid, activityid)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'profile'
-- 
CREATE TABLE profile (
id number(11) DEFAULT '0' NOT NULL,
name varchar2(50) NOT NULL,
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'project'
-- 
CREATE TABLE project (
id number(10)  DEFAULT '0' NOT NULL,
name varchar2(50) NOT NULL,
coordinator varchar2(20),
status varchar2(40) DEFAULT 'active' check (status in ('active','nonactive','archived')) NOT NULL,  
description varchar2(4000),
startdate varchar2(20) NOT NULL,
customer number(11) DEFAULT '0',
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'tpl_dependency'
-- 
CREATE TABLE tpl_dependency (
phaseid_row number(10) DEFAULT '0' NOT NULL,
phaseid_col number(10) DEFAULT '0' NOT NULL,
projectid number(10) DEFAULT '0' NOT NULL,
PRIMARY KEY (phaseid_row, phaseid_col, projectid)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'tpl_phase'
-- 
CREATE TABLE tpl_phase (
id number(10) DEFAULT '0' NOT NULL,
name varchar2(50) NOT NULL,
description varchar2(4000),
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'tpl_phase_activity'
-- 
CREATE TABLE tpl_phase_activity (
phaseid number(10)  DEFAULT '0' NOT NULL,
activityid number(10)  DEFAULT '0' NOT NULL,
PRIMARY KEY (phaseid, activityid)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'tpl_project'
-- 
CREATE TABLE tpl_project (
id number(10)  DEFAULT '0' NOT NULL,
name varchar2(50) NOT NULL,
description varchar2(4000),
PRIMARY KEY (id)
);


--  --------------------------------------------------------
-- 
--  Table structure for table 'tpl_project_phase'
-- 
CREATE TABLE tpl_project_phase (
projectid number(10) DEFAULT '0' NOT NULL,
phaseid number(10) DEFAULT '0' NOT NULL,
PRIMARY KEY (projectid, phaseid)
);


CREATE SEQUENCE node INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE hours INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE project INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE activity INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE customer INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE contact INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE employee INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE phase INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE profile INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE tpl_project INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;
CREATE SEQUENCE tpl_phase INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER;

