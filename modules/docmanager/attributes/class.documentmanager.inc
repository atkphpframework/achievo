<?php
  /**
   * This file is part of the Achievo distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   * projecttpl_doctype node class file
   *
   * @package achievo
   * @subpackage docmanager
   *
   * @copyright (c)2008 Ibuildings B.V.
   * @license http://www.achievo.org/licensing Achievo Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  userelation("atkonetomanyrelation");

  /**
   * Generic document manager attribute.
   * Creates a onetomanyrelation dynamically by using the module.node of
   * the owning node as a filter and a generic foreign key, to be able
   * to store documents for any node.
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class documentManager extends atkOneToManyRelation
  {

    /**
     * Constructor
     *
     * @param string $name Attribute name
     * @param int $flags Flags
     */
    function documentManager($name, $flags=0)
    {
      $this->atkOneToManyRelation($name,"docmanager.document", "master_id", $flags);
    }

    /**
     * Init function to set a destination filter
     *
     * @return boolean
     */
    function init()
    {
      if ($this->m_destinationFilter != "")
        $this->m_destinationFilter .= " AND ";
      $this->m_destinationFilter .= "master='".$this->m_ownerInstance->atkNodeType()."'";
      return parent::init();
    }

    /**
     * Get document types
     *
     * @param array $record Record
     * @return array
     */
    function getDocumentTypes($record = null)
    {
      // Get a reference to the documenttype node
      $documenttypenode = &getNode("docmanager.documenttype");

      // Select all document types belonging to the owner module
      $documenttypes = $documenttypenode->selectDb("master='" . $this->m_ownerInstance->m_module . "." . $this->m_ownerInstance->m_type . "'");

      // Collect all documentids/names in one associative array
      $documenttypearray = array();
      foreach ($documenttypes as $documenttype)
      {
        $documenttypearray[$documenttype["id"]] = $documenttype["name"];
      }

      // Return the list of document types
      return $documenttypearray;
    }

    /**
     * Retrieve header for the recordlist
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail relationship.
     * @return String a String to be added to the header of the recordlist.
     */
    function editFooter($record=NULL, $childrecords=NULL)
    {
      // Set output initial empty
      $output = "";

      // Get the document types to be offered for generation
      $documenttypes = $this->getDocumentTypes($record);

      // Loop through all document types
      foreach ($documenttypes as $documenttypeid => $documenttypename)
      {

        // Compose the parameters to be passed to the generate action when clicking the link
        $linkparams = array(
                        "documenttypeid" => $documenttypeid,
                        "master_id" => $record["id"],
                        "atkselector" => $record["atkprimkey"],
                        "atkfilter" => $this->m_destinationFilter,
                      );

        // Compose the link caption in "Generate <documenttypename>" format
        $linkname = atktext("generate", "docmanager") . " " . $documenttypename;

        // Add a link for the current document
        $output .= href(session_url(dispatch_url("docmanager.document", "generate", $linkparams), SESSION_NESTED), $linkname) . "&nbsp;&nbsp;";
      }

      // Returen generated links
      return $output;
    }
  }

?>