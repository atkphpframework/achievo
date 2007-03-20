<?php
  $setup->installNode("scheduler.scheduler_category");
  $setup->installNode("scheduler.scheduler_userpreferences");
  $setup->installNode("scheduler.scheduler_attendees");
  $setup->installNode("scheduler.scheduler_notes");
  $setup->installNode("scheduler.scheduler_cyclus");
  $setup->installNode("scheduler.scheduler_cyclus_not");
  $setup->installNode("scheduler.scheduler");
  $setup->installNode("scheduler.scheduler_holidays");
  $setup->installNode("scheduler.scheduler_userassistants");
  $setup->installNode("scheduler.scheduler_dates");
  $setup->installNode("scheduler.scheduler_alarms");
  
  // Insert the email templates we use for the scheduler
  $et = &atkGetNode('email_template.email_template');
  $templates = array();
  $templates[] = array('code'=>'cancel_event',
                       'module'=>'scheduler',
                       'title'=>'Notification messages for canceled events',
                       'subject'=>'Scheduler Event - {$action}: {$startdate} {$title}',
                       'plain_body'=>'Your {$title} event in {$location} has been canceled',
                       'html_body'=>'Your {$title} event in {$location} has been canceled',
                       'lastupdatedon'=>date("Y-m-d H:i:s"));
  $templates[] = array('code'=>'add_event',
                       'module'=>'scheduler',
                       'title'=>'Notification messages for added events',
                       'subject'=>'Scheduler Event - {$action}: {$startdate} {$title}',
                       'plain_body'=>'You have a meeting scheduled for {$startdate}',
                       'html_body'=>'You have a meeting scheduled for {$startdate}',
                       'lastupdatedon'=>date("Y-m-d H:i:s"));
  $templates[] = array('code'=>'updated_event',
                       'module'=>'scheduler',
                       'title'=>'Notification messages for modified events',
                       'subject'=>'Scheduler Event - {$action}: {$startdate} {$title}',
                       'plain_body'=>'Your meeting that had been scheduled for {$olddate} has been rescheduled to {$startdate}',
                       'html_body'=>'Your meeting that had been scheduled for {$olddate} has been rescheduled to {$startdate}',
                       'lastupdatedon'=>date("Y-m-d H:i:s"));
  $templates[] = array('code'=>'disinvited_user',
                       'module'=>'scheduler',
                       'title'=>'Notification messages for disinvited participants',
                       'subject'=>'Scheduler Event - {$action}: {$startdate} {$title}',
                       'plain_body'=>'You have been disinvited from the meeting at {$startdate}',
                       'html_body'=>'You have been disinvited from the meeting at {$startdate}',
                       'lastupdatedon'=>date("Y-m-d H:i:s"));
  $templates[] = array('code'=>'user_notification',
                       'module'=>'scheduler',
                       'title'=>'Notification messages for your responses',
                       'subject'=>'Scheduler Event - {$action}: {$startdate} {$title}',
                       'plain_body'=>'On {$date} {$fullname} {$action} your meeting request for {$startdate}',
                       'html_body'=>'On {$date} {$fullname} {$action} your meeting request for {$startdate}',
                       'lastupdatedon'=>date("Y-m-d H:i:s"));
  $templates[] = array('code'=>'event_alarm',
                       'module'=>'scheduler',
                       'title'=>'Notification messages for your alarms',
                       'subject'=>'Alarm for {$title} at {$startdate} in {$location}',
                       'plain_body'=>'Here is your requested alarm.',
                       'html_body'=>'Here is your requested alarm.',
                       'lastupdatedon'=>date("Y-m-d H:i:s"));
  
  foreach($templates as $record)
    $et->addDb($record);

  $setup->setVersion(4);
  set_time_limit(0);

  // Check if there exists a module calendar in the versioninfo
  // table. Ifso we need to convert the data
  
  $sql = "SELECT * FROM versioninfo WHERE module='calendar'";
  $nrows = $db->getrows($sql);
  
  if(count($nrows)>0)
  {
      
      
      // Convert data from calendar
      // This can take some time, set debug to -1 to speed up the process
      // with 100% because it doens't write debug anymore.
    
      set_time_limit(0);
    
      // Clone the db because we need 2 instances.
      if(version_compare(phpversion(), "5.0.0")>=0)
      {
        $db2 = clone($db);
      }
      else
      {
        $db2 = $db;
      }
    
      atkimport("module.scheduler.utils.schedulertools");
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
      $db->query($sql);
    
      while($db->next_record())
      {
        $rec = $db->m_record;
    
        $next_id = $db2->nextid("scheduler_scheduler");
    
        $sql = "INSERT INTO scheduler_scheduler (id,title,location,description,recur,startdate,enddate,starttime,endtime,allday,private,owner,all_users,category)
                VALUES ($next_id,
                        '".$db2->escapeSQL($rec["title"])."',
                        '".$db2->escapeSQL($rec["location"])."',
                        '".$db2->escapeSQL($rec["description"])."',
                        'once',
                        '".$db2->escapeSQL($rec["startdate"])."',
                        '".$db2->escapeSQL($rec["enddate"])."',
                        '".$db2->escapeSQL($rec["starttime"])."',
                        '".$db2->escapeSQL($rec["endtime"])."',
                        '".$db2->escapeSQL($rec["allday"])."',
                        '".$db2->escapeSQL(($rec["publicitem"]?0:1))."',
                        '".$db2->escapeSQL($rec["owner"])."',
                        '".$db2->escapeSQL($rec["all_users"])."',
                        '".$db2->escapeSQL($type_lookup[$rec["scheduletype"]])."')";
    
        $db2->query($sql);
        $scheduler_lookup[$rec["id"]]=$next_id;
        // Create dates records
    
        $tmp_rec = array("recur"=>"once",
                         "startdate"=>array("day"=>substr($rec["startdate"],8,2),
                                            "month"=>substr($rec["startdate"],5,2),
                                            "year"=>substr($rec["startdate"],0,4)),
                         "starttime"=>array("hours"=>substr($rec["starttime"],0,2),
                                            "minutes"=>substr($rec["starttime"],3,2),
                                            "seconds"=>0),
                         "enddate"=>array("day"=>substr($rec["enddate"],8,2),
                                            "month"=>substr($rec["enddate"],5,2),
                                            "year"=>substr($rec["enddate"],0,4)),
                         "endtime"=>array("hours"=>substr($rec["endtime"],0,2),
                                            "minutes"=>substr($rec["endtime"],3,2),
                                            "seconds"=>0)
                         );
        $dates = schedulertools::getDates($tmp_rec,$rec["startdate"],$rec["enddate"]);
        foreach($dates as $date)
        {
          $tmp_start = mktime($tmp_rec["starttime"]["hours"],$tmp_rec["starttime"]["minutes"],$tmp_rec["starttime"]["seconds"],$tmp_rec["startdate"]["month"],$tmp_rec["startdate"]["day"],$tmp_rec["startdate"]["year"]);
          $tmp_end = mktime($tmp_rec["endtime"]["hours"],$tmp_rec["endtime"]["minutes"],$tmp_rec["endtime"]["seconds"],$tmp_rec["enddate"]["month"],$tmp_rec["enddate"]["day"],$tmp_rec["enddate"]["year"]);
          $duration = $tmp_end-$tmp_start;
    
          $startdate = mktime($tmp_rec["starttime"]["hours"],$tmp_rec["starttime"]["minutes"],$tmp_rec["starttime"]["seconds"],substr($date,5,2),substr($date,8,2),substr($date,0,4));
          $enddate = $startdate+$duration;
    
    
          $sql = "INSERT INTO scheduler_dates (scheduler_id,startdate,enddate) VALUES ('$next_id','$startdate','$enddate')";
          $db2->query($sql);
        }
        $sql = "UPDATE scheduler_scheduler SET lastdate = '$enddate' WHERE id='$next_id'";
        $db2->query($sql);
    
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
      $db->query($sql);
    
      while($db->next_record())
      {
        $attendee = $db->m_record;
        $sql = "INSERT INTO scheduler_attendees (schedule_id,person_id)
                VALUES ('".$db2->escapeSQL($scheduler_lookup[$attendee["schedule_id"]])."',
                        '".$db2->escapeSQL($attendee["person_id"])."')";
        $db2->query($sql);
      }
    
      unset($type_lookup);
      unset($scheduler_lookup);
  }
?>
