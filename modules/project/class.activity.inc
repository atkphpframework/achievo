<?php

useattrib("atkboolattribute");

class activity extends atkNode
{
  function activity()
  {
    $this->atkNode("activity", NF_MRA);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE, 50));
    $this->add(new atkAttribute("description",AF_SEARCHABLE, 50));
    $this->add(new atkBoolAttribute("remarkrequired"));

    $this->setTable("activity","activity");
    $this->setOrder("name");

    atkdebug("activity::activity()");
  }

  function descriptor_def()
  {
    return "[name]";
  }

  function action_stats()
  {
    include_once moduleDir("project")."activitystats.inc";
  }
}
?>