<?php

class phase_activity extends atkNode
{
  function phase_activity()
  {
    $this->atkNode("phase_activity");

    $this->addAttribute(new atkManyToOneRelation("activityid","activity",AF_PRIMARY));
    $this->addAttribute(new atkManyToOneRelation("phaseid","phase",AF_PRIMARY));       

    $this->setTable("phase_activity");
    
    atkdebug("phase_activity::phase_activity()");
  }

  function descriptor_def()
  {
    return "activityid: [activity] (todo: name)";
  }
}
?>