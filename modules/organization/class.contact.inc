<?php

userelation("atkmanytoonerelation");
include_once (moduleDir("person")."/class.person.inc");

class contact extends person
{
  function contact()
  {
    $this->person("contact");
    $this->addFilter("person.role='contact'");
    $this->add(new atkManyToOneRelation("company","organization.organization",AF_HIDE));
    $this->add(new atkAttribute("role"           , AF_READONLY|AF_HIDE));
  }

  function initial_values()
  {
    $initial["role"] = "contact";
    return $initial;
  }

  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }
}

?>
