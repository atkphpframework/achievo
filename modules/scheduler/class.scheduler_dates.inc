<?php

useattrib("atknumberattribute");

class scheduler_dates extends atkNode
{
  function scheduler_dates()
  {
    $this->atkNode("scheduler_dates");
    $this->add(new atkNumberAttribute("scheduler_id",AF_PRIMARY));
    $this->add(new atkNumberAttribute("startdate",AF_PRIMARY));
    $this->add(new atkNumberAttribute("enddate"));
    $this->setTable("scheduler_dates");
  }
}


?>