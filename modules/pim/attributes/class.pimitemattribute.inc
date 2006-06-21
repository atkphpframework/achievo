<?php
  /**
   * pimItemAttribute class file
   *
   * @package achievo
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Include the parent class file
   */
  useattrib("atklistattribute");

  /**
   * pimItemAttribute class extends the atkListAttribute class with a routine
   * to automatically populate it with the available pim items when needed.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo
   */
  class pimItemAttribute extends atkListAttribute
  {

    /**
     * Loaded state which is used to prevent loading the pim items multiple times
     *
     * @access protected
     * @var boolean
     */
    var $m_pimitemsloaded = false;

    /**
     * pimItemAttribute constructor
     *
     * @access private
     * @param string $name Template file to parse
     * @param string $flags Template file to parse
     * @param string $size Template file to parse
     */
    function pimItemAttribute($name, $flags=0, $size=255)
    {
      $this->atkListAttribute($name, array(), array(), $flags, $size);
      $this->m_dbfieldtype = "string";
    }

    /**
     * Populates the internal m_options, m_values and m_lookup arrays with pim items
     */
    function loadpimitems()
    {
      // Do not continue if pim items already loaded
      if ($this->m_pimitemsloaded)
        return;

      // Harvest the modules for pim items and populate m_lookup, m_options
      // and m_values
      $pimItems = atkHarvestModules("getPimItems","",true);
      $this->m_lookup = array();
      foreach ($pimItems as $module => $items)
      {
        foreach($items as $itemName => $itemFunc)
        {
          $item = $module.'_'.$itemName;
          $this->m_lookup[$item] = atkText($module,$module).' - '.atkText(array("pim_$itemName", $itemName),$module);
        }
      }

      // Remove disallowed pimitems from the list
      $disallowedpimitems = atkConfig("disallowedpimitems", array());
      foreach($disallowedpimitems as $disallowedpimitem)
        if (array_key_exists($disallowedpimitem, $this->m_lookup))
          unset($this->m_lookup[$disallowedpimitem]);

      $this->m_options = array_values($this->m_lookup);
      $this->m_values = array_keys($this->m_lookup);

      // Set m_pimitemsloaded to true to prevent loading pim items multiple times
      $this->m_pimitemsloaded = true;
    }

    /**
     * Calls loadpimitems before calling the parents display function
     *
     * @param array $record Array with fields
     * @return string text string of $record
     */
    function display($record)
    {
      $this->loadpimitems();
      return parent::display($record);
    }

    /**
     * Calls loadpimitems before calling the parents edit function
     *
     * @param array $record Array with fields
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String Piece of html code containing the select control
     */
    function edit($record="", $fieldprefix="")
    {
      $this->loadpimitems();
      return parent::edit($record, $fieldprefix);
    }
  }
?>