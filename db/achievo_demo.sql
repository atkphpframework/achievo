
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

#
# Dumping data for table 'accessright'
#

INSERT INTO accessright VALUES ( 'employee', 'stats', '2');
INSERT INTO accessright VALUES ( 'employee', 'admin', '2');
INSERT INTO accessright VALUES ( 'activity', 'stats', '2');
INSERT INTO accessright VALUES ( 'activity', 'admin', '2');
INSERT INTO accessright VALUES ( 'userprefs', 'edit', '2');
INSERT INTO accessright VALUES ( 'phase', 'delete', '2');
INSERT INTO accessright VALUES ( 'phase', 'edit', '2');
INSERT INTO accessright VALUES ( 'phase', 'add', '2');
INSERT INTO accessright VALUES ( 'project', 'stats', '2');
INSERT INTO accessright VALUES ( 'project', 'delete', '2');
INSERT INTO accessright VALUES ( 'project', 'edit', '2');
INSERT INTO accessright VALUES ( 'project', 'add', '2');
INSERT INTO accessright VALUES ( 'project', 'admin', '2');
INSERT INTO accessright VALUES ( 'customer', 'delete', '2');
INSERT INTO accessright VALUES ( 'customer', 'edit', '2');
INSERT INTO accessright VALUES ( 'customer', 'add', '2');
INSERT INTO accessright VALUES ( 'hours', 'view_all', '2');
INSERT INTO accessright VALUES ( 'customer', 'admin', '2');
INSERT INTO accessright VALUES ( 'hours', 'hoursurvey', '2');
INSERT INTO accessright VALUES ( 'hours', 'delete', '2');
INSERT INTO accessright VALUES ( 'hours', 'edit', '2');
INSERT INTO accessright VALUES ( 'hours', 'add', '2');
INSERT INTO accessright VALUES ( 'hours', 'admin', '2');
INSERT INTO accessright VALUES ( 'employee', 'view_all', '2');
INSERT INTO accessright VALUES ( 'hours', 'admin', '3');
INSERT INTO accessright VALUES ( 'hours', 'add', '3');
INSERT INTO accessright VALUES ( 'hours', 'edit', '3');
INSERT INTO accessright VALUES ( 'hours', 'delete', '3');
INSERT INTO accessright VALUES ( 'hours', 'hoursurvey', '3');
INSERT INTO accessright VALUES ( 'customer', 'admin', '3');
INSERT INTO accessright VALUES ( 'project', 'admin', '3');
INSERT INTO accessright VALUES ( 'project', 'stats', '3');
INSERT INTO accessright VALUES ( 'userprefs', 'edit', '3');
INSERT INTO accessright VALUES ( 'activity', 'admin', '3');
INSERT INTO accessright VALUES ( 'employee', 'admin', '3');
INSERT INTO accessright VALUES ( 'employee', 'stats', '3');

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

#
# Dumping data for table 'activity'
#

INSERT INTO activity VALUES ( '2', 'Design', '', '0');
INSERT INTO activity VALUES ( '3', 'Documentation', '', '0');
INSERT INTO activity VALUES ( '4', 'Meeting', '', '0');
INSERT INTO activity VALUES ( '5', 'Other', '', '1');
INSERT INTO activity VALUES ( '6', 'Travel', '', '1');
INSERT INTO activity VALUES ( '7', 'Testing', '', '0');
INSERT INTO activity VALUES ( '8', 'Implementation', '', '0');

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

#
# Dumping data for table 'customer'
#

INSERT INTO customer VALUES ( 'Ibuildings.nl BV', '', '', '', '', '', '', 'info@ibuildings.nl', '', '2', '');

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

#
# Dumping data for table 'db_sequence'
#

INSERT INTO db_sequence VALUES ( 'node', '1');
INSERT INTO db_sequence VALUES ( 'hours', '2');
INSERT INTO db_sequence VALUES ( 'project', '2');
INSERT INTO db_sequence VALUES ( 'activity', '8');
INSERT INTO db_sequence VALUES ( 'customer', '2');
INSERT INTO db_sequence VALUES ( 'contact', '1');
INSERT INTO db_sequence VALUES ( 'employee', '1');
INSERT INTO db_sequence VALUES ( 'phase', '3');
INSERT INTO db_sequence VALUES ( 'profile', '3');
INSERT INTO db_sequence VALUES ( 'tpl_project', '1');
INSERT INTO db_sequence VALUES ( 'tpl_phase', '1');

# --------------------------------------------------------
#
# Table structure for table 'dependency'
#

CREATE TABLE dependency (
   phaseid_row int(10) DEFAULT '0' NOT NULL,
   phaseid_col int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid_row, phaseid_col)
);

#
# Dumping data for table 'dependency'
#

INSERT INTO dependency VALUES ( '2', '3');

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

#
# Dumping data for table 'employee'
#

INSERT INTO employee VALUES ( 'Ivo Jansch', 'ivo', 'ivo@ibuildings.nl', '098f6bcd4621d373cade4e832627b4f6', 'active', 'blurb', '3');
INSERT INTO employee VALUES ( 'Administrator', 'administrator', 'ivo@ibuildings.nl', '', 'active', 'achievo', '2');

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

#
# Dumping data for table 'hours'
#

INSERT INTO hours VALUES ( '2', '2001-03-28', '2001-03-28', '2', '4', 'Introduction Meeting', '30', 'administrator');

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

#
# Dumping data for table 'phase'
#

INSERT INTO phase VALUES ( '2', 'Main', '2', 'active', '', '0', '0');
INSERT INTO phase VALUES ( '3', 'After Sales', '2', 'active', '', '0', '0');

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

#
# Dumping data for table 'phase_activity'
#

INSERT INTO phase_activity VALUES ( '3', '4', '0');
INSERT INTO phase_activity VALUES ( '3', '5', '0');
INSERT INTO phase_activity VALUES ( '3', '6', '0');
INSERT INTO phase_activity VALUES ( '2', '2', '0');
INSERT INTO phase_activity VALUES ( '2', '3', '0');
INSERT INTO phase_activity VALUES ( '2', '8', '0');
INSERT INTO phase_activity VALUES ( '2', '4', '0');
INSERT INTO phase_activity VALUES ( '2', '5', '0');
INSERT INTO phase_activity VALUES ( '2', '7', '0');

# --------------------------------------------------------
#
# Table structure for table 'profile'
#

CREATE TABLE profile (
   id int(11) DEFAULT '0' NOT NULL,
   name varchar(50) NOT NULL,
   PRIMARY KEY (id)
);

#
# Dumping data for table 'profile'
#

INSERT INTO profile VALUES ( '2', 'Project Managers');
INSERT INTO profile VALUES ( '3', 'Users');

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
   customer varchar(100),
   PRIMARY KEY (id)
);

#
# Dumping data for table 'project'
#

INSERT INTO project VALUES ( '2', 'Test Project', 'ivo', 'active', '', '20010328', '2');

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

#
# Dumping data for table 'tpl_dependency'
#


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

#
# Dumping data for table 'tpl_phase'
#


# --------------------------------------------------------
#
# Table structure for table 'tpl_phase_activity'
#

CREATE TABLE tpl_phase_activity (
   phaseid int(10) unsigned DEFAULT '0' NOT NULL,
   activityid int(10) unsigned DEFAULT '0' NOT NULL,
   PRIMARY KEY (phaseid, activityid)
);

#
# Dumping data for table 'tpl_phase_activity'
#


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

#
# Dumping data for table 'tpl_project'
#


# --------------------------------------------------------
#
# Table structure for table 'tpl_project_phase'
#

CREATE TABLE tpl_project_phase (
   projectid int(10) DEFAULT '0' NOT NULL,
   phaseid int(10) DEFAULT '0' NOT NULL,
   PRIMARY KEY (projectid, phaseid)
);

#
# Dumping data for table 'tpl_project_phase'
#

