<?php

class customer extends atkNode
{
  function customer()
  {
    $this->atkNode("customer", NF_EDITAFTERADD); 

    $this->addAttribute(new atkAttribute("id" ,AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY));
    $this->addAttribute(new atkOneToManyRelation("contactperson","contact","company",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("zipcode", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("city", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("country", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("phone",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("fax",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("email",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("bankaccount", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkTextAttribute("remark", AF_HIDE_LIST|AF_HIDE_ADD)); 
    $this->addAttribute(new atkOneToManyRelation("contracts","contracts","customer",AF_HIDE_LIST));    
    $this->setTable("customer", "customer");
    $this->setOrder("customer.name");
    $this->setIndex("name");
    
    atkdebug("customer::customer()");    
  }

  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }

  function descriptor_def()
  {
    return "[name], [city]";
  }
}
?>
