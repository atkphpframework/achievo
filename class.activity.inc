<?php

class activity extends atkNode
{
  function activity()
  {
    $this->atkNode("activity");

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("description",AF_SEARCHABLE));
    $this->addAttribute(new atkBoolAttribute("remarkrequired"));

    $this->setTable("activity","activity");
    $this->setOrder("name");

    atkdebug("activity::activity()");
  }

  function descriptor_def()
  {
    return "[name]";
  }
  
  function action_stats()
  {
    include_once "activitystats.inc";
  }
}
?>