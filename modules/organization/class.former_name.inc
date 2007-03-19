<?php
userelation("atkmanytoonerelation");
class former_name extends atkNode 
{
  
  function former_name()
  {
    $this->atkNode("former_name",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("former_name",AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("account","organization.organization",AF_RELATION_AUTOLINK));
    
    $this->setSecurityAlias("organization.organization");
    $this->setOrder("former_name");
    $this->setTable("organization_former_name");
  }
  
  function descriptor_def()
  {
    return "[former_name]";
  }
}

?>