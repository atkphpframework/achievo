<?php

class contact extends atkNode
{
  function contact()
  {      
    $this->atkNode("contact", NF_ADD_LINK); 

    $this->addAttribute(new atkAttribute("id" ,AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkManyToOneRelation("company","customer",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("lastname",AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("firstname",AF_SEARCHABLE));
    $this->addAttribute(new atkAttribute("address",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("zipcode",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("city",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("country",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("phone"));
    $this->addAttribute(new atkAttribute("cellular"));
    $this->addAttribute(new atkAttribute("fax"));
    $this->addAttribute(new atkAttribute("email"));
    $this->addAttribute(new atkTextAttribute("remark", AF_HIDE_LIST)); 

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
