<?php

useattrib("atknumberattribute");
class contracttype extends atkNode
{
  function contracttype()
  {
    $this->atkNode("contracttype", NF_MRA);

    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("description",AF_OBLIGATORY, 100));    
    
    $this->setTable("contracttype","contracttype");
    $this->setOrder("description");
   }
 
  function descriptor_def()
  {
    return "[description]";
  }
  
}
?>
