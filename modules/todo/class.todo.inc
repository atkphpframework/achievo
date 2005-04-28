<?php

require_once("achievotools.inc");
userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkupdatestampattribute");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atktextattribute");

class todo extends atkNode
{
  var $prioId   = array(1,2,3,4,5);
  var $prioName = array("highest","high","normal","low","lowest");

  function todo()
  {
    $this->atkNode("todo", NF_TRACK_CHANGES);

    $this->add(new atkAttribute("id"                   , AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_READONLY|AF_HIDE|AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("projectid"    , "project.project",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkAttribute("title"                , AF_OBLIGATORY, 50));
    $this->add(new atkManyToOneRelation("assigned_to"  , "employee.employee", AF_SEARCHABLE));
    $this->add(new atkDateAttribute("entrydate"        , "F d Y", "d F Y", 0, 0, AF_READONLY_EDIT|AF_HIDE_ADD|AF_OBLIGATORY));
    $this->add(new atkDateAttribute("duedate"          , "F d Y","d F Y", 0, 0, AF_OBLIGATORY));
    $this->add(new atkUpdateStampAttribute("updated"   , AF_HIDE|AF_FORCE_LOAD)); // force_load is necessary, because we need the value when copying the todo to the history table.
    $this->add(new atkListAttribute("priority"         , $this->prioName, $this->prioId, AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"      , TEXT_LARGE, AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkListAttribute("status"           , array("todo_new", "todo_rejected", "todo_in_progress","todo_on_hold","todo_completed"), array(1,2,3,4,5),AF_SEARCHABLE|AF_OBLIGATORY));
    $this->add(new atkOneToManyRelation("history","todo.todo_history","todoid",AF_HIDE_LIST|AF_CASCADE_DELETE));

    $this->setTable("todo");
    $this->setOrder("duedate, priority");

  }

  function initial_values()
  {
    global $g_user;

    $entrydate = array("year"=>date("Y"),
                    "month"=>date("m"),
                    "day"=>date("d"));
    $nextweek = getdate(time()+60*60*24*7);
    $duedate = array("year"=>$nextweek['year'],
                  "month"=>$nextweek['mon'],
                  "day"=>$nextweek['mday']);
    return array("owner"=>array("id"=>$g_user["id"]),
                 "entrydate"=>$entrydate,
                 "duedate"=>$duedate,
                 "updated"=>$date,
                 "priority"=>3);
  }

  // overide display of priority (set high to a red font)
  function priority_display($rec)
  {
   $prio = $rec["priority"];   
   $org = $this->m_attribList["priority"]->display($rec);
   $result = "";      

   switch ($prio)
   {
    case 1:
     $result = '<font color="'.COLOR_ERROR.'"><b>'.$org."</b></font>"; break;
    case 2:
     $result = '<font color="'.COLOR_WARNING.'"><b>'.$org."</b></font>"; break;
    case 4:
     $result = '<font color="#666666">'.$org."</font>"; break;
    default:
     $result = $org;
     break;
   }
   return $result;
  }
  
  function duedate_display($rec)
  {
    $duedate = sprintf("%d-%02d-%02d", $rec["duedate"]["year"], $rec["duedate"]["month"], $rec["duedate"]["day"]);            
    $open = (in_array($rec["status"], array(1, 3, 4)));    
    $org = $this->m_attribList["duedate"]->display($rec);
    
    if (!$open)
    {
      return $org;
    }
    else
    {                 
      $days_left = $this->daysLeft(date("Y-m-d"), $duedate);         
      
      if ($days_left>2) return $org; // normal
      if ($days_left<0) return '<font color="#FF0000"><b>'.$org.'</b></font>'; // expired
      return '<font color="#ee6c22"><b>'.$org.'</b></font>'; // almost expired
      
    }         
  }

  function descriptor_def()
  {
   return "[title]";
  }

  // support function to add the priority header to a mail message
  function prioHeader($level)
  {
   $cr = chr(13).chr(10);

   switch ($level)
   {
    case 1:
     $mslevel = "High";
     break;

    case 2:
     $mslevel = "High";
     break;

    case 3:
     $mslevel = "Normal";
     break;

    case 4:
     $mslevel = "Low";
     break;

    case 5:
     $mslevel = "Low";
     break;

    default:
     $level   = 3;
     $mslevel = "Normal";
   }

   $header  = "X-Priority: ".$level.$cr;
   $header .= "X-MSMail-Priority: ".$mslevel.$cr;
   return $header;
  }

  function postAdd($rec)
  {
   global $g_db;

   // obtain some project info
   $assignTo = $rec["assigned_to"]["id"];
   $assigner = $rec["owner"]["id"];
   $pid      = $rec["projectid"]["id"];

   // get project name
   $q       = "SELECT name FROM project WHERE id=$pid";
   $r       = $g_db->getRows($q);
   $project = $r[0]["name"];

   // now get her/his email address
   $q  = "SELECT email, lastname, firstname, userid, id FROM person WHERE id IN ('$assignTo','".$assigner."')";
   $emails  = $g_db->getrows($q);

   // now assemble the mail body
   $subj  = text("todoadded").": ".$rec["title"];

   $body = text("todo").": ".$rec["title"]."\n";
   $body.= text("description").": \n".$rec["description"]."\n\n";
   $body.= text("priority").": ".$this->prioName[$rec["priority"]-1]."\n";
   $body.= text("status").": ".$this->m_attribList["status"]->display($rec)."\n";
   if ($project!="") $body.= text("project").": ".$project."\n";
   $body.= text("duedate").": ".$this->m_attribList["duedate"]->display($rec)."\n";

   $body.= "\n";

   for ($i=0;$i<count($emails);$i++)
   {
     if ($emails[$i]["id"]==$assigner)
       $body.= text("todo_ownedby").": ".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."\n";
     if ($emails[$i]["id"]==$assignTo)
       $body.= text("todo_assignedto").": ".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."\n";
     $to[] = $emails[$i]["email"];
   }

   // send mail
   $header = $this->prioHeader($rec["priority"]);
   usermail($to, $subj, $body, $header);
  }

  function todoChanged($new, $old)
  {
    foreach (array_keys($this->m_attribList) as $attribname)
    {
      if ($attribname!="updated") // leave out the updated attrib, for it always
                                  // changes, but that doesn't mean a change in
                                  // the record.
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (!$p_attrib->equal($new,$old))
        {
          atkdebug($attribname." changed");
          return true;
        }
      }
    }
    return false;
  }

  function postUpdate($rec)
  {
   global $g_db, $g_user;

   if ($this->todoChanged($rec,$rec["atkorgrec"]))
   {
     $owner = $rec["owner"]["id"];
     $newassign = $rec["assigned_to"]["id"];
     $oldassign = $rec["atkorgrec"]["assigned_to"]["id"];

     // read all relevant emails.
     $query = "SELECT DISTINCT lastname, firstname, email, userid, id
               FROM person
               WHERE id IN('$owner','".$g_user["id"]."','$newassign','$oldassign')";
     $emails    = $g_db->getrows($query);
     $pid  = $rec["projectid"]["id"];
     // get project name
     $query = "SELECT name FROM project WHERE id=$pid";
     $res   = $g_db->getrows($query);
     $project = $res[0]["name"];

     $body = text("todo").": ".$rec["title"]."\n";
     $body.= text("description").": \n".$rec["description"]."\n\n";
     $body.= text("priority").": ".$this->prioName[$rec["priority"]-1]."\n";
     $body.= text("status").": ".$this->m_attribList["status"]->display($rec)."\n";
     if ($project!="") $body.= text("project").": ".$project."\n";
     $body.= text("duedate").": ".$this->m_attribList["duedate"]->display($rec)."\n";

     $body.= "\n";
     for ($i=0;$i<count($emails);$i++)
     {
       if ($emails[$i]["id"]==$owner)
         $body.= text("todo_ownedby").": ".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."\n";
       if ($emails[$i]["id"]==$oldassign)
         $body.= text("todo_previouslyassignedto").": ".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."\n";
       if ($emails[$i]["id"]==$newassign)
         $body.= text("todo_assignedto").": ".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."\n";
       if ($emails[$i]["id"]==$g_user["id"])
         $body.= text("todo_changedby").": ".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."\n";
       $to[] = $emails[$i]["email"];
     }

     // send mail
     $header = $this->prioHeader($rec["priority"]);
     usermail($to,text("todochanged"), $body, $header);

     // also store the old todo in the history table.
     $historynode = getNode("todo.todo_history");
     $rec["atkorgrec"]["todoid"]["id"] = $rec["id"]; // the todoid in the historytable
                                               // points to the current todoid.
     $historynode->addDb($rec["atkorgrec"]);
   }
  }
  
  /** Workaround for bug #194.
   * By setting this method, the owner field is always filled with something, so ownerless
   * todo's will not complain that they can't be saved.
   * Note that this will not actually store the new owner, for that would be even more 
   * dangerous.
   * Since this only happens to very few todo's that were converted when a certain 
   * conversion bug was still present, this workaround suffices.
   */
  function edit_values($record)
  {
    if ($record["owner"]["id"]=="")
    {
      $record["owner"]=getUser();
    }    
    return $record;
  }
  
   function daysLeft($start, $end)  
   {                      
     if ($start==""||$end=="") return 0;    
      
     $d1 = adodb_mktime(12,0,0,substr($start,5,2), substr($start,8,2), substr($start,0,4));    
     $d2 = adodb_mktime(12,0,0,substr($end,5,2), substr($end,8,2), substr($end,0,4));    
     return floor(($d2-$d1)/(60*60*24));  
   }   

}

?>
