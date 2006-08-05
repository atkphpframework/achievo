<?php

class scheduler_dates extends atkNode
{
  function scheduler_dates()
  {
    $this->atkNode("scheduler_dates");
    $this->add(new atkNumberAttribute("scheduler_id",AF_PRIMARY));
    $this->add(new atkDateAttribute("date"));
    $this->setTable("scheduler_dates");
  }
}


?>