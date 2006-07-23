<?php
userelation("atkmanytoonerelation");
class scheduler_userassistents extends atkNode
{
   function scheduler_userassistents()
   {
     $this->atkNode("scheduler_userassistents");
     $this->add(new atkManyToOneRelation("userid","scheduler.scheduler_userpreferences",AF_PRIMARY));
     $this->add(new atkManyToOneRelation("employeeid","employee.employee",AF_PRIMARY));
     $this->setSecurityAlias("scheduler.scheduler");

     $this->setTable("scheduler_userassistents");
   }
}

?>