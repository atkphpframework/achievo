ALTER TABLE project ADD abbreviation VARCHAR(10) NOT NULL;
ALTER TABLE accessright CHANGE node node VARCHAR(50) NOT NULL;

CREATE TABLE planning (
  id int(10) NOT NULL default '0',
  phaseid int(10) NOT NULL default '0',
  employeeid varchar(15) NOT NULL default '0',
  date date NOT NULL default '0000-00-00',
  time varchar(20) NOT NULL default '',
  description varchar(250) NOT NULL default '',
  PRIMARY KEY  (id)
);

CREATE TABLE employee_project (
  employeeid varchar(10) NOT NULL default '0',
  projectid int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (employeeid,projectid)
);