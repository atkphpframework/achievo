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
// Use the atkdummyattribute to extend from
useattrib("atkdummyattribute");

/**
 * relatedDocument class for generating documents and sending them to a related node's documentmanager
 *
 * @author guido <guido@ibuildings.nl>
 * @package achievo_modules
 * @subpackage docmanager
 */
class relatedDocument extends atkDummyAttribute
{
    /**
     * @internal Name of the manytoone relation which is targeted when generating documents
     */
    var $m_relationname;

    /**
     * relatedDocument constructor
     *
     * @param string $name description
     * @param integer $flags description
     */
    function relatedDocument($name, $relationname, $flags = 0)
    {
        // Call parent constructor with addition of the af_hide_add flag
        // because this attribute should not be used in add operations where
        // the recorddata is not yet present in the database.
        $this->atkDummyAttribute($name, "", $flags | AF_HIDE_ADD | AF_DUMMY_SHOW_LABEL);

        $this->m_relationname = $relationname;
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param array $record The record that holds the value for this attribute.
     * @return string HTML code for selection control
     */
    function edit($record)
    {
        return $this->documentSelector($record);
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $mode Name of the mode in which the attribute should be displayed
     * @return string description
     */
    function display($record, $mode)
    {
        if (($mode != "csv") && ($mode != "plain"))
            return $this->documentSelector($record, $mode);
        else
            return "";
    }

    /**
     * Gets a list of document types that can be offered for generation using the given record
     *
     * @param Array $record Record for which you want to know what document types can be used
     * @return Array Associative array containing documenttypeid=>name pairs
     */
    function getDocumentTypes($record = null)
    {
        // Get a reference to the documenttype node
        $documenttypenode = &getNode("docmanager.documenttype");

        // Select all document types belonging to the owner module
        $documenttypes = $documenttypenode->selectDb("master='" . $this->m_ownerInstance->m_module . "." . $this->m_ownerInstance->m_type . "'");

        // Collect all documentids/names in one associative array
        $documenttypearray = array();
        foreach ($documenttypes as $documenttype) {
            $documenttypearray[$documenttype["id"]] = $documenttype["name"];
        }

        // Return the list of document types
        return $documenttypearray;
    }

    /**
     * Gets the display code for the document selector in the specified mode
     *
     * @param string $mode Name of the mode in which the document selector will be displayed
     * @return string Display code for this attribute in the specified mode
     */
    function documentSelector($record, $mode = "")
    {
        // Get the relation
        $relation = &$this->m_ownerInstance->getAttribute($this->m_relationname);

        // Return nothing if relation not found
        if (is_null($relation))
            return "";

        // Get the related node
        $relatednode = $relation->m_destination;

        // Get the list of document files
        $documenttypes = $this->getDocumentTypes($record);

        if (empty($documenttypes)) {
            return $this->text("no_document_templates_found");
        }

        // Button counter
        static $cnt = 0;
        $buttoncounter = ++$cnt;

        // Add the select box to the html
        $html = '<select name="doctype' . $buttoncounter . '">';
        foreach ($documenttypes as $id => $name) {
            $html .= '<option value="' . urlencode($id) . '">' . $name . '</option>';
        }
        $html .= '</select>';

        // Add the button to the html
        $selector = $this->m_ownerInstance->primaryKey($record);
        $linkparams = array(
            "destinationmaster" => $relatednode,
            "destinationmaster_id" => $record[$this->m_relationname]["id"],
            "atkselector" => $selector,
        );
        $onclickscript = 'window.location="' . session_url(dispatch_url("docmanager.document", "generate", $linkparams), SESSION_NESTED) . 'documenttypeid="+this.form.doctype' . $buttoncounter . '.value;';
        $html .= ' <input type="button" class="atkbtn' . $buttoncounter . '" name="atkgenerate" value="' . atktext("generate", "docmanager") . '" onclick=\'' . $onclickscript . '\'>';

        // Return the generated html
        return $html;
    }

}

?>