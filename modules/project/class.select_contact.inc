<?php

include_once (moduleDir("person")."/class.person.inc");
userelation("atkmanytoonerelation");

class select_contact extends person
{
  function select_contact()
  {
    $this->person("select_contact");
    $this->addFilter("person.role = 'contact'");

    // Hide the role attribute
    $attr = &$this->getAttribute("role");
    $attr->addFlag(AF_HIDE);

    $this->add(new atkManyToOneRelation("company","organization.organization", AF_RELATION_AUTOLINK));

    $this->setSecurityAlias("organization.contact");
  }

  function initial_values()
  {
    return array(
      "role" => "contact",
    );
  }

  function descriptor_def()
  {
    return "[lastname], [firstname] ([company.name])";
  }
}

?>