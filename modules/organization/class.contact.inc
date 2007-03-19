<?php

userelation("atkmanytoonerelation");
atkimport("modules.person.person");
useattrib("organization.contactprojectslist");
     useattrib("atkcreatedbyattribute");
  useattrib("atkCreateStampAttribute");
  useattrib("atkupdatedbyattribute");
  useattrib("atkupdatestampattribute");

class contact extends person
{
  function contact($name="contact")
  {
    $this->person($name);
    $this->remove("userid");

    $this->addFilter("person.role='contact'");
    $this->add(new atkManyToOneRelation("company","organization.organization",AF_HIDE_ADD|AF_RELATION_AUTOLINK));
    $this->add(new atkAttribute("role"           , AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15));
    $this->add(new contactprojectslist("projects", "active", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new contactprojectslist("archivedprojects", "archived", AF_HIDE_LIST|AF_HIDE_ADD));
    
    $this->add(new atkCreatedByAttribute("created_by",AF_HIDE_LIST),"history");
    $this->add(new atkCreateStampAttribute("created_on",AF_HIDE_LIST),"history");
    $this->add(new atkUpdatedByAttribute("last_modified_by",AF_HIDE_LIST),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST),"history");
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