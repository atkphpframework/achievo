<?php

userelation("atkmanytoonerelation");
useattrib("atkemailattribute");
useattrib("atktextattribute");

class contact extends atkNode
{
  function contact()
  {      
    $this->atkNode("contact", NF_ADD_LINK); 

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("company","customer",AF_HIDE_LIST));
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE));
    $this->add(new atkAttribute("firstname",AF_SEARCHABLE));
    $this->add(new atkAttribute("address",AF_HIDE_LIST));
    $this->add(new atkAttribute("zipcode",AF_HIDE_LIST));
    $this->add(new atkAttribute("city",AF_HIDE_LIST));
    $this->add(new atkAttribute("country",AF_HIDE_LIST));
    $this->add(new atkAttribute("phone"));
    $this->add(new atkAttribute("cellular"));
    $this->add(new atkAttribute("fax"));
    $this->add(new atkEmailAttribute("email"));
    $this->add(new atkTextAttribute("remark", AF_HIDE_LIST)); 

    $this->setTable("contact", "contact");
    $this->setOrder("contact.lastname");
  }

  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }

  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }
}

?>