<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
  useattrib('atklistattribute');
  atkimport("atk.atklanguage");

  /**
   * Language select attribute
   * @package achievo
   * @subpackage employee
   */
  class adminLangSelectAttribute extends atkListAttribute
  {
    /**
     * Constructor
     *
     * @param string $name Name
     * @param int $flags Flags
     */
    function adminLangSelectAttribute($name,$flags=0)
    {
      $languagestranslations = $this->getLanguagesTranslations();
      $supportedlanguages = $this->getSupportedLanguages();
      array_multisort($languagestranslations, $supportedlanguages);
      $this->atkListAttribute($name, $languagestranslations, $supportedlanguages, $flags|AF_NO_TRANSLATION);
    }

    /**
     * Translate supported languages
     *
     * @return array
     */
    function getLanguagesTranslations()
    {
      $langs = $this->getSupportedLanguages();
      foreach ($langs as $lang)
      {
        $translated[] = atktext(array('language_'.$lang, $lang));
      }
      return $translated;
    }

    /**
     * Override edit setting a default value
     *
     * @param array $record Record
     * @param string $fieldprefix Field prefix
     * @return string
     */
    function edit($record='',$fieldprefix='')
    {
      if (!$record[$this->fieldName()]) $record[$this->fieldName()] = $this->getDefaultLanguage();
      return parent::edit($record, $fieldprefix);
    }

    /**
     * Get default language
     *
     * @return string
     */
    function getDefaultLanguage()
    {
      $language = atkInstance("atk.atklanguage");
      return $language->getUserLanguage();
    }

    /**
     * Get supported languages
     *
     * @return array
     */
    function getSupportedLanguages()
    {
      $language = atkInstance("atk.atklanguage");
      return $language->getSupportedLanguages();
    }

    /**
     * Convert array to database record
     *
     * @param array $record Record
     * @return array
     */
    function value2db($record)
    {
      global $g_sessionManager;
      if($this->m_owner=='userpreferences')
      {
        $fieldname = atkconfig('auth_languagefield');
        $session = &atkSessionManager::getSession();
        $session['globals']['authentication']['user'][$fieldname] = $record[$this->fieldName()];
        sessionStore('reload_page','true');
      }
      return parent::value2db($record);
    }

    /**
     * Set storage type
     *
     * @param string $mode Mode
     * @return int
     */
    function storageType($mode)
    {
      return ADDTOQUERY|POSTSTORE;
    }

    /**
     * Override store
     *
     * @return boolean
     */
    function store()
    {
      return true;
    }
  }

?>