<?php

class contracttype extends atkNode
{
  function contracttype()
  {
    $this->atkNode("contracttype", NF_MRA);

    $this->addAttribute(new atkAttribute("id",AF_AUTOKEY));
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
