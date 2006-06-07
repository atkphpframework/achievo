<?php

  useattrib('atklistattribute');
  class adminLangSelectAttribute extends atkListAttribute 
  {
    function adminLangSelectAttribute($name,$flags=0)
    {
      $this->atkListAttribute($name,$this->getLanguagesTranslations(),$this->getSupportedLanguages(),$flags);
    }
    
    function getLanguagesTranslations()
    {
      $langs = $this->getSupportedLanguages();
      foreach ($langs as $lang)
      {
        $translated[] = atktext('language_'.$lang);
      }
      return $translated;
    }
    
    function edit($record='',$fielprefix='')
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
    
    function storageType($mode){ return ADDTOQUERY|POSTSTORE; }

    function store()
    {
      global $ATK_VARS;
      $page = &$this->m_ownerInstance->getPage(); 
      $page->addContent("<script>top.window.location='index.php?atknodetype=".$this->m_ownerInstance->atknodetype()."&atkaction=edit&atkselector=".$this->m_ownerInstance->m_table.'.'.$this->m_ownerInstance->primaryKeyField()."={$ATK_VARS[$this->m_ownerInstance->primaryKeyField()]}"."'</script>");
      return true;
    }
  }

?>