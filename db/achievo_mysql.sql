
# --------------------------------------------------------
#
# Table structure for table 'accessright'
#

CREATE TABLE accessright (
   node varchar(25) NOT NULL,
   action varchar(25) NOT NULL,
   entity int(11) DEFAULT '0' NOT NULL,
   PRIMARY KEY (node, action, entity)
);


# --------------------------------------------------------
#
# Table structure for table 'activity'
#

CREATE TABLE activity (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   description varchar(50) NOT NULL,
   remarkrequired tinyint(4) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);


# --------------------------------------------------------
#
# Table structure for table 'customer'
#

CREATE TABLE customer (
   name varchar(100) NOT NULL,
   address varchar(100) NOT NULL,
   zipcode varchar(20) NOT NULL,
   city varchar(100) NOT NULL,
   country varchar(100) NOT NULL,
   phone varchar(20) NOT NULL,
   fax varchar(20) NOT NULL,
   email varchar(50) NOT NULL,
   bankaccount varchar(30) NOT NULL,
   id int(11) DEFAULT '0' NOT NULL,
   remark text NOT NULL,
   PRIMARY KEY (id)
);


# --------------------------------------------------------
#
# Table structure for table 'contact'
#

CREATE TABLE contact (
   lastname varchar(50) NOT NULL,
   firstname varchar(50) NOT NULL,
   address varchar(100) NOT NULL,
   zipcode varchar(20) NOT NULL,
   city varchar(100) NOT NULL,
   country varchar(100) NOT NULL,
   phone varchar(20) NOT NULL,
   fax varchar(20) NOT NULL,
   email varchar(50) NOT NULL,
   id int(11) DEFAULT '0' NOT NULL,
   remark text NOT NULL,
   company int(11) DEFAULT '0' NOT NULL,
   owner varchar(15) NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'db_sequence'
#

CREATE TABLE db_sequence (
   seq_name varchar(20) NOT NULL,
   nextid int(10) unsigned DEFAULT '0' NOT NULL,
   PRIMARY KEY (seq_name)
);

INSERT INTO db_sequence (seq_name,nextid) VALUES ("node",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("hours",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("project",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("activity",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("customer",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("contact",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("employee",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("phase",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("profile",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("tpl_project",1);
INSERT INTO db_sequence (seq_name,nextid) VALUES ("tpl_phase",1);



# --------------------------------------------------------
#
# Table structure for table 'dependency'
#

CREATE TABLE dependency (
   phaseid_row int(10) DEFAULT '0' NOT NULL,
   phaseid_col int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col)
);

# --------------------------------------------------------
#
# Table structure for table 'employee'
#

CREATE TABLE employee (
   name varchar(40) NOT NULL,
   userid varchar(15) NOT NULL,
   email varchar(100) NOT NULL,
   password varchar(40) NOT NULL,
   status enum('active','nonactive') DEFAULT 'active' NOT NULL,
   theme varchar(50) NOT NULL,
   entity int(11) DEFAULT '0' NOT NULL,
   PRIMARY KEY (userid)
);

# --------------------------------------------------------
#
# Table structure for table 'hours'
#

CREATE TABLE hours (
   id int(10) unsigned DEFAULT '0' NOT NULL auto_increment,
   entrydate date DEFAULT '0000-00-00' NOT NULL,
   date date DEFAULT '0000-00-00' NOT NULL,
   phaseid int(10) unsigned DEFAULT '0' NOT NULL,
   activityid int(10) unsigned DEFAULT '0' NOT NULL,
   remark text,
   time int(4) DEFAULT '0' NOT NULL,
   userid varchar(15) NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'phase'
#

CREATE TABLE phase (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   projectid int(10) DEFAULT '0' NOT NULL,
   status enum('active','nonactive') DEFAULT 'active' NOT NULL,
   description text NOT NULL,
   max_phasetime int(10) DEFAULT '0' NOT NULL,
   max_hours int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'phase_activity'
#

CREATE TABLE phase_activity (
   phaseid int(10) unsigned DEFAULT '0' NOT NULL,
   activityid int(10) unsigned DEFAULT '0' NOT NULL,
   billable tinyint(4) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

# --------------------------------------------------------
#
# Table structure for table 'profile'
#

CREATE TABLE profile (
   id int(11) DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'project'
#

CREATE TABLE project (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   coordinator varchar(20),
   status enum('active','nonactive','archived') DEFAULT 'active' NOT NULL,
   description text NOT NULL,
   startdate varchar(20) NOT NULL,
   customer int(11) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'tpl_dependency'
#

CREATE TABLE tpl_dependency (
   phaseid_row int(10) DEFAULT '0' NOT NULL,
   phaseid_col int(10) DEFAULT '0' NOT NULL,
   projectid int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col, projectid)
);


# --------------------------------------------------------
#
# Table structure for table 'tpl_phase'
#

CREATE TABLE tpl_phase (
   id int(10) DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   description text NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'tpl_phase_activity'
#

CREATE TABLE tpl_phase_activity (
   phaseid int(10) unsigned DEFAULT '0' NOT NULL,
   activityid int(10) unsigned DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

# --------------------------------------------------------
#
# Table structure for table 'tpl_project'
#

CREATE TABLE tpl_project (
   id int(10) unsigned DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   description text NOT NULL,
   PRIMARY KEY (id)
);

# --------------------------------------------------------
#
# Table structure for table 'tpl_project_phase'
#

CREATE TABLE tpl_project_phase (
   projectid int(10) DEFAULT '0' NOT NULL,
   phaseid int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (projectid, phaseid)
);

