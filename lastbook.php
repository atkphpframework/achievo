<?
  /*
   * calculates the day's between today and the last date a employee has booked
  *  and sends a mail if it's more as one day ago!
  *  
  * version 0.4 Rene Bakx (rene@ibuildings.nl)
 */

  include_once("atk.inc");
  atksession(); 
  atksecure();
 
  global $g_layout;
 
  function sendmail($to,$userid,$days)
  {
   $bodytext = text(timeguard_mail);
   $bodytext = str_replace("[name]",$userid,$bodytext);
   $bodytext = str_replace("[days]",$days,$bodytext);
   $dbg_email = $to;
   $bodytext .= "Debugmail ".$dbg_email;
   $email = "debug@ibuildings.net";
   $from  = "From: ".text(timeguard_mail_from)."\n";
   mail($email, text(timeguard_mail_header),$bodytext, $from); 
  }

  // Counts the number of days that have passed since the employee has last booked
  function countDays($today,$lastdate,$holidays)
  {
    // There are 86400 seconds in one day: 24 * 60 * 60
    global $config_hourweekend;
    $dayLast = date ("z", strtotime($lastdate));
    $dayNow = date ("z", strtotime($today));      
    // calculate offset for the not booked days
    $daynot = $dayNow-$dayLast;
    // more as one year ago??
    if ($daynot < 0)
    {
      // get's the nr of passed years
      $thisYear = substr($today,0,4);
      $lastYear = substr($lastdate,0,4); 
    // how many days remaining this year?
      $tempDay = $thisYear.substr($lastdate,4,10); 
      $days = 365 - date("z", strtotime($tempDay)); 
      $nrPassed = $thisYear-$lastYear;
      $daynot = ($nrPassed*365)+$days;
    }
    // last time booked is more as 0 day(s) ago?
    if ($daynot !=0)
    {
    // loop trough all possible holidays to match if they fall between the two days
     for ($i=0;$i<count($holidays);$i++)
     {
      if ( ($dayLast > $holidays[$i]) || ($dayNow < $dayNow))
       {
        $daynot--;
      }
     }
    } 
    return $daynot;
    }
  
  $today = date("Y-m-d");
  // get all active users
  $q_users ="select userid,email,name from employee where (status='active') and (userid <>'administrator')";
  $r_users = $g_db->getrows($q_users);
  $c_user = count($r_users);

 // get the holidays where booking is enabled
  $q_holiday ="select holiday_date from holidays where holiday_work='1' order by holiday_date";
  $r_holiday = $g_db->getrows($q_holiday);
 // build array with daynumbers for selected holidays.
  for ($i=0;$i<count($r_holiday);$i++)
  {
  $holidays[] = date("z",strtotime($r_holiday[$i]["holiday_date"]));
  }

  // loop trough every user
  for ($i=0;$i<$c_user;$i++)
    {
    $q_hours = "select activitydate from hours where userid='".$r_users[$i]["userid"]."' order by activitydate desc limit 1";
    $r_hours = $g_db->getrows($q_hours);
    $lastDate = $r_hours[0]["activitydate"];
    $daysAgo = countDays($today,$lastDate,$holidays);
    if ($daysAgo > 0)
      {
      sendmail($r_users[$i]["email"],$r_users[$i]["name"],$daysAgo);
      } 
    }


?>

