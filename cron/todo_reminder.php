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
  // being mailed: the owner, and the assignee. If multiple todos have the
  // same owner and assignee, we can group them into one mail.
  
  $mails = array();
  $today = date("Y-m-d");
  
  for ($i=0, $_i=count($todos); $i<$_i; $i++)
  {
    $grouping = $todos[$i]["assigned_email"]."|".$todos[$i]["owner_email"];
    $mails[$grouping]["owner_name"] = $todos[$i]["owner_name"];
    $mails[$grouping]["owner_email"] = $todos[$i]["owner_email"];
    $mails[$grouping]["assigned_name"] = $todos[$i]["assigned_name"];
    $mails[$grouping]["assigned_email"] = $todos[$i]["assigned_email"];
    
    $todo = array("title"=>$todos[$i]["title"],
                  "description"=>$todos[$i]["description"],
                  "duedate"=>$todos[$i]["duedate"],
                  "project"=>$todos[$i]["project_name"]);
    
    if ($todos[$i]["duedate"]==$today)
    {
      $mails[$grouping]["todos_today"][] = $todo;
    }
    else
    {
      $mails[$grouping]["todos_late"][] = $todo;
    }
  }
 
  foreach ($mails as $grouping => $todos)
  {
    $body = "";
    $to = "";
    $cc = "";
    if ($todos["assigned_email"] != "") $to = $todos["assigned_email"];      
    if ($todos["owner_email"] != "" && $todos["owner_email"]!=$todos["assigned_email"]) 
    {
      $cc = $todos["owner_email"];
    }
    
    if ($to=="") // No to address.. 
    {
      $to = $cc; // mail at least to the owner.
      $cc = "";
    }        
        
    if ($to!="") // If we don't have a $to by now, we can't mail anything.
    {
      $body.=text("todocheck_mail_header")."\n\n";
      $body.=text("owner").": ".$todos["owner_name"]." <".$todos["owner_email"].">\n";
      $body.=text("assigned_to").": ".$todos["assigned_name"]." <".$todos["assigned_email"].">\n\n\n";
    
      $today = $todos["todos_today"];
      if (count($today)>0)
      {
        $body.=text("todocheck_mail_duetoday")."\n\n";
        for ($i=0, $_i=count($today);$i<$_i;$i++)
        {
          $body.="  ".$today[$i]["title"];
          if ($today[$i]["project"]!="") $body.= " (".$today[$i]["project"].")";
          $body.="\n    ".$today[$i]["description"]."\n\n";
        }
      }
      
      $body.="\n";
            
      $late = $todos["todos_late"];
      if (count($late)>0)
      {
        $body.=text("todocheck_mail_late")."\n\n";
        for ($i=0, $_i=count($late);$i<$_i;$i++)
        {  
          $body.="  ".$late[$i]["duedate"]." - ".$late[$i]["title"];
          if ($late[$i]["project"]!="") $body.= " (".text(project).": ".$late[$i]["project"].")";
          $body.="\n".str_repeat(" ",15).$late[$i]["description"]."\n\n";
        }
      }
      
      usermail($to,text("todocheck_mail_subject"),$body,($cc!=""?"Cc: $cc\n":""));
      echo "sent mail to $to";
      if ($cc!="") echo " and a cc to $cc";
      echo "\n";
    }
    else
    {
      echo "couldn't sent mail for user '".$todos["assigned_name"]."' (no email)";
    }
  }
  
?>