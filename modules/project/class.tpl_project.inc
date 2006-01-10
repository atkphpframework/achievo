<?php

useattrib("project.tpl_dependencyattrib");
useattrib("atktextattribute");
userelation("atkonetomanyrelation");

class tpl_project extends atkNode
{
  function tpl_project()
  {
    $this->atkNode("tpl_project",NF_EDITAFTERADD);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_UNIQUE|AF_SEARCHABLE|AF_OBLIGATORY, 50));
    $this->add(new atkTextAttribute("description",TEXT_LARGE, AF_HIDE_ADD));
    $this->add(new atkOneToManyRelation("tpl_project_phase","project.tpl_project_phase","projectid", AF_HIDE_ADD|AF_HIDE_LIST|AF_NOSORT));
    $this->add(new tpl_dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL));

    $this->setTable("tpl_project","tpl_project");
    $this->setOrder("name");
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
