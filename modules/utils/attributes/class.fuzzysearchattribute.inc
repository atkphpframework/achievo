<?php

  class fuzzySearchAttribute extends atkAttribute
  {
    var $m_searchnode = "";
    var $m_callback = "";
    
    function fuzzySearchAttribute($name, $searchnode, $callback, $flags=0, $size=0)
    {
      if ($size==0)
      {
        global $g_layout;
        $size = $g_layout->maxInputSize();        
      }
      $this->atkAttribute($name, $flags|AF_HIDE_VIEW|AF_HIDE_LIST, $size);
      $this->m_searchnode = $searchnode;
      $this->m_callback = $callback;
    }
    
    function edit($rec="", $prefix="")
    {
      $rec=""; // clear the record so we always start with an empty
               // searchbox.
      return parent::edit($rec, $prefix);
    }
        
    function store($db, $rec, $mode)
    {
      $node = &getNode($this->m_searchnode);
      if (is_object($node) && $rec[$this->fieldName()]!="")
      {
        $tokens = explode(",", $rec[$this->fieldName()]);
        $result = array();
        foreach ($tokens as $token)
        {
          $token = trim($token);
          $result[$token] = $node->searchDb($token);
        }
      
        if (method_exists($this->m_ownerInstance, $this->m_callback))
        {
          $funcname = $this->m_callback;
          return $this->m_ownerInstance->$funcname($rec, $result);
        }
      }
      
      return true;
    }
    
    // Dummy methods to prevent loading/storing of data.
    function load() {}    
    function addToQuery() {}
    function hide() {}
    function search() {}
    function getSearchModes() {}
    function searchCondition() {}
    function fetchMeta() {}
  }

?>