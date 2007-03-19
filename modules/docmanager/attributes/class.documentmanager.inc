<?php

  userelation("atkonetomanyrelation");

  /**
   * Generic document manager attribute.
   * Creates a onetomanyrelation dynamically by using the module.node of
   * the owning node as a filter and a generic foreign key, to be able
   * to store documents for any node.
   */
  class documentManager extends atkOneToManyRelation
  {

    function documentManager($name, $flags=0)
    {
      $this->atkOneToManyRelation($name,"docmanager.document", "master_id", $flags);
    }

    function init()
    {
      if ($this->m_destinationFilter != "")
        $this->m_destinationFilter .= " AND ";
      $this->m_destinationFilter .= "master='".$this->m_ownerInstance->atkNodeType()."'";
      return parent::init();
    }

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