<?php

include_once "class.tpl_dependencyattrib.inc";
    
class tpl_project extends atkNode
{
  function tpl_project()
  {
    $this->atkNode("tpl_project",NF_EDITAFTERADD);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_UNIQUE|AF_SEARCHABLE));
    $this->addAttribute(new atkTextAttribute("description",AF_HIDE_ADD));
    $this->addAttribute(new atkOneToManyRelation("tpl_project_phase","tpl_project_phase","projectid", AF_HIDE_ADD|AF_HIDE_LIST|AF_NOSORT));
    $this->addAttribute(new tpl_dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL));

    $this->setTable("tpl_project","tpl_project");
    $this->setOrder("name");

    atkdebug("tpl_project::tpl_project()");
  }

  function descriptor_def()
  {
    return "[name]";
  }
  
  function initial_values()
  {
    return array("status"=>"active");
  }
}
?>