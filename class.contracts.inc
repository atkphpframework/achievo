<?php

class contracts extends atkNode
{
  function contracts()
  {
    $this->atkNode("contracts",NF_ADD_LINK);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("description",AF_OBLIGATORY|AF_HIDE_LIST));    
    $this->addAttribute(new atkAttribute("period_price",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkManyToOneRelation("contracttype","contracttype",AF_SEARCHABLE|AF_OBLIGATORY));    
    $this->addAttribute(new atkListAttribute("billing_period",array(text("monthly"),text("quarterly"),text("yearly"),text("per_incident")),array(),AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY));    
    $this->addAttribute(new atkManyToOneRelation("customer","customer",AF_SEARCHABLE|AF_OBLIGATORY));    
    
    $this->setTable("contract","contract");
    $this->setOrder("contract_startdate");
   }

   function contract_period_price_edit($rec)
   {
     return $this->m_attribList["contract_period_price"]->edit($rec)." ".atkconfig("currency_symbol");
   }
  
}
?>
