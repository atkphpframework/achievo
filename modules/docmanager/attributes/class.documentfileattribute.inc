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

  useattrib("atkfileattribute");

  /**
   * The class for documentFileAttribute attribute.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class documentFileAttribute extends atkFileAttribute
  {

    /**
     * @internal Backup variable for the originally used dir when constructing this attribute
     */
    var $m_originaldir;

    /**
     * @internal Backup variable for the originally used url when constructing this attribute
     */
    var $m_originalurl;

    /**
     * @internal Local path that can optionally be used to make an attribute available through file:// links
     */
    var $m_localpath;

    /**
     * @internal Allow external modules to specify different paths
     */
    var $m_allowpathcustomization;

    /**
     * Constructor
     *
     * @param array $name Attribute name
     * @param string $dir Directory
     * @param string $localpath Local path
     * @param boolean $allowpathcustomization Allow path customization
     * @param int $flags Flags
     * @param int $size Field size
     */
    function documentFileAttribute($name, $dir, $localpath="", $allowpathcustomization=false, $flags=0, $size=0)
    {
      // Call the parent constructor
      $this->atkFileAttribute($name, $dir);

      // Save the set dir, to be able to revert to it as the m_dir member
      // will change frequently inside this attribute.
      $this->m_originaldir = $dir;
      $this->m_originalurl = $this->m_url;
      $this->m_localpath = $localpath;
      $this->m_allowpathcustomization = $allowpathcustomization;
      $this->setAllowedFileTypes(atkconfig::get('docmanager','docmanager_allowedfiletypes'));
    }

    /**
     * Default directory
     *
     * @param array $rec Record
     * @param string $type "dir" or "url"
     * @return string
     */
    function defaultdir($rec, $type)
    {
      $options = atkconfig::get("docmanager","docmanageroptions", array());
      $result = "";
      // If localpath set, prefix it with file://, if not, use the default m_url
      if ($this->m_localpath != "")
        $path = "file://" . $this->m_localpath;
      else
        $path = $this->m_originalurl;
      if (in_array($rec["master"], array_keys($options)))
      {
      	if ($type == "dir") $result = atkArrayNvl($options[$rec["master"]], "dir", $this->m_originaldir);
      	if ($type == "url") $result = atkArrayNvl($options[$rec["master"]], "url", $path);
      }
      if (empty($result))
      {
      	if ($type == "dir") $result = $this->m_originaldir;
      	if ($type == "url") $result = $path;
      }
      atk_var_dump($result,"DOC ATTRIBUTE RESULT");
      return $result;
    }


    /**
     * Get custom dir
     *
     * @param array $rec
     * @param string $defaultdir Default directory
     * @return string
     */
    function customdir($rec, $defaultdir)
    {
      if (!$this->m_allowpathcustomization)
        return $defaultdir;
      // Load a list of all modules
      global $g_modules;
      $modulekeys = array_keys($g_modules);

      // Loop through the modules and seek for a getCustomDocumentDir function,
      // If this function is found, call it and return the return value
      foreach ($modulekeys as $modname)
      {
        $module = &getModule($modname);
        if (is_object($module) && method_exists($module, "getCustomDocumentDir"))
        {
          return $module->getCustomDocumentDir($rec, $defaultdir);
        }
      }

      // No getCustomDocumentDir function was found, return the defaultdir
      return $defaultdir;
    }

    /**
     * Override edit
     *
     * @param array $record Record
     * @param string $fieldprefix Field prefix
     * @param string $mode Mode
     * @return string
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      if($mode=="add")
      {
        // Let the customdir function determine the right directory using
        // the record and the determined path and store it to m_url
        $this->m_url = $this->customdir($record, $this->defaultdir($record, "url"));

        // Let the customdir function set the storage dir for the documentfile
        $this->m_dir = $this->customdir($record, $this->defaultdir($record, "dir"));
        atkdebug("DIR IN ADD MODE: ".$this->m_dir." | DEFAULT: ".$this->defaultdir($record, "dir"));
      }
      return parent::edit($record, $fieldprefix, $mode);
    }

    /**
     * Display override
     *
     * @param array $record Record
     * @return string
     */
    function display(&$record)
    {
      // Let the customdir function determine the right directory using
      // the record and the determined path and store it to m_url
      $this->m_url = $this->customdir($record, $this->defaultdir($record, "url"));

      // Let the customdir function set the storage dir for the documentfile
      $this->m_dir = $this->customdir($record, $this->defaultdir($record, "dir"));

      // Determine the filename
      $filename = $record[$this->fieldName()]["orgfilename"];

      // If no filename available, show nothing
      if ($filename == "")
      {
        return "";
      }

      // If the file is found, show a link
      else if (is_file($this->m_dir.$filename))
      {
        return "<a href=\"".$this->m_url."$filename\">$filename</a>";
      }

      // Else display a "file not found" message
      else
      {
        return $filename."(<font color=\"#ff0000\">".atktext("file_not_exist", "atk")."</font>)";
      }
    }

    /**
     * value to database, use custom dir to set the storage dir
     *
     * @param array $rec Record
     * @return boolean
     */
    function value2db($rec)
    {
      // Let the customdir function set the storage dir for the documentfile
      $this->m_dir = $this->customdir($rec, $this->defaultdir($rec, "dir"));

      // Use the default value2db function
      $result = parent::value2db($rec);

      // Get the filename
      $filename = $this->m_dir . $rec[$this->fieldName()]["filename"];

      if (file_exists($filename))
      {
        /**
         * Modify the file rights:
         * Default file mode is 660 (owner rw, group rw, global none)
         *
         * Change to 664 if your system requires docs to have global rights
         * in order to view or download. Note that allowing global access
         * is a potential security risk and may allow undesired access.
         */
        chmod($this->m_dir . $rec[$this->fieldName()]["filename"], 0660);

        // Modify the file group if set in config:
        if (atkconfig::get("docmanager","docmanager_filegroup") != "")
          chgrp($this->m_dir . $rec[$this->fieldName()]["filename"], atkconfig::get("docmanager","docmanager_filegroup"));
      }
      else
      {
        atkerror("documentFileAttribute->value2db: File '$filename' does not exist.");
      }

      return $result;
    }

    /**
     * database to value
     *
     * @param array $rec Record
     * @return string
     */
    function db2value($rec)
    {
      // Let the customdir function set the storage dir for the documentfile
      $this->m_dir = $this->customdir($rec, $this->defaultdir($rec, "dir"));

      // Use the default db2value function
      return parent::db2value($rec);
    }
  }
?>
