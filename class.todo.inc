<?php

class todo extends atkNode
{
  function todo()
  {
    $this->atkNode("todo");

    $this->addAttribute(new atkAttribute("id"                   , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("owner"                , AF_HIDE));
    $this->addAttribute(new atkManyToOneRelation("project_id"   , "project",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("title"                , AF_OBLIGATORY));
    $this->addAttribute(new atkManyToOneRelation("assigned_to"  , "employee", AF_OBLIGATORY|AF_SEARCHABLE));    
    $this->addAttribute(new atkDateAttribute("begin_date"       , "F d Y","d F Y", date("Ymd"), 0, AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkDateAttribute("end_date"         , "F d Y","d F Y", date("Ymd"), 0, AF_OBLIGATORY));
    $this->addAttribute(new atkTextAttribute("description"      , AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkListAttribute("status"           , array("Assigned","In progress","Awaiting external person","On hold","Done"), array(1,2,3,4,5),AF_SEARCHABLE));
    
    $this->setTable("project_todo", "project_todo");         
    $this->setOrder("id");

  }

  function initial_values()
  {
    global $g_user;
    return array("owner"=>$g_user["name"]);
  }

  function descriptor_def()
  {
    return "[name]";
  }
}
?>