<?php
userelation("atkmanytoonerelation");

class tpl_dependency extends atkNode
{

    function tpl_dependency()
    {
        $this->atkNode("tpl_dependency", NF_READONLY);

        $this->add(new atkManyToOneRelation("phaseid_row", "project.phase", AF_PRIMARY));
        $this->add(new atkManyToOneRelation("phaseid_col", "project.phase", AF_PRIMARY));
        $this->add(new atkManyToOneRelation("projectid", "project.project", AF_PRIMARY));

        $this->setTable("tpl_dependency");
    }

}

?>