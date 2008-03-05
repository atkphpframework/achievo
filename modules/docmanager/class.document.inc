<?php
  /**
   * Node file for the document class
   *
   * @package achievo_modules
   * @subpackage docmanager
   *
   * @author lineke <lineke@ibuildings.nl>
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  // Load used files and attribute classes
  atkimport("atk.document.atkdocumentwriter");
  useattrib("docmanager.documentfileattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("atkboolattribute");
  useattrib("atknumberattribute");

  /**
   * Document class
   *
   * @author lineke <lineke@ibuildings.nl>
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class document extends atkNode
  {

    /**
     * Constructor
     */
    function document()
    {
      // Call the parent constructor, passing the nodetype
      $this->atkNode("document");

      // Add the attributes used by this node
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("documentcode", AF_UNIQUE));
      $this->add(new atkAttribute("name", AF_OBLIGATORY));
      $this->add(new documentFileAttribute("filename",atkconfig("documentpath", "documents/"),atkconfig("docmanager_localdocumentpath"),true,AF_FILE_POPUP|AF_OBLIGATORY));
      $this->add(new atkAttribute("owner",AF_READONLY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkAttribute("master", AF_HIDE|AF_FORCE_LOAD));
      $this->add(new atkNumberAttribute("master_id", AF_HIDE|AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("entrydate",AF_READONLY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkAttribute("version"));
      $this->add(new atkListAttribute("status", array("concept", "internal", "accepted")));
      $this->add(new atkBoolAttribute("confidential"));
      $this->add(new atkBoolAttribute("internal"));

      // Set the database table name
      $this->setTable("docmanager_document");

      // Set the sorting order for use in list views
      $this->setOrder("docmanager_document.entrydate DESC");
    }

    /**
     * Returns the descriptor template
     *
     * @return String Template definition which can be used when composing the descriptor for a record
     */
    function descriptor_def()
    {
      return "[name]";
    }

    /**
     * Defines initial values used when adding a new document
     *
     * @return array Fieldname=>Value pairs describing the initial values for several fields
     */
    function initial_values()
    {
      // Return an array containing the default values used when adding a new document
      return array(
               "documentcode" => $this->getInitialDocumentcode(),
               "owner"=>atkArrayNvl(getUser(), "name"),
               "entrydate"=>array("year"=>date("Y"), "month"=>date("m"), "day"=>date("d"))
             );
    }

    function getInitialDocumentcode()
    {
      // Document initial filename generator module
      $documentnumbermodulename = atkConfig::get("docmanager", "documentnumbermodule", "");
      if (!empty($documentnumbermodulename))
      {
        atkdebug("Getting alternative documentcode from the '$documentnumbermodulename' module.");
        $documentnumbermodule = &atkGetModule($documentnumbermodulename);
        return $documentnumbermodule->getInitialDocumentcode($doctypetemplate);
      }

      atkdebug("Using default empty projectcode.");
      return "";
    }

    /**
     * Constructs a filename for a generated file
     *
     * @param String $basefilename Original filename used to build the final filename
     * @return String Constructed filename
     */
    function getFilenameForGeneratedFile($doctypetemplate, $document)
    {
      // Construct a stringparser to parse the template and construct the filename
      atkimport("atk.utils.atkstringparser");
      $stringparser = new atkStringParser(atkConfig::get("docmanager", "documentfilenametemplate", "[year]-[month]-[day] [doctypetemplate.template.filename]"));

      // Construct the data to be used in the determination of the filename
      $data = array(
        "day"=>date("d"),
        "month"=>date("m"),
        "year"=>date("Y"),
        "fileextension"=>$this->getFileExtension($doctypetemplate["template"]["filename"]),
        "doctypetemplate"=>$doctypetemplate,
        "document"=>$document,
      );

      // Parse the template and return the data
      return $stringparser->parse($data);
    }

    function getFileExtension($filename)
    {
      $parts = explode(".", $filename);
      return (count($parts) > 0) ? $parts[count($parts)-1] : "";
    }

    /**
     * Generates the selected document and adds it in the docmanager for the specified node
     *
     * @param type name description
     * @return type description
     * @todo NEEDS MORE DOCUMENTATION AND CODE REVIEW!!!
     */
    function action_generate(&$handler)
    {
      if (isset($this->m_postvars["atkcancel"]))
      {
        $this->redirect();
        return $handler;
      }

      // Raise an error if the documenttypeid is not passed
      if (!isset($this->m_postvars["documenttypeid"]) || $this->m_postvars["documenttypeid"]=='')
      {
        atkerror("No documenttypeid present");
      }

      // Raise an error if the master_id is not passed
      if (!isset($this->m_postvars["atkselector"]))
      {
        atkerror("No selector present");
      }

      // Get the documenttypeid
      $documenttypeid = $this->m_postvars["documenttypeid"];

      // Select the documenttype form the database
      $documenttypenode = &getNode("docmanager.documenttype");
      $documenttypes = $documenttypenode->selectDb("id='$documenttypeid'");

      // Raise an error if the documenttype specified using the documenttypeid doesn't exist
      if (count($documenttypes) == 0)
      {
        atkerror("Specified documenttype does not exist");
      }
      $documenttype = $documenttypes[0];

      // Determine the destination to document manager to which the parsed document should be added
      $destinationmaster = isset($this->m_postvars["destinationmaster"]) ? $this->m_postvars["destinationmaster"] : $documenttype["master"];
      $destinationmaster_id = isset($this->m_postvars["destinationmaster_id"]) ? $this->m_postvars["destinationmaster_id"] : $this->m_postvars["master_id"];

      // Prepare the record for adding it to the database
      $record = $this->initial_values();
      $record["name"] = $documenttype["name"];
      $record["master"] = $destinationmaster;
      $record["master_id"] = $destinationmaster_id;
      $record = array_merge($record, decodeKeyValueSet($this->m_postvars["atkfilter"]));

      $tempfilename = isset($this->m_postvars["filename"]) ? $this->m_postvars["filename"] : $this->getFilenameForGeneratedFile($documenttype, $record);

      if (((!isset($this->m_postvars["filename"])) && (atkConfig::get("docmanager", "documentgenerateaskfilename", true))) || (file_exists(atkconfig("documentpath", "documents/") . $tempfilename)))
      {

        $theme = &atkTheme::getInstance();
        $reqimg = '<img align="top" onMouseOver="javascript:window.status=\''.
                     addslashes(atktext("field_obligatory")).'\';" src="'.$theme->imgPath("required_field.gif").'" border="0" alt="'.
                     atktext("field_obligatory").'">';



        // Set some fixed parameters to be passed through the filename dialog
        $params = array(
          "destinationmaster"=>isset($this->m_postvars["destinationmaster"]) ? $this->m_postvars["destinationmaster"] : $documenttype["master"],
          "destinationmaster_id"=>isset($this->m_postvars["destinationmaster_id"]) ? $this->m_postvars["destinationmaster_id"] : $this->m_postvars["master_id"],
          "atkselector"=>$this->m_postvars["atkselector"],
          "documenttypeid"=>$this->m_postvars["documenttypeid"],
          "master_id"=>$this->m_postvars["master_id"],
          "atkfilter"=>$this->m_postvars["atkfilter"],
        );

        // Show filename dialog
        $content = '<form action="'. dispatch_url("docmanager.document", "generate", $params) . '" method="post">';
        $content.= session_form();
        $content.= '<table name="entryform" id="entryform" cellspacing="0">';
        $content.= '<tr><td colspan="2" class="fieldlabel"><b>' . atktext("give_new_filename", "docmanager") . '</b></td></tr>';
        if ((isset($this->m_postvars["filename"])) && (file_exists(atkconfig("documentpath", "documents/") . $this->m_postvars["filename"])))
        {
          $content.= '<tr><td colspan="2" style="color: #ee0000" class="fieldlabel">' . atktext("filename_already_in_use", "docmanager") . '</td></tr>';
        }
        $content.= '<tr><td colspan="2" class="fieldlabel">&nbsp;</td></tr>';
        $content.= '<tr valign="top">';
        $content.= '<td class="fieldlabel">' . atktext("filename", "docmanager") . ' ' . $reqimg . '</td>';
        $content.= '<td><input id="filename" name="filename" value="' . $tempfilename . '"></td>';
        $content.= '</table>';
        $content.= '<br>';
        $content.= '<input type="submit" class="btn_save" name="atknoclose" value="'.atktext("save", "atk").'">&nbsp;&nbsp;';
        $content.= '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel", "atk").'">';
        $content.= '</form>';
        $content.= '<br>';

        $title = atktext("generate", "docmanager") . " " . $documenttype["name"];

        $ui = $this->getUi();
        $boxedcontent = $ui->renderbox(array("title"=>$title,"content"=>$content));

        $page = &$this->getPage();
        $page->register_style($theme->stylePath("style.css"));
        $page->addContent($boxedcontent);
        return $handler;
      }

      // Determine the location of the template and the destination document
      $from = $documenttype["template"]["tmpfile"];
      $filenameattribute = &$this->getAttribute("filename");
      $to = $filenameattribute->customdir($record, atkconfig("documentpath", $filenameattribute->defaultdir($record, "dir"))) . $tempfilename;

      // Initialize the filename attribute value
      $record["filename"] = $documenttype["template"];
      $record["filename"]["tmpfile"] = $to;
      $record["filename"]["orgfilename"] = $tempfilename;
      $record["filename"]["filename"] = $tempfilename;

      // Ask if other modules have added columns to the documenttype that we should copy to the document
      $copycolumns = atkHarvestModules("getDocmanagerCopyColumns", $record);
      foreach ($copycolumns as $copycolumn)
        if (!isset($record[$copycolumn]))
          $record[$copycolumn] = $documenttype[$copycolumn];

      // If the template document shouldn't be parsed using the documentwriter,
      // then just copy it
      if (atkConfig("docmanager_dontusedocumentwriter", false))
      {

        // Just copy the template document
        if (!copy($from, $to))
        {
          atkerror("Error while copying document");
        }

      }

      // Else parse it using the documentwriter
      else
      {

        // Get a reference to the opendocumentwriter
        $documentwriter = atkdocumentwriter::getInstance("opendocument");

        $masternode = &getNode($documenttype["master"]);

        // Get a reference to the document handler and set the references
        // to the master node
        $documenthandler = &atkActionHandler::getDefaultHandler("document");
        $documenthandler->m_module = getNodeModule($documenttype["master"]);
        $documenthandler->m_type = getNodeType($documenttype["master"]);
        $documenthandler->m_node = &$masternode;

        // Assign the record variables to the OpenOffice.org DocumentWriter
        if (method_exists($masternode, "assignDocumentVars"))
          $masternode->assignDocumentVars($documentwriter, $this->m_postvars["atkselector"]);
        else
          $documenthandler->assignDocumentVars($documentwriter, $this->m_postvars["atkselector"]);

        // Try to parse and store the template document and raise an atkerror in case of failure
        if (!$documentwriter->store($from, $to))
        {
          atkerror("Error while generating document");
        }

      }

      // Add the new document record to the database
      $this->addDb($record);

      // Go back to the previous page
      $this->redirect();
    }

  }

?>
