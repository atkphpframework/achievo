<?php

userelation("atkmanytoonerelation");
include_once (moduleDir("person")."/class.person.inc");

class contact extends person
{
  function contact()
  {
    $this->person("contact");

    $p_attrib = &$this->getAttribute("userid");
    $p_attrib->m_flags|=AF_HIDE;

    $this->addFilter("person.role='contact'");
    $this->add(new atkManyToOneRelation("company","organization.organization",AF_HIDE_ADD|AF_RELATION_AUTOLINK));
    $this->add(new atkAttribute("role"           , AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15));
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