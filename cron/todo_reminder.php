<?

 /*  
  * @version $Revision$
  * @Author Ivo Jansch <ivo@achievo.org>
  *
  * Sends an email reminder if there are any todo's that have a 
  * due date <= today. The reminder is sent to the person to whom 
  * the todo is assigned, with a cc to the owner of the todo (unless
  * ofcourse they are both the same person).
  *
  * This script should be run daily, preferably in the morning, so
  * people are notified of the todo's that should be fixed today.
  * 
  * $Id$
  *
  */
  
  chdir("../");
  include_once("atk.inc");
  include_once("achievotools.inc");    
  
  // check which todo's have a duedate of today or earlier, and which have a 
  // status of new, in_progress or on_hold.
  $todos = $g_db->getrows("SELECT 
                             todo.title,
                             todo.description,
                             todo.duedate,
                             ASSIGNED.name as assigned_name,
                             ASSIGNED.email as assigned_email,
                             OWNER.name as owner_name,
                             OWNER.email as owner_email, 
                             project.name as project_name
                           FROM  
                             todo LEFT JOIN project ON (todo.projectid = project.id),
                             employee ASSIGNED,
                             employee OWNER
                            WHERE 
                             todo.assigned_to = ASSIGNED.userid
                             AND todo.owner = OWNER.userid
                             AND todo.duedate<=NOW() 
                             AND todo.status IN (1,3,4)
                            ORDER BY
                             ASSIGNED.email,
                             todo.duedate,
                             todo.title");
  
  // We sent as little mail as possible. For each todo, there are two people
  // being mailed: the owner, and the assignee. We group todos such, that each 
  // user only gets one email.
  
  $mails = array();
  $today = date("Y-m-d");
  
  for ($i=0, $_i=count($todos); $i<$_i; $i++)
  {    
    $assignee = $todos[$i]["assigned_email"];
    $owner = $todos[$i]["owner_email"];
    
    $lookup[$owner] = $todos[$i]["owner_name"];    
    $lookup[$assignee] = $todos[$i]["assigned_name"];    
            
    $due = ($todos[$i]["duedate"]==$today?"today":"late");
    
    if ($todos[$i]["assigned_email"]!=$todos[$i]["owner_email"])    
    {
      // Assignee and owner are 2 separate people. They should both get mail.      
      if ($owner!="") $mails[$owner]["byyou"][$due][] = $todos[$i];
    }
    if ($assignee!="") $mails[$assignee]["toyou"][$due][] = $todos[$i];
  }
 
  foreach ($mails as $to => $todometa)
  {
    $body = "";    
    $cc = "";    
    
    $whos = array("toyou", "byyou");
    for ($j=0;$j<2;$j++) // small manual loop, to ensure that toyou is 
                         // displayed before byyou (if we would use a foreach, 
                         // the order could be random).
    {        
      $who = $whos[$j];
      $todos=$todometa[$who];
      $body.=text("todocheck_mail_assigned_$who")."\n";
      $body.=str_repeat("-", 70)."\n\n";
      
      $today = $todos["today"];
      if (count($today)>0)
      {
        $body.=text("todocheck_mail_duetoday")."\n\n";
        for ($i=0, $_i=count($today);$i<$_i;$i++)
        {
          $body.="  - ".$today[$i]["title"];          
          if ($today[$i]["project_name"]!="") $body.= " (".$today[$i]["project_name"].")";

          // Display owner if this todo is assigned to you by somebody else then you.
          if ($who=="toyou" && $today[$i]["owner_email"]!=$to) 
          {
            $body.="\n    ".text("owner").": ".$today[$i]["owner_name"]." <".$today[$i]["owner_email"].">";
          }
          // always show assigned_to. (bugs assigned to ourselve don't show up as a byyou, only as a toyou)
          else if ($who=="byyou")
          {
            $body.="\n    ".text("assigned_to").": ".$today[$i]["assigned_name"]." <".$today[$i]["assigned_email"].">";
          }          
          $body.="\n\n    ".str_replace("\n","\n    ",$today[$i]["description"])."\n\n";
        }
        $body.="\n";
      }            
           
      $late = $todos["late"];
      if (count($late)>0)
      {
        $body.=text("todocheck_mail_late")."\n\n";
        for ($i=0, $_i=count($late);$i<$_i;$i++)
        {  
          $body.="  - ".$late[$i]["title"];          
          if ($late[$i]["project_name"]!="") $body.= " (".text("project").": ".$late[$i]["project_name"].")\n";
          $body.="    ".text("duedate").": ".$late[$i]["duedate"];          
          // Display owner if this todo is assigned to you by somebody else then you.
          if ($who=="toyou" && $late[$i]["owner_email"]!=$to) 
          {
            $body.="\n    ".text("owner").": ".$late[$i]["owner_name"]." <".$late[$i]["owner_email"].">";
          }
          // always show assigned_to. (bugs assigned to ourselve don't show up as a byyou, only as a toyou)
          else if ($who=="byyou")
          {
            $body.="\n    ".text("assigned_to").": ".$late[$i]["assigned_name"]." <".$late[$i]["assigned_email"].">";
          }                    
          $body.="\n\n    ".str_replace("\n", "\n    ",$late[$i]["description"])."\n\n";
        }
      }
      $body.="\n";
    }
    usermail($to,text("todocheck_mail_subject"),$body);
    echo "sent mail to $to";      
    echo "\n";

  }
  
?>