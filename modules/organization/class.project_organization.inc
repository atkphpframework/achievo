<?php

/**
 * Module Organization
 * Class project_organization
 *
 * This class makes it possible to relate organizations to a project in a certain role(s).
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

userelation("atkmanytoonerelation");

class project_organization extends atkNode
{
  function project_organization()
  {
    $this->atkNode("project_organization");

    $this->add(new atkManyToOneRelation("projectid", "project.project", AF_OBLIGATORY|AF_PRIMARY));
    $this->add(new atkManyToOneRelation("organizationid", "organization.organization", AF_OBLIGATORY|AF_PRIMARY));
    $this->add(new atkManyToOneRelation("role", "organization.role", AF_OBLIGATORY|AF_PRIMARY));
    $this->add(new atkManyToOneRelation("contact", "organization.contact", AF_OBLIGATORY));

    $this->setTable("project_organization", "project_organization");
    $this->setSecurityAlias("project.project");
  }

  function recordActions($record, &$actions)
  {
    if (!$this->hasFlag(NF_NO_VIEW)) $actions["view"] = session_url(dispatch_url("project.project", "view", array("atkselector" => "project.id=".$record["projectid"]["id"])),SESSION_NESTED);
  }
}

?>