<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atktextattribute");
useattrib("atkdateattribute");
useattrib("atknumberattribute");

class project_todoview extends atkNode
{
  function project_todoview()
  {
    global $ATK_VARS;

    $this->atkNode("project_todoview",NF_NO_ADD|NF_NO_DELETE);

    $this->add(new atkNumberAttribute("id"                                , AF_AUTOKEY));
    $this->add(new atkAttribute("name"                              , AF_READONLY));
    //$this->add(new atkManyToOneRelation("customer","organization.organization"       , AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("coordinator","employee.employee", AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkTextAttribute("description"                   , TEXT_LARGE, AF_READONLY));
    $this->add(new atkDateAttribute("startdate","","",0,0 , AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("todo","todo.todo","projectid"  , AF_HIDE_LIST));

    $this->addFilter("project.status","active");
    $this->setTable("project","project");
    $this->setOrder("name");
    $this->setSecurityAlias("todo.todo");
    $this->addSecurityMap("edit","add");


    atkdebug("project::project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }

}

?>
