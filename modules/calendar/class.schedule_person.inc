<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atktextattribute");
useattrib("atklistattribute");
include_once (moduleDir("person")."/class.person.inc");

class schedule_person extends person
{
  function schedule_person()
  {
    $this->person("schedule_person");
    $this->addFilter("person.status <> 'nonactive'");

    $this->remove("role");
    $this->remove("fax");
    $this->add(new atkAttribute("role", AF_SEARCHABLE));
    $this->setSecurityAlias("person.person");
  }
}

?>