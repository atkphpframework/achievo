<?php

class contracts extends atkNode
{
  function contracts()
  {
    $this->atkNode("contracts");

    $this->addAttribute(new atkAttribute("contract_id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("contract_desc",AF_OBLIGATORY));    
    $this->addAttribute(new atkAttribute("period_price",AF_OBLIGATORY));
    $this->addAttribute(new atkDummyAttribute("test",atkconfig("currency_symbol"),AF_HIDE_LIST));     
    $this->addAttribute(new atkManyToOneRelation("contract_type","contracttype","type_id",AF_HIDE_LIST));    
    $this->addAttribute(new atkListAttribute("billing_period",array("Maandelijks","Kwartaal","Half Jaar","Jaar","Per incident"),array(),AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd")));
    $this->addAttribute(new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd")));    
    $this->addAttribute(new atkManyToOneRelation("customer","customer","id",AF_HIDE_LIST));    
    
    $this->setTable("contracts","contract_id");
    $this->setOrder("contract_type");
   }

  
}
?>