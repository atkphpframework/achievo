<?php

class contracttype extends atkNode
{
  function contracttype()
  {
    $this->atkNode("contracttype");

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("description",AF_OBLIGATORY));    
    $this->setTable("contracttype","contracttype");
    $this->setOrder("description");
   }
 
  function descriptor_def()
  {
    return "[description]";
  }
  
}
?>
