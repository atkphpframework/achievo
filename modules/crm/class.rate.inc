<?php

class rate extends atkNode
{

  function rate()
  {
    $this->atkNode("rate",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("rate_name",AF_OBLIGATORY));

    $this->setSecurityAlias("organization.organization");
    $this->setOrder("rate_name");
    $this->setTable("crm_rate");
  }

  function descriptor_def()
  {
    return "[rate_name]";
  }
}



?>