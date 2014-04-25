<?php
useattrib("atknumberattribute");
useattrib("person.useridattribute");

class employeeselector extends atkNode
{

    function employeeselector()
    {
        $this->atkNode("employeeselector");

        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new useridAttribute("userid", AF_UNIQUE | AF_SEARCHABLE), 1); // first field
        $this->add(new atkAttribute("lastname", AF_SEARCHABLE | AF_OBLIGATORY, 50));
        $this->add(new atkAttribute("firstname", AF_HIDE_LIST | AF_FORCE_LOAD, 50));

        $this->addFilter("person.status", 'active');
        $this->setTable("person");
        $this->setOrder("person.lastname");
        $this->setSecurityAlias('employee.employee');
        $this->setDescriptorTemplate("[lastname], [firstname] ([userid])");
    }

}

?>