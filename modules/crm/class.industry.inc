<?php

class industry extends atkNode
{

  function industry()
  {
    $this->atkNode("industry",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("industry_name",AF_OBLIGATORY));

    $this->setSecurityAlias("organization.organization");
    $this->setOrder("industry_name");
    $this->setTable("crm_industry");
  }

  function descriptor_def()
  {
    return "[industry_name]";
  }
}



?>