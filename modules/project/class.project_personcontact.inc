<?php

/**
 * Module Project
 * Class project_personcontact
 *
 * This class makes it possible to relate contacts to a project in a certain role(s).
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

userelation("atkmanytoonerelation");

class project_personcontact extends atkNode
{
  function project_personcontact()
  {
    $this->atkNode("project_personcontact");

    $this->add(new atkManyToOneRelation("projectid", "project.project", AF_OBLIGATORY|AF_PRIMARY|AF_HIDE));
    $this->add(new atkManyToOneRelation("personid", "project.select_contact", AF_OBLIGATORY|AF_PRIMARY|AF_LARGE));
    $this->add(new atkManyToOneRelation("role", "project.role", AF_OBLIGATORY|AF_PRIMARY|AF_RELATION_AUTOLINK));
    $this->addFilter("personid.role = 'contact'");

    $this->setTable("project_person", "project_person");
  }

  function recordActions($record, &$actions)
  {
    if (!$this->hasFlag(NF_NO_VIEW)) $actions["view"] = session_url(dispatch_url("project.project", "view", array("atkselector" => "project.id=".$record["projectid"]["id"])),SESSION_NESTED);
  }
}

?>