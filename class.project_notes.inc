<?php

class project_notes extends atkNode
{
  function project_notes()
  {
    $this->atkNode("project_notes");

    $this->addAttribute(new atkAttribute("id"                   , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("owner"                , AF_HIDE));
    $this->addAttribute(new atkManyToOneRelation("project_id"   , "project",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("title"                , AF_OBLIGATORY|AF_SEARCHABLE));    
    $this->addAttribute(new atkDateAttribute("date_inserted"    , "F d Y","d F Y", date("Ymd"), 0, AF_HIDE));
    $this->addAttribute(new atkTextAttribute("description"      , AF_OBLIGATORY|AF_HIDE_LIST));
    $this->setTable("project_notes", "project_notes");         
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