<?php

userelation("atkonetomanyrelation");

class scheduler_group extends atkNode
{
  function scheduler_group()
  {
    $this->atkNode("scheduler_group",NF_NO_SECURITY);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("userid",AF_HIDE));
    $this->add(new atkAttribute("name"));
    $this->add(new atkOneToManyRelation("members","scheduler.scheduler_group_member","groupid",AF_HIDE_LIST));
    
    $this->setTable("scheduler_group");
    $this->setOrder("scheduler_group.name");
  }
}

?>