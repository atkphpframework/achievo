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
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE|AF_OBLIGATORY, 50));
    $this->add(new atkAttribute("firstname",AF_HIDE_LIST|AF_FORCE_LOAD, 50));
    $this->add(new atkAttribute("address",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("zipcode",AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("city",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("state",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("country",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("phone", 0, 20));
    $this->add(new atkAttribute("cellular", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("fax", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkEmailAttribute("email", false, 0, 50));
    $this->add(new atkAttribute("function", AF_HIDE_LIST, 50));
    $this->add(new atkTextAttribute("remark", AF_HIDE_LIST));
    $this->add(new atkAttribute("role", AF_HIDE, 25)); //discriminator

    $this->setTable("person", "person");
    $this->setOrder("person.role, person.lastname");
    $this->setIndex("lastname");
    atkdebug("person::person()");
  }
/*
  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }
  */
  /*
  function lastname_display($record, $mode)
  {
    if ($mode=="list")
    {
      return $record["lastname"].($record["firstname"]!=""?", ".$record["firstname"]:"");
    }
    else
    {
      return $this->m_attribList["lastname"]->display($record, $mode);
    }
  }
  
  function phone_display($record, $mode)
  {
    if ($mode=="list") // in listmode, group the numbers
    {
      $res = array();
      if ($record["phone"]!="")
      {
        $res[] = "<b>".substr(text("phone"),0,1).":</b> ".$record["phone"];
      }
      if ($record["cellular"]!="")
      {
        $res[] = "<b>".substr(text("cellular"),0,1).": </b>".$record["cellular"];
      }
      if ($record["fax"]!="")
      {
        $res[] = "<b>".substr(text("fax"),0,1).": </b>".$record["fax"];
      }
      return implode("<BR>", $res);
    }
    else
    {
      return $this->m_attribList["phone"]->display($record, $mode);
    }
  }
*/
  function descriptor_def()
  {
    return "[lastname], [firstname] ([role])";
  }
    
}

?>