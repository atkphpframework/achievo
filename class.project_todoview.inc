<?php

class project_todoview extends atkNode
{
  function project_todoview()
  {
    global $ATK_VARS;
    
    $this->atkNode("project_todoview",NF_NO_ADD|NF_NO_DELETE);

    $this->addAttribute(new atkAttribute("id"                                , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name"                              , AF_READONLY));
    $this->addAttribute(new atkManyToOneRelation("customer","customer"       , AF_READONLY|AF_HIDE_LIST));
    $this->addAttribute(new atkManyToOneRelation("coordinator","employee"    , AF_READONLY|AF_HIDE_LIST));
    $this->addAttribute(new atkTextAttribute("description"                   , AF_READONLY));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y",0,0 , AF_READONLY|AF_HIDE_LIST));    
    $this->addAttribute(new atkOneToManyRelation("Todo","todo","projectid"  , AF_HIDE_LIST));

    $this->addFilter("project.status","active");
    $this->setTable("project","project");
    $this->setOrder("name");
    $this->setSecurityAlias("todo");

    atkdebug("project::project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }
  
}
?>
