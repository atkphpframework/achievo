<?php

useattrib("atktextattribute");
userelation("atkmanyboolrelation");

class tpl_phase extends atkNode
{
  function tpl_phase()
  {
    $this->atkNode("tpl_phase",NF_EDITAFTERADD);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE, 50));
    $this->add(new atkTextAttribute("description",AF_HIDE_ADD));

    $tmp = new atkManyBoolRelation("tpl_phase_activity", "project.tpl_phase_activity", "project.activity",AF_HIDE_LIST|AF_HIDE_ADD);
    $tmp->m_localKey = "phaseid";
    $tmp->m_remoteKey = "activityid";
    $this->add($tmp);

    $this->setOrder("name");
    $this->setTable("tpl_phase","tpl_phase");
  }

  function descriptor_def()
  {
    return "[name]";
  }
}
?>