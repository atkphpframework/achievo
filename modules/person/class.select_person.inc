<?php

include_once (moduleDir("person")."/class.person.inc");

class select_person extends person
{
  function select_person()
  {
    $this->person("select_person");
    $this->addFilter("person.status <> 'nonactive' AND (person.role = 'contact' OR person.role = 'employee')");

    $this->remove("role");
    $this->remove("fax");
    $this->add(new atkAttribute("role", AF_SEARCHABLE));
    $this->setSecurityAlias("person.person");
  }

}

?>