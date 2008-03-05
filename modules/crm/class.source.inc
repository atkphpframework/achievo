<?php

class source extends atkNode
{

  function source()
  {
    $this->atkNode("source",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("source_name",AF_OBLIGATORY));

    $this->setSecurityAlias("organization.organization");
    $this->setOrder("source_name");
    $this->setTable("crm_source");
  }

  function descriptor_def()
  {
    return "[source_name]";
  }
}



?>