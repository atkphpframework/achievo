<?
 /*  
  * @version $Revision$
  * @Author Ivo Jansch <ivo@achievo.com>
  *
  * Sends an email reminder if someone did not enter all of his hours in 
  * the previous week.
  * 
  * $Id$
  *
  */

  // work from achievo dir. .
  chdir("../");
  include_once("atk.inc");
  include_once("achievotools.inc");    
  
  $today = date("Y-m-d");
  $year = substr($today,0,4);
  $month = substr($today,5,2);
  $day = substr($today,8,2);
  
  // generate a date in the previous week (doesn't matter which, so day-7 should
  // always work.
  $prevweekstamp = mktime(0,0,0,$month,$day-7,$year);    
    
  $startdate = startOfWeek(date("Y-m-d",$prevweekstamp));
  $enddate =  endOfWeek(date("Y-m-d",$prevweekstamp));
  
  $week = strftime("%V",$prevweekstamp);
    
  // get's all users who can be a supervisor and presort them 
  $query = "SELECT
              userid, email
            FROM
              employee
            WHERE               
              status = 'active'
           ";
              
  $users = $g_db->getrows($query);
  for ($i=0;$i<count($users);$i++)
  {
  $userMail[$users[$i]["userid"]] = $users[$i]["email"];
  }
  // get all contracts.
  $query = "SELECT
              uc_hours, usercontract.userid, email, supervisor
            FROM
              usercontract, employee
            WHERE               
              startdate <= '$startdate'
              AND enddate > '$startdate'
              AND usercontract.userid = employee.userid";
              
  $contracts = $g_db->getrows($query);
  
  for ($i=0;$i<count($contracts);$i++)
  { 
    $time[$contracts[$i]["userid"]]["contract"] = $contracts[$i]["uc_hours"]*60;
    $time[$contracts[$i]["userid"]]["email"] = $contracts[$i]["email"];
    $time[$contracts[$i]["userid"]]["supervisor"] = $contracts[$i]["supervisor"];
  }
  
  // get working hours
  $query = "SELECT 
              sum(time) as time, userid
            FROM
              hours
            WHERE 
              activitydate between '$startdate' and '$enddate'
            GROUP BY userid";
  
  $hours = $g_db->getrows($query);
  $bcc   = "Bcc: ".$userMail[$userid]."\r\n";
  for ($i=0;$i<count($hours);$i++)
  {
    $time[$hours[$i]["userid"]]["time"] = $hours[$i]["time"];
  }
  
  // mail people who have entered less time than their contract
  if (is_array($time))
  {
    foreach($time as $user => $data)
    {
      if ($data["time"]<$data["contract"])
      {
        $body = stringparse(text("timeguard_mail"),array("hours"=>time_format($data["contract"]-$data["time"]),
                                                         "userid"=>$user,
                                                         "startdate"=>$startdate,
                                                         "week"=>$week,
                                                         "enddate"=>$enddate));
        $to = $data["email"];
        
        if ($userMail[$data["supervisor"]] !="")        
        {
         $cc = "Cc: ".$userMail[$data["supervisor"]]."\r\n";
        }
        if ($to!="")
        {
          usermail($to,text("timeguard_mail_subject"),$body,$cc);
          echo "sent mail to $to\n and a cc to $cc\n";
        }
        else
        {
          echo "would've sent mail to $user, but he doesn't have an email address\n";
        }
      }
    }
  }
            
?>
