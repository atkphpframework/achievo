<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage notes
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");

/**
 * Class for managing project notes
 * @package achievo
 * @subpackage notes
 */
class project_notes extends atkNode
{
  /**
   * Constructor
   */
  function project_notes()
  {
    $this->atkNode("project_notes");

    $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_HIDE_ADD|AF_READONLY|AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("projectid", "project.project",AF_OBLIGATORY|AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkAttribute("title", AF_OBLIGATORY|AF_SEARCHABLE, 50));
    $this->add(new atkDateAttribute("entrydate", "","", date("Ymd"), 0, AF_HIDE_EDIT|AF_HIDE_ADD|AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description" , TEXT_LARGE, AF_OBLIGATORY|AF_HIDE_LIST));
    $this->setTable("project_notes");
    $this->setOrder("entrydate");

  }

  /**
   * Set initial values
   * @return array Array with initial values
   */
  function initial_values()
  {
    global $g_user;
    return array("owner"=>array("id"=>$g_user["id"]),
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
  }

  /**
   * Set descriptor template
   * @return string
   */
  function descriptor_def()
  {
    return "[title]";
  }
}
?>
