<?

class todo extends atkNode
{
 var $prioId   = array(1,2,3);
 var $prioName = array("High","Normal","Low");

  function todo()
  {
    global $atkaction, $g_user;

    $this->atkNode("todo", NF_TRACK_CHANGES);
    $this->addAttribute(new atkAttribute("id"                   , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("owner"                , AF_READONLY|AF_HIDE));
    $this->addAttribute(new atkManyToOneRelation("projectid"    , "project",AF_READONLY|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("title"                , AF_OBLIGATORY));
    $this->addAttribute(new atkManyToOneRelation("assigned_to"  , "employee", AF_OBLIGATORY|AF_SEARCHABLE));    
    $this->addAttribute(new atkDateAttribute("entrydate"        , AF_READONLY_EDIT|AF_HIDE_ADD));
    $this->addAttribute(new atkDateAttribute("duedate"          , "F d Y","d F Y", date("Ymd"), 0, AF_OBLIGATORY));
    $this->addAttribute(new atkListAttribute("priority"         , $this->prioName, $this->prioId, AF_OBLIGATORY));
    $this->addAttribute(new atkTextAttribute("description"      , AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkListAttribute("status"           , array("Assigned","In progress","Awaiting external person","On hold","Done"), array(1,2,3,4,5),AF_SEARCHABLE));
    $this->addAttribute(new atkBoolAttribute("mail_on_change"   , AF_HIDE_LIST));

    $this->setTable("todo", "todo");         
    $this->setOrder("duedate");

  }

  function initial_values()
  {
    global $g_user;
    return array("owner"=>$g_user["name"],
                 "assigned_to"=>array("userid"=>$g_user["name"]),
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
  }

  // overide display of priority (set high to a red font)
  function priority_display($rec)
  {
   if ($rec["priority"] == 1)
   {
    return "<font color='#FF0000'>".$this->prioName[$rec["priority"]-1]."</font>";
   } 
   else
   {
    return $this->prioName[$rec["priority"]-1];
   }
  }

  function descriptor_def()
  {
   return "[title]";
  }
  
  function postAdd($rec)
  {
   global $g_db;

   // obtain some project info
   $assignTo = $rec["assigned_to"]["userid"];
   $assigner = $rec["owner"]; 
   $pid      = $rec["projectid"]["id"];

   // get project name
   $q       = "SELECT name FROM project WHERE id=$pid";
   $r       = $g_db->getRows($q);
   $project = $r[0]["name"];

   // now get her/his email address
   $q  = "SELECT email FROM employee WHERE userid='$assignTo'";
   $r  = $g_db->getRows($q);
   $to = $r[0]["email"];
   
   // now assemble the mail body
   $subj  = text("Todo").": ".$rec["title"];
   $body  = $assigner." ".text("assigned a new task to you")."\n\n";
   $body .= text("Project").":\t".$project."\n";
   $body .= text("Priority")."\t".$this->prioName[$rec["priority"]-1]."\n";
   $body .= text("Description").":\t".$rec["description"]."\n";

   // send mail
   mail($to, $subj, $body, "From: achievo@achievo.com\n");  
  }
  
  function postUpdate($rec)
  {
   global $g_db;

   if ($rec["mail_on_change"] == 1)
   {
    // check if they match and if not mail the item owner
    if ($rec["atkorgrec"]["status"] != $rec["status"])
    {  
     $owner = $rec["owner"];
     $q     = "SELECT email FROM employee WHERE userid='$owner'";
     $r     = $g_db->getRows($q);
     $to    = $r[0]["email"];
     $pid   = $rec["projectid"]["id"];
     $body  = $rec["assigned_to"]["userid"]." ".text("changed the status of item")." ".$rec["title"]."\n";
 
     // get project name
     $q       = "SELECT name FROM project WHERE id=$pid";
     $r       = $g_db->getRows($q);
     $project = $r[0]["name"];
 
     // send mail
     mail($to,$project." ".text("todo status"), $body, "From: achievo@achievo.com\n");
    }
   }
  }

}
?>
