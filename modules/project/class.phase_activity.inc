<?php

userelation("atkmanytoonerelation");

class phase_activity extends atkNode
{
  function phase_activity()
  {
    $this->atkNode("phase_activity");

    $this->add(new atkManyToOneRelation("activityid","project.activity",AF_PRIMARY));
    $this->add(new atkManyToOneRelation("phaseid","project.phase",AF_PRIMARY));       

    $this->setTable("phase_activity");
    
  }

  function descriptor_def()
  {
    return "activityid: [activity] (todo: name)";
  }
}
?>