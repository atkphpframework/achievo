<?php
  /**
   * Docmanager module file
   *
   * @package achievo_modules
   * @subpackage docmanager
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  // Add the docmanager to any modules specified in the $config_docmanagernodes array
  $modifiers = array_values(array_diff(array_merge($modifiers, atkconfig("docmanagernodes", array())), array("project.project")));

  /**
   * docmanager module class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class mod_docmanager extends achievoModule
  {

    /**
     * Sets the nodes contained within this module and the actions a user could perform on them
     */
    function getNodes()
    {
      registerNode("docmanager.document", array("add","edit","delete","generate"));
      registerNode("docmanager.documenttype", array("admin","add","edit","delete"));
    }

    /**
     * Adds menu items to the menu when loading the menu
     */
    function getMenuItems()
    {
      $this->menuitem("document_types", dispatch_url("docmanager.documenttype", "admin"), "setup", array("docmanager.documenttype", "admin"));
    }

    /**
     * Generic modifier for nodes that want to include the documentManager attribute
     *
     * @param atkNode &$node Node
     */
    function addDocumentManager(&$node)
    {
      useattrib("docmanager.documentmanager");
      $node->add(new documentManager("documents", AF_HIDE_LIST|AF_HIDE_SEARCH), "documents");
    }

    function getModifierFunctions(&$node)
    {
      $modifierfunctions = parent::getModifierFunctions($node);
      $docmanagernodes = atkconfig("docmanagernodes", array());
      if (in_array($node->atknodetype(), $docmanagernodes) && ($node->atknodetype() != "project.project"))
        $modifierfunctions[] = "addDocumentManager";
      return $modifierfunctions;
    }
  }
?>