<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atktextattribute");
useattrib("atkdateattribute");

class project_notesview extends atkNode
{
  function project_notesview()
  {
    global $ATK_VARS;

    $this->atkNode("project_notesview",NF_NO_ADD|NF_NO_DELETE);

    $this->add(new atkAttribute("id"                                , AF_AUTOKEY));
    $this->add(new atkAttribute("name"                              , AF_READONLY));
    $this->add(new atkManyToOneRelation("customer","organization.organization"       , AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("coordinator","employee.employee", AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkTextAttribute("description"                   , AF_READONLY));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0 , AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("notes","notes.project_notes","projectid", AF_HIDE_LIST));

    $this->addFilter("project.status","active");
    $this->setTable("project","project");
    $this->setOrder("name");
    $this->setSecurityAlias("project_notes");

    atkdebug("project::project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }

}

?>