<?php

include_once (moduleDir("person")."/class.person.inc");

class select_person extends person
{
  function select_person()
  {
    $this->person("select_person");
    
    // for employees the nonactive ones must be filtered out.
    $this->addFilter("(person.status <> 'nonactive' OR person.role <> 'employee')");

    $this->remove("role");
    $this->remove("fax");
    $this->add(new atkAttribute("role", AF_SEARCHABLE));
    $this->setSecurityAlias("person.person");
  }

}

?>