<?php

userelation("atkmanytoonerelation");

class tpl_phase_activity extends atkNode
{
  function tpl_phase_activity()
  {
    $this->atkNode("tpl_phase_activity");

    $this->add(new atkManyToOneRelation("activityid","activity",AF_PRIMARY));
    $this->add(new atkManyToOneRelation("phaseid","tpl_phase",AF_PRIMARY)); 

    $this->setTable("tpl_phase_activity","tpl_phase_activity");    
  }
}
?>