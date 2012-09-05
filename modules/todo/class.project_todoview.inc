<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage todo
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atktextattribute");
useattrib("atkdateattribute");
useattrib("atknumberattribute");

/**
 * Module file for the menu items and security
 *
 * @package achievo
 * @subpackage todo
 */
class project_todoview extends atkNode
{
  /**
   * Constructor
   *
   * @return project_todoview
   */
  function project_todoview()
  {
    global $ATK_VARS;

    $this->atkNode("project_todoview",NF_NO_ADD|NF_NO_DELETE);

    $this->add(new atkNumberAttribute("id"                                , AF_AUTOKEY));
    $this->add(new atkAttribute("name"                              , AF_READONLY));
    //$this->add(new atkManyToOneRelation("customer","organization.organization"       , AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("coordinator","employee.employee", AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkTextAttribute("description"                   , TEXT_LARGE, AF_READONLY));
    $this->add(new atkDateAttribute("startdate","","",0,0 , AF_READONLY|AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("todo","todo.todo","projectid"  , AF_HIDE_LIST));

    $this->addFilter("project.status","active");
    $this->setTable("project","project");
    $this->setOrder("name");
    $this->setSecurityAlias("todo.todo");
    $this->addSecurityMap("edit","add");


    atkdebug("project::project()");
  }

  /**
   * set descriptor template
   *
   * @return string
   */
  function descriptor_def()
  {
    return "[name]";
  }

  /**
   * Check for the any project right, if user doesn't have the right
   * add a filter to the node
   *
   * @param object $handler
   * @return string
   */
  function action_admin(&$handler)
  {
    // Access control
    if (!$this->allowed("any_project"))
    {
      $user = getUser();
      $this->addFilter("project.coordinator=".$user["id"]);
    }
    return $handler->action_admin();
  }

}

?>
