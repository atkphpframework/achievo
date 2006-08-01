<?php

userelation("atkmanytoonerelation");
atkimport("modules.person.person");
useattrib("organization.contactprojectslist");

class contact extends person
{
  function contact($name="contact")
  {
    $this->person($name);
    $this->remove("userid");

    $this->addFilter("person.role='contact'");
    $this->add(new atkManyToOneRelation("company","organization.organization",AF_HIDE_ADD|AF_RELATION_AUTOLINK));
    $this->add(new atkAttribute("role"           , AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15));
    $this->add(new contactprojectslist("projects", "active"));
    $this->add(new contactprojectslist("archivedprojects", "archived"));
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