<?php
userelation("atkmanytoonerelation");

class phase_activity extends atkNode
{

    function phase_activity()
    {
        $this->atkNode("phase_activity");

        $this->add(new atkManyToOneRelation("phaseid", "project.phase", AF_PRIMARY | AF_HIDE));
        $this->add(new atkManyToOneRelation("activityid", "project.activity", AF_PRIMARY | AF_READONLY));

        $this->setSecurityAlias("project.phase");
        $this->setTable("phase_activity");
    }

    function descriptor_def()
    {
        return "activityid: [activity] (todo: name)";
    }

}

?>