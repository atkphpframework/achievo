<?php

class tpl_phase_activity extends atkNode
{
  function tpl_phase_activity()
  {
    $this->atkNode("tpl_phase_activity");

    $this->addAttribute(new atkManyToOneRelation("activityid","activity",AF_PRIMARY));
    $this->addAttribute(new atkManyToOneRelation("phaseid","tpl_phase",AF_PRIMARY)); 

    $this->setTable("tpl_phase_activity","tpl_phase_activity");
    atkdebug("tpl_phase_activity::tpl_phase_activity()");
  }

  function descriptor_def()
  {
    return "activityid: [activity] (todo: name)";
  }
}
?>