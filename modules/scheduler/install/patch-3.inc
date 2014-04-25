<?php
// Install the scheduler node in case the category column is missing.
$setup->installNode("scheduler.scheduler");

// Allow a rather long execution time
set_time_limit(0);

if (version_compare(phpversion(), "5.0.0") >= 0) {
    $db_clone = clone($db);
} else {
    $db_clone = $db;
}

// Convert scheduler types
$sql = "SELECT * FROM schedule_type";
$db->query($sql);
$type_lookup = array();
while ($db->next_record()) {
    $next_id = $db->nextid("scheduler_category");
    $insertsql = "INSERT INTO scheduler_category (id,description,bgcolor,fgcolor)
          VALUES ($next_id,
                  '" . $db->escapeSQL($db->m_record["description"]) . "',
                  '" . $db->escapeSQL($db->m_record["bgcolor"]) . "',
                  '" . $db->escapeSQL($db->m_record["fgcolor"]) . "')";
    $db_clone->query($insertsql);
    $type_lookup[$db->m_record["id"]] = $next_id;
}

// Convert scheduler events
$sql = "SELECT * FROM schedule";
$db->query($sql);
$scheduler_lookup = array();
while ($db->next_record()) {
    $rec = $db->m_record;
    $next_id = $db->nextid("scheduler_scheduler");

    $insertsql = "INSERT INTO scheduler_scheduler (id,title,location,description,recur,startdate,enddate,starttime,endtime,allday,private,owner,all_users,category)
          VALUES ($next_id,
                  '" . $db->escapeSQL($rec["title"]) . "',
                  '" . $db->escapeSQL($rec["location"]) . "',
                  '" . $db->escapeSQL($rec["description"]) . "',
                  'once',
                  '" . $db->escapeSQL($rec["startdate"]) . "',
                  '" . $db->escapeSQL($rec["enddate"]) . "',
                  '" . $db->escapeSQL($rec["starttime"]) . "',
                  '" . $db->escapeSQL($rec["endtime"]) . "',
                  '" . $db->escapeSQL($rec["allday"]) . "',
                  '" . $db->escapeSQL(($rec["publicitem"] ? 0 : 1)) . "',
                  '" . $db->escapeSQL($rec["owner"]) . "',
                  '" . $db->escapeSQL($rec["all_users"]) . "',
                  '" . $db->escapeSQL($type_lookup[$rec["scheduletype"]]) . "')";

    $db_clone->query($insertsql);
    $scheduler_lookup[$rec["id"]] = $next_id;
}

// Convert scheduler notes
$sql = "SELECT * FROM schedule_note";
$db->query($sql);
while ($db->next_record()) {
    $next_id = $db->nextid("scheduler_notes");
    $insertsql = "INSERT INTO scheduler_notes (id,owner,scheduler_id,entrydate,title,description)
          VALUES ($next_id,
                  '" . $db->escapeSQL($db->m_record["owner"]) . "',
                  '" . $db->escapeSQL($scheduler_lookup[$db->m_record["scheduler_id"]]) . "',
                  '" . $db->escapeSQL($db->m_record["entrydate"]) . "',
                  '" . $db->escapeSQL($db->m_record["title"]) . "',
                  '" . $db->escapeSQL($db->m_record["description"]) . "')";
    $db_clone->query($insertsql);
}

// Convert scheduler attendees
$sql = "SELECT * FROM schedule_attendee";
$db->query($sql);
while ($db->next_record()) {
    $insertsql = "INSERT INTO scheduler_attendees (scheduler_id,person_id)
          VALUES ('" . $db->escapeSQL($scheduler_lookup[$db->m_record["scheduler_id"]]) . "',
                  '" . $db->escapeSQL($db->m_record["person_id"]) . "')";
    $db_clone->query($insertsql);
}

unset($type_lookup);
unset($scheduler_lookup);
?>