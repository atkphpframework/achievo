<?php

include_once (moduleDir("person")."/class.person.inc");
userelation("atkmanytoonerelation");

class select_contact extends person
{
  function select_contact()
  {
    $this->person("select_contact");
    $this->addFilter("person.role = 'contact'");

    $this->remove("role");
    $this->remove("fax");
    $this->remove("cellular");
    $this->add(new atkManyToOneRelation("company","organization.organization"));

    $this->setSecurityAlias("organization.contact");
  }

  function descriptor_def()
  {
    return "[lastname], [firstname] ([company.name])";
  }
}

?>