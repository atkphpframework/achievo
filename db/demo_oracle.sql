ALTER SESSION SET NLS_DATE_FORMAT='YYYY-MM-DD';
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

INSERT INTO activity VALUES ( '2', 'Design', '', '0');
INSERT INTO activity VALUES ( '3', 'Documentation', '', '0');
INSERT INTO activity VALUES ( '4', 'Meeting', '', '0');
INSERT INTO activity VALUES ( '5', 'Other', '', '1');
INSERT INTO activity VALUES ( '6', 'Travel', '', '1');
INSERT INTO activity VALUES ( '7', 'Testing', '', '0');
INSERT INTO activity VALUES ( '8', 'Implementation', '', '0');

INSERT INTO customer VALUES ( 'Ibuildings.nl BV', '', '', '', '', '', '', 'info@ibuildings.nl', '', '2', '');

DROP SEQUENCE node;
DROP SEQUENCE hours;
DROP SEQUENCE project;
DROP SEQUENCE activity;
DROP SEQUENCE customer;
DROP SEQUENCE contact;
DROP SEQUENCE employee;
DROP SEQUENCE phase;
DROP SEQUENCE profile;
DROP SEQUENCE tpl_project;
DROP SEQUENCE tpl_phase;

CREATE SEQUENCE node INCREMENT BY 1 START WITH 2 NOCYCLE NOORDER;
CREATE SEQUENCE hours INCREMENT BY 1 START WITH 3 NOCYCLE NOORDER;
CREATE SEQUENCE project INCREMENT BY 1 START WITH 3 NOCYCLE NOORDER;
CREATE SEQUENCE activity INCREMENT BY 1 START WITH 9 NOCYCLE NOORDER;
CREATE SEQUENCE customer INCREMENT BY 1 START WITH 3 NOCYCLE NOORDER;
CREATE SEQUENCE contact INCREMENT BY 1 START WITH 2 NOCYCLE NOORDER;
CREATE SEQUENCE employee INCREMENT BY 1 START WITH 2 NOCYCLE NOORDER;
CREATE SEQUENCE phase INCREMENT BY 1 START WITH 4 NOCYCLE NOORDER;
CREATE SEQUENCE profile INCREMENT BY 1 START WITH 4 NOCYCLE NOORDER;
CREATE SEQUENCE tpl_project INCREMENT BY 1 START WITH 2 NOCYCLE NOORDER;
CREATE SEQUENCE tpl_phase INCREMENT BY 1 START WITH 2 NOCYCLE NOORDER;

INSERT INTO dependency VALUES ( '2', '3');

INSERT INTO employee VALUES ( 'Ivo Jansch', 'ivo', 'ivo@ibuildings.nl', '098f6bcd4621d373cade4e832627b4f6', 'active', 'blurb', '3');
INSERT INTO employee VALUES ( 'Administrator', 'administrator', 'ivo@ibuildings.nl', ' ', 'active', 'achievo', '2');

INSERT INTO hours VALUES ( '2', '2001-03-28', '2001-03-28', '2', '4', 'Introduction Meeting', '30', 'administrator');

INSERT INTO phase VALUES ( '2', 'Main', '2', 'active', ' ', '0', '0');
INSERT INTO phase VALUES ( '3', 'After Sales', '2', 'active', ' ', '0', '0');

INSERT INTO phase_activity VALUES ( '3', '4', '0');
INSERT INTO phase_activity VALUES ( '3', '5', '0');
INSERT INTO phase_activity VALUES ( '3', '6', '0');
INSERT INTO phase_activity VALUES ( '2', '2', '0');
INSERT INTO phase_activity VALUES ( '2', '3', '0');
INSERT INTO phase_activity VALUES ( '2', '8', '0');
INSERT INTO phase_activity VALUES ( '2', '4', '0');
INSERT INTO phase_activity VALUES ( '2', '5', '0');
INSERT INTO phase_activity VALUES ( '2', '7', '0');

INSERT INTO profile VALUES ( '2', 'Project Managers');
INSERT INTO profile VALUES ( '3', 'Users');

INSERT INTO project VALUES ( '2', 'Test Project', 'ivo', 'active', '', '2001-03-28', '2');

