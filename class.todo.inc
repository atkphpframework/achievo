<?php

class todo extends atkNode
{
  function todo()
  {
    $this->atkNode("todo");

    $this->addAttribute(new atkAttribute("id"                   , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("owner"                , AF_READONLY));
    $this->addAttribute(new atkManyToOneRelation("projectid"   , "project",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("title"                , AF_OBLIGATORY));
    $this->addAttribute(new atkManyToOneRelation("assigned_to"  , "employee", AF_OBLIGATORY|AF_SEARCHABLE));    
    $this->addAttribute(new atkDateAttribute("entrydate",AF_READONLY_EDIT|AF_HIDE_ADD));
    $this->addAttribute(new atkDateAttribute("duedate"         , "F d Y","d F Y", date("Ymd"), 0, AF_OBLIGATORY));
    $this->addAttribute(new atkTextAttribute("description"      , AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkListAttribute("status"           , array("Assigned","In progress","Awaiting external person","On hold","Done"), array(1,2,3,4,5),AF_SEARCHABLE));
    
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
                                    "day"=>date("d")),);
  }

  function descriptor_def()
  {
    return "[title]";
  }
}
?>
