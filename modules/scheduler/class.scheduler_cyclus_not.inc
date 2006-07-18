<?php

  class scheduler_cyclus_not extends atkNode
  {
     function scheduler_cyclus_not()
     {
       $this->atkNode("scheduler_cyclus_not");
       $this->add(new atkAttribute("scheduler_id",AF_PRIMARY));
       $this->add(new atkDateAttribute("date"));
       $this->setTable("scheduler_cyclus_not");
     }
  }
?>