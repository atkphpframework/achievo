<?
  /*
   * 
  *  version 0.5 Rene Bakx (rene@ibuildings.nl)
  *  Run this once everyweek with a cronjob or at-command
  *  Sends an employee an email with non-booked hours based on a simple formula
  *  nonworked  = (contractHours-[nonworkdays * 8] - booked) 
  * 
  * employees need to book free days, and administrators have to make sure that holidays or other non work days are entered in the system
  * All calculations are made in MINUTES!
 
  */

  include_once("atk.inc");
  atksession(); 
  atksecure();
 
  global $g_layout;
 
  function sendmail($to,$userid,$hours)
  {
   $bodytext = text(timeguard_mail);
   $bodytext = str_replace("[name]",$userid,$bodytext);
   $bodytext = str_replace("[hours]",$hours,$bodytext);
   $dbg_email = $to;
   $bodytext .= "Debugmail ".$dbg_email;
   $email = "debug@ibuildings.net";
   $from  = "From: ".text(timeguard_mail_from)."\n";
   mail($email, text(timeguard_mail_header),$bodytext, $from); 
  }
  
  $today = date("Y-m-d");
  $yr = substr($today,0,4);
  $mnth = substr($today,5,2);
  $dy = substr($today,8,2);
  
  $dateFrom = date("Y-m-d", mktime(0,0,0,$mnth,$dy-6-date("w",mktime(0,0,0,$mnth,$dy,$yr)),$yr));
  $dateTo =  date("Y-m-d", mktime(0,0,0,$mnth,$dy-date("w",mktime(0,0,0,$mnth,$dy,$yr)),$yr));
 
  // get all active users
  $q_users ="select userid,email,name from employee where (status='active') and (userid <>'administrator')";
  $r_users = $g_db->getrows($q_users);
  $c_user = count($r_users);
  
 
 // get the holidays where booking is enabled in the selected week
  $q_holiday ="select holiday_date from holidays where (holiday_work='1') 
                    AND  holiday_date >= '$dateFrom'
                    AND  holiday_date <= '$dateTo' order by holiday_date";

  $r_holiday = $g_db->getrows($q_holiday);
 // count the hours your unable to book during  the holidays in this period
 $holidayHours = (count($r_holiday)*$config_workdaylength);
 
  // loop trough every user
  for ($i=0;$i<$c_user;$i++)
    {
    // get the booked hour in this period
    $q_hours = "SELECT sum(time) from hours where (userid='".$r_users[$i]["userid"]."') 
                      AND  activitydate >= '$dateFrom'
                      AND  activitydate <= '$dateTo'";
    $r_hours = $g_db->getrows($q_hours);
    // user has not booked at all
     if ($r_hours[0][0] == 0) 
        {
        $bookedMinutes = 0;
        } 
    else
        {
        $bookedMinutes= $r_hours[0][0];
        }
    // get the users contact for this period
    $q_contract = "SELECT uc_hours from usercontract where (uc_userid='".$r_users[$i]["userid"]."') 
                      AND  uc_startdate <= '$dateFrom'
                      AND  uc_enddate >= '$dateTo'";
    $r_contract = $g_db->getrows($q_contract);
    
    // if the user has no contract, why would he book hours???
   // but we default to workweeklength anyway ;-)
    if (count($r_contract) == 0) 
      {
        $contractHours = $config_workweeklength;
      }
    else
      {
        $contractHours = $r_contract[0][0];
      }

    $weekMinutes = $config_workweeklength * 60;
    $contractMinutes = $contractHours * 60; 
    $holidayMinutes = $holidayHours * 60;
    $availableMinutes = $weekMinutes - $holidayMinutes;
    $missingMinutes = $availableMinutes - $bookedMinutes;
    $missingHours = $missingMinutes / 60;
    if ($bookedMinutes < $contractMinutes)
    {
      $hourssplit = explode(".",$missingHours);
      // make the missing hour string slightly more understandable
      $hours = $hourssplit[0];
      if ($hourssplit[1] == 25)
        {
        $minutes = "15";
        }
      elseif ($hourssplit[1] == 50)
        {
        $minutes = "30";
        }
      elseif ($hourssplit[1] == 75)
        {
        $minutes = "45";
        }
      else
        {
        $minutes = "00";
        }
      $missedHours = $hours." ".text(hours)." ".$minutes." ".text(minutes);
      sendmail($r_users[$i]["email"],$r_users[$i]["name"],$missedHours);
     }
   }


?>

