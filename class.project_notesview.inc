<?php

class project_notesview extends atkNode
{
  function project_notesview()
  {
    global $ATK_VARS;
    
    $this->atkNode("project",NF_EDITAFTERADD|NF_NO_ADD);

    $this->addAttribute(new atkAttribute("id"                                , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name"                              , AF_READONLY));
    $this->addAttribute(new atkManyToOneRelation("customer","customer"       , AF_READONLY));
    $this->addAttribute(new atkManyToOneRelation("coordinator","employee"    , AF_READONLY));
    $this->addAttribute(new atkTextAttribute("description"                   , AF_READONLY));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y",0,0 , AF_READONLY|AF_HIDE_LIST));    
    $this->addAttribute(new atkOneToManyRelation("Notes","project_notes","project_id"  , AF_HIDE_LIST));

    $this->addFilter("project.status","active");
    $this->setTable("project","project");
    $this->setOrder("name");

    atkdebug("project::project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }
  
  function initial_values()
  {
    return array("startdate"=>array("year"=>date(Y),"month"=>date(m),"day"=>date(d)),"status"=>"active");
  }

  function action_stats()
  {
    include_once 'projectstats.inc';   // MILESTONES FUNCTIONS     
  }
  
  function action_planning()
  {
    include_once 'projectplanning.inc';   // MILESTONES FUNCTIONS     
  }
  
  function action_billsetup()
  {
    include_once 'billsetup.inc';
  } 
  
  function action_billing()
  {
    include_once 'projectbilling.inc';
  } 
  
  function postDel($rec)
  {
    // TODO: delete phases and dependencies, and hours (???)
  }
}
?>