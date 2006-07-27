<?php


// Convert scheduler types
$sql = "SELECT * FROM schedule_type";
$types = $db->getrows($sql);

$type_lookup = array();

for($i=0,$_i=count($types);$i<$_i;$i++)
{
  $next_id = $db->nextid("scheduler_category");
  $sql = "INSERT INTO scheduler_category (id,description,bgcolor,fgcolor)
          VALUES ($next_id,
                  '".$db->escapeSQL($types[$i]["description"])."',
                  '".$db->escapeSQL($types[$i]["bgcolor"])."',
                  '".$db->escapeSQL($types[$i]["fgcolor"])."')";
  $db->query($sql);
  $type_lookup[$types[$i]["id"]]=$next_id;
}
unset($types);

// Convert scheduler events
$sql = "SELECT * FROM schedule";
$nrows = $db->getrows($sql);

$scheduler_lookup = array();

for($i=0,$_i=count($nrows);$i<$_i;$i++)
{
  $rec = $nrows[$i];
  $next_id = $db->nextid("scheduler_scheduler");

  $sql = "INSERT INTO scheduler_scheduler (id,title,location,description,recur,startdate,enddate,starttime,endtime,allday,private,owner,all_users,category)
          VALUES ($next_id,
                  '".$db->escapeSQL($rec["title"])."',
                  '".$db->escapeSQL($rec["location"])."',
                  '".$db->escapeSQL($rec["description"])."',
                  'once',
                  '".$db->escapeSQL($rec["startdate"])."',
                  '".$db->escapeSQL($rec["enddate"])."',
                  '".$db->escapeSQL($rec["starttime"])."',
                  '".$db->escapeSQL($rec["endtime"])."',
                  '".$db->escapeSQL($rec["allday"])."',
                  '".$db->escapeSQL(($rec["publicitem"]?0:1))."',
                  '".$db->escapeSQL($rec["owner"])."',
                  '".$db->escapeSQL($rec["all_users"])."',
                  '".$db->escapeSQL($type_lookup[$rec["scheduletype"]])."')";

  $db->query($sql);
  $scheduler_lookup[$rec["id"]]=$next_id;
}

// Convert scheduler notes
$sql = "SELECT * FROM schedule_note";
$notes = $db->getrows($sql);

for($i=0,$_i=count($notes);$i<$_i;$i++)
{
  $next_id = $db->nextid("scheduler_notes");
  $sql = "INSERT INTO scheduler_notes (id,owner,schedule_id,entrydate,title,description)
          VALUES ($next_id,
                  '".$db->escapeSQL($notes[$i]["owner"])."',
                  '".$db->escapeSQL($scheduler_lookup[$notes[$i]["schedule_id"]])."',
                  '".$db->escapeSQL($notes[$i]["entrydate"])."',
                  '".$db->escapeSQL($notes[$i]["title"])."',
                  '".$db->escapeSQL($notes[$i]["description"])."')";
  $db->query($sql);
}
unset($notes);

// Convert scheduler attendees
$sql = "SELECT * FROM schedule_attendee";
$attendees= $db->getrows($sql);

for($i=0,$_i=count($attendees);$i<$_i;$i++)
{
  $sql = "INSERT INTO scheduler_attendees (schedule_id,person_id)
          VALUES ('".$db->escapeSQL($scheduler_lookup[$attendees[$i]["schedule_id"]])."',
                  '".$db->escapeSQL($attendees[$i]["person_id"])."')";
  $db->query($sql);
}
unset($attendees);

unset($type_lookup);
unset($scheduler_lookup);

?>