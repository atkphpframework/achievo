<?php

include_once (moduleDir("organization")."/class.contact.inc");
userelation("atkmanytoonerelation");

class select_contact extends contact
{
  function select_contact()
  {
    $this->contact("select_contact");
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