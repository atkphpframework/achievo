<?php

/**
 * Module Organization
 * Class role
 *
 * This class is used to define roles for organizations.
 * At this moment it is not possible to add, edit and delete roles, we only use the roles that are in the database.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

class role extends atkNode
{
  function role()
  {
    $this->atkNode("role");

    $this->add(new atkAttribute("id", AF_PRIMARY));
    $this->add(new atkAttribute("name"));

    $this->setTable("role", "role");
    $this->setOrder("name");
  }

  function descriptor_def()
  {
    return text("[name]");
  }

  function descriptor($record)
  {
    return text($record["name"]);
  }
}

?>