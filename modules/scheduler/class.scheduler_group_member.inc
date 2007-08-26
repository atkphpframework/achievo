<?php
userelation("atkmanytoonerelation");

class scheduler_group_member extends atkNode
{
  function scheduler_group_member()
  {
    $this->atkNode("scheduler_group_member",NF_NO_SECURITY);
    $this->add(new atkmanytoonerelation("groupid","scheduler.scheduler_group",AF_PRIMARY));
    $this->add(new atkmanytoonerelation("userid","employee.employee",AF_PRIMARY));
    
    $this->setTable("scheduler_group_member");
    $this->setSecurityAlias("scheduler.userpreferences");
  }
}

?>