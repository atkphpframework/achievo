<?php
useattrib("atknumberattribute");
useattrib("atkupdatedbyattribute");

class scheduler_alarms extends atkNode
{
  function scheduler_alarms()
  {
    $this->atkNode("scheduler_alarms",NF_NO_EDIT|NF_NO_VIEW);
    $this->add(new atkNumberAttribute("scheduler_id",AF_PRIMARY|AF_HIDE));
    $this->add(new atkNumberAttribute("startdate",AF_PRIMARY|AF_HIDE));
    $this->add(new atkNumberAttribute("before"));
    $this->add(new atkNumberAttribute("senddate",AF_HIDE));
    $this->add(new atkNumberAttribute("userid"));
    $this->add(new atkUpdatedByAttribute("owner"));
    $this->setTable("scheduler_alarms");
    $this->setSecurityAlias("scheduler.scheduler");
  }

}
?>