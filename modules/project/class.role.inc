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
    $this->atkNode("role");

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkAttribute("name"));

    $this->setTable("role");
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
  
  function recordActions($record, &$actions)
  {
    global $g_db;
    $role_id = $record["id"];
    // Check if role is being used.
    $recs = $g_db->getrows("select count(1) as cnt from project_person where role='".$role_id."'");
    if ($recs[0]["cnt"]>0)
    {
      unset($actions["delete"]);
    }
  }
}

?>