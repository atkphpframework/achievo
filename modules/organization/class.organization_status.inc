<?php

class organization_status extends atkNode 
{
  
  function organization_status()
  {
    $this->atkNode("organization_status",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("status_name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("organization.organization");
    $this->setOrder("status_name");
    $this->setTable("organization_status");
  }
  
  function descriptor_def()
  {
    return "[status_name]";
  }
}



?>