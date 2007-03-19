<?php
  /**
   * projectdocumentmanager attribute class file
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

  /**
   * Load the base attribute
   */
  useattrib("docmanager.documentmanager");

  /**
   * The class for projectdocumentmanager attribute.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class projectDocumentManager extends documentManager
  {

    /**
     * Returns the document types to be offered for generation by this attribute
     *
     * Full description of the function
     *
     * @param type name description
     * @return type description
     */
    function getDocumentTypes($record)
    {
      // Read the template used to generate the project
      $template = $record["template"]["id"];

      // If the template is empty (aka unkown), then offer no types
      if ($template == "")
      {
        return parent::getDocumentTypes($record);
      }

      // Get the document types by querying the projectpl_doctype relation
      $projecttpl_doctypenode = &getNode("docmanager.projecttpl_doctype");
      $projecttpl_doctypes = $projecttpl_doctypenode->selectDb("projecttpl_id='$template'");

      // Create an associative array containing $id=>$name items for each documenttype
      $documenttypearray = array();
      foreach ($projecttpl_doctypes as $projecttpl_doctype)
      {
        $documenttypearray[$projecttpl_doctype["documenttype_id"]["id"]] = $projecttpl_doctype["documenttype_id"]["name"];
      }

      // Return the list of document types
      return $documenttypearray;
    }

  }
?>
