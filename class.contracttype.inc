<?php

class contracttype extends atkNode
{
  function contracttype()
  {
    $this->atkNode("contracttype");

    $this->addAttribute(new atkAttribute("type_id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("contracttype",AF_OBLIGATORY));    
    $this->setTable("contract_types","contract_type_id");
    $this->setOrder("contracttype");
   }
 
  function descriptor_def()
  {
    return "[contracttype]";
  }
  
}
?>