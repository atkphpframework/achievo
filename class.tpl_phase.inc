<?php

class tpl_phase extends atkNode
{
  function tpl_phase()
  {
    $this->atkNode("tpl_phase",NF_EDITAFTERADD);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkTextAttribute("description",AF_HIDE_ADD));
    
    $tmp = new atkManyBoolRelation("tpl_phase_activity","activity",AF_HIDE_LIST|AF_HIDE_ADD);
    $tmp->m_localKey = "phaseid";
    $tmp->m_remoteKey = "activityid";
    $this->addAttribute($tmp);      

    $this->setOrder("name");
    $this->setTable("tpl_phase","tpl_phase");
  }
  
  function descriptor_def()
  {
    return "[name]";
  }
}
?>