<?php

class campaign_status extends atkNode
{

  function campaign_status()
  {
    $this->atkNode("campaign_status",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("status_name",AF_OBLIGATORY));

    $this->setOrder("status_name");
    $this->setTable("crm_campaign_status");
  }

  function descriptor_def()
  {
    return "[status_name]";
  }
}



?>