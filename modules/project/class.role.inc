<?php

/**
 * Module Organization
 * Class role
 *
 * This class is used to define roles for persons.
 * At this moment it is not possible to delete roles.
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
    $this->atkNode("role", NF_NO_DELETE);

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