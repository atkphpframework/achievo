<?php

class project_category extends atkNode
{

    function project_category()
    {
        $this->atkNode("project_category");

        $this->add(new atkAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("project_category", AF_OBLIGATORY | AF_UNIQUE | AF_SEARCHABLE, 50));

        $this->setTable("project_category");
        $this->setOrder("project_category");
        $this->setDescriptorTemplate("[project_category]");
        $this->setSecurityAlias("project.activity");
    }

}

?>