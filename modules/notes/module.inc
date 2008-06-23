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


  /**
   * Module file for the menu items and security
   *
   * @package achievo
   * @subpackage notes
   */
  class mod_notes extends achievoModule
  {
    function getNodes()
    {
      registerNode("notes.project_notes", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("-","","projectmanagement");
      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : 0;
      if ($selectedproject == 0)
      {
        $this->menuitem("project_notesview", dispatch_url("notes.project_notesview", "admin"), "projectmanagement", array("notes.project_notes", "admin"));
      }
      else
      {
        $this->menuitem("project_notesview", dispatch_url("notes.project_notesview", "edit", array("atkselector" => "project.id=".$selectedproject)), "projectmanagement", array("notes.project_notes", "admin"));
      }
    }

    /**
     * Set dependencies for this module
     *
     * @param string $mode Mode
     * @return array Array with dependencies
     */
    function getDependencies($mode)
    {
      return array("project");
    }
  }

?>
