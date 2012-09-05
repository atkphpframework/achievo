<?php

 // Add fields to Scheduler Scheduler
 $setup->addColumn("scheduler_scheduler", "times", "int(11)");

 // Add fields to Scheduler Cyclus
 $setup->addColumn("scheduler_cyclus", "cyclus_times", "int(11)");
 $setup->addColumn("scheduler_cyclus", "monthly_day", "int(2)");
 $setup->addColumn("scheduler_cyclus", "yearly_day", "int(2)");
 $setup->addColumn("scheduler_cyclus", "yearly_month", "int(2)");
 $setup->addColumn("scheduler_cyclus", "yearly_month2", "int(2)");
 $setup->addColumn("scheduler_cyclus", "daily_every", "int(2)");
 $setup->addColumn("scheduler_cyclus", "weekly_every", "int(2)");
 $setup->addColumn("scheduler_cyclus", "monthly_every", "int(2)");
 $setup->addColumn("scheduler_cyclus", "monthly_every2", "int(2)");
 $setup->addColumn("scheduler_cyclus", "monthly_month_time", "int(1)");
 $setup->addColumn("scheduler_cyclus", "yearly_month_time", "int(1)");
 $setup->addColumn("scheduler_cyclus", "weekly_weekday", "int(11)");
 $setup->addColumn("scheduler_cyclus", "monthly_weekday_list", "int(11)");
 $setup->addColumn("scheduler_cyclus", "yearly_weekday_list", "int(11)");
 $setup->addColumn("scheduler_cyclus", "daily_choice", "int(1)");
 $setup->addColumn("scheduler_cyclus", "monthly_choice", "int(1)");
 $setup->addColumn("scheduler_cyclus", "yearly_choice", "int(1)");
 $setup->addColumn("scheduler_cyclus", "end_choice", "int(1)");

 // Create second db connections
 if(version_compare(phpversion(), "5.0.0")>=0)
 {
   $db2 = clone($db);
 }
 else
 {
   $db2 = $db;
 }

 // Retrieve old scheduler records that need to be formatted
 $sql = "SELECT s.id,s.recur, sc.*
         FROM scheduler_scheduler s, scheduler_cyclus sc
         WHERE s.id = sc.scheduler_id
           AND s.recur !='once'
         ";
 
 
 $db->query($sql);
 while($db->next_record())
 {
   $rec = $db->m_record;
   $id = $rec['id'];
   // Convert values to new fields
   $sql = "UPDATE scheduler_cyclus SET ";
   switch($rec['recur'])
   {
     case "daily":           $sql.= "daily_every='".$rec['every']."', daily_choice=1 ";
                             break;
     case "weekly":          $sql.= "weekly_every='".$rec['every']."', weekly_weekday='".$rec["weekday"]."' ";
                             break;
     case "monthly_date":    $sql.= "monthly_day='".$rec['startday']."', monthly_every='".$rec['every']."', monthly_choice=1 ";
                             break;
     case "monthly_day":     $sql.= "monthly_month_time='".$rec['month_time']."', monthly_every2='".$rec['every']."', monthly_weekday_list='".$rec['weekday']."', monthly_choice=2 ";
                             break;
     case "yearly_date":     $sql.= "yearly_day='".$rec['startday']."', yearly_month='".$rec['startmonth']."', yearly_choice=1 ";
                             break;
     case "yearly_day":      $sql.= "yearly_month_time='".$rec['month_time']."', yearly_month2='".$rec['startmonth']."',yearly_weekday_list='".$rec['weekday']."', yearly_choice=2 ";
                             break;
   }
   if($rec['cyclus_enddate']=='') { $sql.=', end_choice=1 '; } else { $sql.=', end_choice=2 '; }  
   $sql.= "WHERE scheduler_id=$id";
   $db2->query($sql);
   
   // Update recur field
   if($rec['recur']=='monthly_date' || $rec['recur']=='monthly_day')
   {
     $sql = "UPDATE scheduler_scheduler SET recur='monthly' WHERE id='$id'";
     $db2->query($sql);
   }
   elseif($rec['recur']=='yearly_date' || $rec['recur']=='yearly_day')
   {
     $sql = "UPDATE scheduler_scheduler SET recur='yearly' WHERE id='$id'";
     $db2->query($sql);
   }
   
   // Remove old scheduler dates
   $sql = "DELETE FROM scheduler_dates WHERE scheduler_id='$id'";
   $db2->query($sql);
   // Reset last date and times 
   $sql = "UPDATE scheduler_scheduler SET lastdate = NULL, times = 0 WHERE id='$id'";
   $db2->query($sql);
 }
 
 // Remove the old columns
 $setup->dropColumn("scheduler_cyclus","startday");
 $setup->dropColumn("scheduler_cyclus","startmonth");
 $setup->dropColumn("scheduler_cyclus","endday"); 
 $setup->dropColumn("scheduler_cyclus","endmonth"); 
 $setup->dropColumn("scheduler_cyclus","every"); 
 $setup->dropColumn("scheduler_cyclus","month_time"); 
 $setup->dropColumn("scheduler_cyclus","weekday"); 
 
?>