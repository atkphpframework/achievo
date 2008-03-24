<?php
  /**
   * documenttype node class file
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

  // Load used classes
  useattrib("docmanager.documentfileattribute");
  useattrib("atklistattribute");

  /**
   * The node class for documenttype.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class documenttype extends atkNode
  {
    /**
     * documenttype constructor
     */
    function documenttype()
    {
      $this->atkNode("documenttype");
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY));
      $this->add(new documentFileAttribute("template", atkconfig::get("docmanager","docmanager_doctypetemplatepath", moduledir("docmanager") . "doctypetemplates/"), atkconfig::get("docmanager","docmanager_localdocumenttypepath"), false, AF_OBLIGATORY));

      $nodes = atkconfig::get("docmanager","docmanagernodes", array());
      $nodes[] = "project.project_personcontact";
      $nodetranslations = array();
      foreach ($nodes as $node)
      {
        $nodetranslations[] = atktext(getNodeType($node), getNodeModule($node));
      }
      $this->add(new atkListAttribute("master", $nodetranslations, $nodes, AF_OBLIGATORY|AF_NO_TRANSLATION|AF_FILE_NO_SELECT, 255));

      $this->setTable("docmanager_documenttype");
    }

    /**
     * Return record descriptor (for use in relations etc.)
     *
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      return "[name]";
    }
  }
?>