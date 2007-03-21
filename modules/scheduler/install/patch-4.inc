<?php
  // add user preference show lunar phases
  $setup->addColumn("scheduler_userpreferences", "showlunarphases", "int(1)");
  $setup->addColumn("scheduler_userpreferences", "showtodo", "int(1)");
  $setup->addColumn("scheduler_holidays","length","int(11)");

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

?>