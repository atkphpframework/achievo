<?php

useattrib("atkdateattribute");
useattrib("atktextattribute");
userelation("atkmanytoonerelation");

class project_notes extends atkNode
{
  function project_notes()
  {
    $this->atkNode("project_notes");

    $this->add(new atkAttribute("id"                   , AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_HIDE_ADD|AF_READONLY));
    $this->add(new atkManyToOneRelation("projectid", "project.project",AF_OBLIGATORY|AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkAttribute("title"                , AF_OBLIGATORY|AF_SEARCHABLE, 50));    
    $this->add(new atkDateAttribute("entrydate"    , "F d Y","d F Y", date("Ymd"), 0, AF_HIDE_EDIT|AF_HIDE_ADD));
    $this->add(new atkTextAttribute("description"      , AF_OBLIGATORY|AF_HIDE_LIST));
    $this->setTable("project_notes");
    $this->setOrder("entrydate");

  }

  function initial_values()
  {
    global $g_user;
    return array("owner"=>array("id"=>$g_user["id"]),
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
  }

  function descriptor_def()
  {
    return "[title]";
  }
}
?>