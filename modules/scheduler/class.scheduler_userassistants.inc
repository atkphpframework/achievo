<?php
userelation("atkmanytoonerelation");
class scheduler_userassistants extends atkNode
{
   function scheduler_userassistants()
   {
     $this->atkNode("scheduler_userassistants");
     $this->add(new atkManyToOneRelation("userid","scheduler.scheduler_userpreferences",AF_PRIMARY));
     $this->add(new atkManyToOneRelation("employeeid","employee.employee",AF_PRIMARY));
     $this->setSecurityAlias("scheduler.scheduler");

     $this->setTable("scheduler_userassistants");
   }
}

?>