<?php
$setup->report("Start conversion of 0.4 -> 0.6", "info");

$setup->dropTable("accessright");
$db->query("CREATE TABLE accessright (
                   node varchar(25) NOT NULL,
                   action varchar(25) NOT NULL,
                   entity int(11) DEFAULT '0' NOT NULL,
                   PRIMARY KEY (node, action, entity)
                )");

$setup->report("Due to changes in authorization handling, user rights could not be preserved
                  between Achievo 0.4 and 0.6. Please review the security profiles after conversion.", "important");

$db->query("CREATE TABLE customer (
                   id int(10) NOT NULL,
                   name varchar(100) NOT NULL,
                   address varchar(100),
                   zipcode varchar(20),
                   city varchar(100),
                   country varchar(100),
                   phone varchar(20),
                   fax varchar(20),
                   email varchar(50),
                   bankaccount varchar(30),                   
                   remark text,
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE contact (
                   id int(10) NOT NULL,
                   lastname varchar(50),
                   firstname varchar(50),
                   address varchar(100),
                   zipcode varchar(20),
                   city varchar(100),
                   country varchar(100),
                   phone varchar(20),
                   fax varchar(20),
                   email varchar(50),                   
                   remark text,
                   company int(10),
                   owner varchar(15),
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE db_sequence (
                  seq_name varchar(20) NOT NULL,
                  nextid int(10) unsigned DEFAULT '0' NOT NULL,
                  PRIMARY KEY (seq_name)
                )");

$db->query("CREATE TABLE dependency (
                  phaseid_row int(10) DEFAULT '0' NOT NULL,
                  phaseid_col int(10) DEFAULT '0' NOT NULL,
                  PRIMARY KEY (phaseid_row, phaseid_col)
                )");

$setup->alterColumn("employee", "password", "password", "varchar(40)");
$setup->dropColumn("employee", "ip");
$setup->dropColumn("employee", "prefs_bgcolor");
$setup->dropColumn("employee", "prefs_fontcolor");
$setup->dropColumn("employee", "prefs_tableheadercolor");
$setup->dropColumn("employee", "prefs_fontsize");
$setup->dropColumn("employee", "prefs_titlefontcolor");
$setup->addColumn("employee", "theme", "varchar(50)");
$setup->addColumn("employee", "entity", "int(10)");

$db->query("ALTER TABLE employee DROP PRIMARY KEY, ADD PRIMARY KEY(userid)");

$db->query("CREATE TABLE profile (
                   id int(10) NOT NULL,
                   name varchar(50) NOT NULL,
                   PRIMARY KEY (id)
                )");

$db->query("INSERT INTO profile (id,name) VALUES (" . $db->nextid("profile") . ",'Default group')");

$setup->report("Due to the way password handling changed between 0.4 and 0.6, 
                  all user passwords have been reset to 'default'. 
                  Users should change their password after they first log in.", "important");

$db->query("UPDATE employee SET password='" . md5('default') . "', theme='outlook', entity=1");

$setup->addColumn("project", "startdate", "date");
$setup->addColumn("project", "customer", "int(10)");

$db->query("UPDATE project SET startdate='" . date("Y-m-d") . "'");

$db->query("CREATE TABLE phase (
                   id int(10) unsigned NOT NULL,
                   name varchar(50) NOT NULL,
                   projectid int(10) NOT NULL,
                   status varchar(15) DEFAULT 'active' NOT NULL,
                   description text,
                   max_phasetime int(10),
                   max_hours int(10),
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE phase_activity (
                   phaseid int(10) unsigned NOT NULL,
                   activityid int(10) unsigned NOT NULL,
                   billable tinyint(4) DEFAULT '0' NOT NULL,
                   PRIMARY KEY (phaseid, activityid)
                )");

// For each project, create a main phase
$projects = $db->getrows("SELECT id, status FROM project");
$phaseidlookup = array();

for ($i = 0, $_i = count($projects); $i < $_i; $i++) {
    $id = $db->nextid("phase");

    $db->query("INSERT INTO phase (id,name,projectid,status,description,max_phasetime,max_hours)
                          VALUES ('$id','Main','" . $projects[$i]["id"] . "','" . $projects[$i]["status"] . "','Default phase',
                          0,0)");
    $phaseidlookup[$projects[$i]["id"]] = $id;
}

//Convert the projectactivitys to phase activities
$projact = $db->getrows("SELECT * FROM projectactivity");

for ($i = 0, $_i = count($projact); $i < $_i; $i++) {
    $sql = "INSERT INTO phase_activity (phaseid,activityid)
            VALUES ('" . $phaseidlookup[$projact[$i]["projectid"]] . "','" . $projact[$i]["activityid"] . "')";
    $db->query($sql);
}

$setup->dropTable("projectactivity");

$setup->addColumn("hours", "time", "int(10)", false, 0);

$db->query("UPDATE hours SET time = (hours*60)+minutes");

$setup->dropColumn("hours", "hours");
$setup->dropColumn("hours", "minutes");

$setup->alterColumn("hours", "remark", "remark", "text");

$setup->addColumn("hours", "phaseid", "int(10)", false);

foreach ($phaseidlookup as $proj => $phase) {
    $db->query("UPDATE hours SET phaseid = '$phase' WHERE projectid='$proj'");
}
$setup->dropColumn("hours", "projectid");

$db->query("CREATE TABLE tpl_dependency (
                   phaseid_row int(10) NOT NULL,
                   phaseid_col int(10) NOT NULL,
                   projectid int(10) NOT NULL,
                   PRIMARY KEY (phaseid_row, phaseid_col, projectid)
                )");

$db->query("CREATE TABLE tpl_phase (
                   id int(10) NOT NULL,
                   name varchar(50) NOT NULL,
                   description text,
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE tpl_phase_activity (
                   phaseid int(10) unsigned NOT NULL,
                   activityid int(10) unsigned NOT NULL,
                   PRIMARY KEY (phaseid, activityid)
                )");

$db->query("CREATE TABLE tpl_project (
                   id int(10) unsigned NOT NULL,
                   name varchar(50) NOT NULL,
                   description text,
                   PRIMARY KEY (id)
                )");

$db->query("CREATE TABLE tpl_project_phase (
                   projectid int(10) NOT NULL,
                   phaseid int(10) NOT NULL,
                   PRIMARY KEY (projectid, phaseid)
                )");

//Convert the templates, 
$tpls = $db->getrows("SELECT * FROM template");

$tplphaselookup = array();

for ($i = 0, $_i = count($tpls); $i < $_i; $i++) {
    $pid = $db->nextid("tpl_project");
    $db->query("INSERT INTO tpl_project (id,name,description)
                         VALUES ('$pid','" . addslashes($tpls[$i]["name"]) . "','')");

    $phid = $db->nextid("tpl_phase");
    $db->query("INSERT INTO tpl_phase (id,name,description)
           VALUES ('$phid','Main phase for " . addslashes($tpls[$i]["name"]) . "','Default phase')");

    $db->query("INSERT INTO tpl_project_phase (projectid,phaseid)
                         VALUES ('$pid','$phid')");
    $tplphaselookup[$tpls[$i]["id"]] = $phid;
}

//Convert the templates, create for every template a main phase
$tplact = $db->getrows("SELECT * FROM templateactivity");
for ($i = 0, $_i = count($tplact); $i < $_i; $i++) {
    $db->query("INSERT INTO tpl_phase_activity (phaseid,activityid)
                      VALUES ('" . $tplphaselookup[$tplact[$i]["templateid"]] . "','" . $tplact[$i]["activityid"] . "')");
}

// Remove the old tables.
$setup->dropTable("template");
$setup->dropTable("templateactivity");

function setSequence($seq, $value)
{
    $db = &atkGetDb();
    $rows = $db->getrows("SELECT * FROM db_sequence WHERE seq_name='$seq'");
    if (count($rows)) {
        $db->query("UPDATE db_sequence SET nextid = $value WHERE seq_name='$seq'");
    } else {
        $db->query("INSERT INTO db_sequence (seq_name, nextid) VALUES ('$seq', '$value')");
    }
}

function calcSequence($table, $field)
{
    $db = &atkGetDb();
    $recs = $db->getrows("SELECT max($field) as curid FROM $table");
    if (count($recs)) {
        setSequence($table, ($recs[0]['curid'] + 1));
    } else {
        setSequence($table, 1);
    }
}

calcSequence("project", "id");
calcSequence("activity", "id");
calcSequence("hours", "id");
?>