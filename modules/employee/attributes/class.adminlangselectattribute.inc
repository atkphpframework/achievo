<?php

  useattrib('atklistattribute');
  atkimport("atk.atklanguage");
  class adminLangSelectAttribute extends atkListAttribute
  {
    function adminLangSelectAttribute($name,$flags=0)
    {
      $languagestranslations = $this->getLanguagesTranslations();
      $supportedlanguages = $this->getSupportedLanguages();
      array_multisort($languagestranslations, $supportedlanguages);
      $this->atkListAttribute($name, $languagestranslations, $supportedlanguages, $flags|AF_NO_TRANSLATION);
    }

    function getLanguagesTranslations()
    {
      $langs = $this->getSupportedLanguages();
      foreach ($langs as $lang)
      {
        $translated[] = atktext(array('language_'.$lang, $lang));
      }
      return $translated;
    }

    function edit($record='',$fieldprefix='')
    {
      if (!$record[$this->fieldName()]) $record[$this->fieldName()] = $this->getDefaultLanguage();
      return parent::edit($record, $fieldprefix);
    }

    function getDefaultLanguage()
    {
      return atkLanguage::getUserLanguage();
    }

    function getSupportedLanguages()
    {
      return atkLanguage::getSupportedLanguages();
    }

    function value2db($record)
    {
      global $g_sessionManager;
      $fieldname = atkconfig('auth_languagefield');
      $session = &atkSessionManager::getSession();
      $session['globals']['authentication']['user'][$fieldname] = $record[$this->fieldName()];
      sessionStore('reload_page','true');
      return parent::value2db($record);
    }

    function storageType($mode)
    {
      return ADDTOQUERY|POSTSTORE;
    }

    function store()
    {
      return true;
    }
  }

?>