<?php

/**
 * Module Person
 * Class person
 *
 * This class is a generalisation of employee and contact and is used for schedule_attendees
 * and for the general attribute of employee and contact.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkemailattribute");
useattrib("atktextattribute");
useattrib("atklistattribute");

class person extends atkNode
{
  function person($name="person", $flags=0)
  {
    $this->atkNode($name, $flags|NF_ADD_LINK);

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE|AF_OBLIGATORY));
    $this->add(new atkAttribute("firstname",AF_SEARCHABLE));
    $this->add(new atkAttribute("address",AF_HIDE_LIST));
    $this->add(new atkAttribute("zipcode",AF_HIDE_LIST));
    $this->add(new atkAttribute("city",AF_HIDE_LIST));
    $this->add(new atkAttribute("country",AF_HIDE_LIST));
    $this->add(new atkAttribute("phone"));
    $this->add(new atkAttribute("cellular"));
    $this->add(new atkAttribute("fax"));
    $this->add(new atkEmailAttribute("email"));
    $this->add(new atkAttribute("function", AF_HIDE_LIST));
    $this->add(new atkTextAttribute("remark", AF_HIDE_LIST));
    $this->add(new atkAttribute("role", AF_HIDE));

    $this->setTable("person", "person");
    $this->setOrder("person.role, person.lastname");
    $this->setIndex("lastname");
    atkdebug("person::person()");
  }

  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }

  function descriptor_def()
  {
    return "[lastname], [firstname] ([role])";
  }
}

?>