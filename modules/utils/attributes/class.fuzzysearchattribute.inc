<?php

  define("FSA_ALL"             , 1);
  define("FSA_FIRST"           , 2);
  define("FSA_FIRSTPERKEYWORD" , 3);  
  define("FSA_SELECT"          , 4);
  define("FSA_SELECTPERKEYWORD", 5);
  define("FSA_MULTISELECT"     , 6);

  class fuzzySearchAttribute extends atkAttribute
  {
    var $m_searchnode = "";
    var $m_callback = "";
    var $m_mode = FSA_ALL;
    
    var $m_select = false;
    var $m_matches = array();
    var $m_searchnodeInstance = NULL;
    
    function fuzzySearchAttribute($name, $searchnode, $callback, $mode=FSA_ALL, $flags=0, $size=0)
    {
      if ($size==0)
      {
        global $g_layout;
        $size = $g_layout->maxInputSize();        
      }
      $this->atkAttribute($name, $flags|AF_HIDE_VIEW|AF_HIDE_LIST, $size);
      $this->m_searchnode = $searchnode;
      $this->m_callback = $callback;
      $this->m_mode = $mode;      
    }
    
    function createSearchNodeInstance()
    {
      if (!is_object($this->m_searchnodeInstance))
      {
        $this->m_searchnodeInstance = &getNode($this->m_searchnode);            
        return is_object($this->m_searchnodeInstance);        
      }
      return true;
    }
    
    function validate(&$rec, $mode)
    {    
      if (is_array($rec[$this->fieldName()]))
      {
        // Coming from selectscreen, no search necessary anymore.
      }
      else
      {
        $this->m_matches = $this->getMatches($rec[$this->fieldName()]);     
        
        $mustselect = false;
          
        if ($this->m_mode==FSA_MULTISELECT || $this->m_mode==FSA_SELECTPERKEYWORD)
        {
          // In multiselect and selectperkeyword mode, we present the selector 
          // if one or more keywords returned more than one match. If they 
          // all returned exactly one match, we pass all records and don't 
          // offer selection.
          foreach($this->m_matches as $keyword=>$res)
          { 
            if (count($res)>1)
            {
              $mustselect = true;
              break;
            }
          }
        }
        else if ($this->m_mode==FSA_SELECT)
        {
          // In single select mode, we show the selector if they all return 
          // just one match together.                
          $total = 0;
          foreach($this->m_matches as $keyword=>$res)
          { 
            $total+=count($res);
          }
          $mustselect = ($total>1);
        }
        
        if ($mustselect)
        {
          triggerError($rec, $this->fieldName(), 'fsa_pleasemakeselection');
          $this->m_select = true;
          return false;
        }
          
      }
      return true;
    }
    
    function edit($rec="", $prefix="")
    {
      // There are 2 possibilities. Either we are going to search,
      // in which case we show a searchbox. 
      // Or, a search has already been performed but multiple 
      // matches have been found and one of the FSA_SELECT flags was set.
      // In this case, we show the selects.
      if ($this->m_select && $this->createSearchNodeInstance())
      {
        $res = "";
        if ($this->m_mode==FSA_MULTISELECT)
        {
          // Select multiple records from all matches
          $checkboxes = array();
          foreach($this->m_matches as $keyword => $matches)
          { 
            for ($i=0, $_i=count($matches); $i<$_i; $i++)
            {
              $item = '<INPUT TYPE="CHECKBOX" NAME="'.$prefix.$this->fieldName().'[]" VALUE="'.
                              $this->m_searchnodeInstance->primaryKey($matches[$i]).'">'.$this->m_searchnodeInstance->descriptor($matches[$i]);
              if (!in_array($item, $checkboxes))
              {
                $checkboxes[] = $item;
              }
            }
          }
          $res.= implode("<BR>", $checkboxes);
        }
        else if ($this->m_mode==FSA_SELECT || ($this->m_mode==FSA_MULTISELECT && count($this->m_matches)==1))
        {
          // Select one record from all matches.
          $res.= '<SELECT NAME="'.$prefix.$this->fieldName().'[]">';
          $res.= '<OPTION VALUE="">'.text('select_none');
          $selects = array();
          foreach($this->m_matches as $keyword => $matches)
          { 
            for ($i=0, $_i=count($matches); $i<$_i; $i++)
            {
              $item = '<OPTION VALUE="'.$this->m_searchnodeInstance->primaryKey($matches[$i]).'">'.$this->m_searchnodeInstance->descriptor($matches[$i]);
              if (!in_array($item, $selects))
              {
                $selects[] = $item;
              }              
            }
            $res .= implode("\n", $selects);
          }
          $res.= '</SELECT>';
        }        
        else if ($this->m_mode==FSA_SELECTPERKEYWORD)
        {                  
          // Select one record per keyword.
          $res = '<table border="0">';
          foreach($this->m_matches as $keyword => $matches)
          {
            if (count($matches)>0)
            {
              $res.= '<tr><td>\''.$keyword.'\': </td><td><SELECT NAME="'.$prefix.$this->fieldName().'[]">';
              $res.= '<OPTION VALUE="">'.text('select_none');
              for ($i=0, $_i=count($matches); $i<$_i; $i++)
              {
                $res.= '<OPTION VALUE="'.$this->m_searchnodeInstance->primaryKey($matches[$i]).'">'.$this->m_searchnodeInstance->descriptor($matches[$i]);
              }
              $res.= '</SELECT></td></tr>';            
            }            
          }
          $res.='</table>';
        }        
        return $res;
      }
      else
      {      
        $rec=""; // clear the record so we always start with an empty
                 // searchbox.
        return parent::edit($rec, $prefix);
      }
    }
    
    function getMatches($searchstring)
    {
      atkdebug("Performing search");
      $result = array();
      if ($this->createSearchNodeInstance() && $searchstring!="")
      {      
        $tokens = explode(",", $searchstring);
        foreach ($tokens as $token)
        {
          $token = trim($token);
          $result[$token] = $this->m_searchnodeInstance->searchDb($token);
        }
      }
      return $result;
    }
        
    function store($db, $rec, $mode)
    {        
      $resultset = array();
      
      if (is_array($rec[$this->fieldName()]))
      {      
        // If the value is an array, this means we must have come from a select.
        // The user has selected some options, and we must process those.        
        
        // First, load the records, based on the where clauses. 
        $wheres = array();
        $matches = $rec[$this->fieldName()];
        for($i=0, $_i=count($matches); $i<$_i; $i++)
        {
          if ($matches[$i]!="") $wheres[] = $matches[$i];          
        }
        if (count($wheres) && $this->createSearchNodeInstance())
        {
          $whereclause = "((".implode(") OR (", $wheres)."))";
          
          $resultset = $this->m_searchnodeInstance->selectDb($whereclause, 
                                                             $this->m_searchnodeInstance->m_defaultOrder, 
                                                             "", 
                                                             $this->m_searchnodeInstance->m_listExcludes, 
                                                             "", 
                                                             "admin");          
        }
      }
      else if (count($this->m_matches)>0)
      {      
        // We didn't come from a select, but we found something anyway. 
        // Depending on our mode parameter, we either pass all records to
        // the callback, or the first for every keyword, or the very first.
        if ($this->m_mode==FSA_ALL)
        {
          // Pass all matches.
          foreach($this->m_matches as $keyword=>$matches)
          {
            for ($i=0, $_i=count($matches); $i<$_i; $i++)
            {
              // Make sure there are no duplicates
              if (!in_array($matches[$i], $resultset))
              {
                $resultset[] = $matches[$i];
              }
            }
          }
        }        
        else if ($this->m_mode==FSA_FIRSTPERKEYWORD)
        {
          // Pass first matches of all keywords.
          foreach($this->m_matches as $keyword=>$matches)
          {
            if (count($matches))
            {              
              $resultset[] = $matches[0]; 
            }
          }
        }        
        else if ($this->m_mode==FSA_FIRST)
        {
          // Pass only the first record of the first match. 
          if (count($this->m_matches))
          {
            $first = reset($this->m_matches);
            if (count($first))
            {
              $resultset[] = $first[0];
            }
          }
        }
        else
        {
          // We get here if one of the SELECT modes is active, but no 
          // selection was made. Getting here means that the validate() 
          // method above decided that presenting a selector was not 
          // necessary. We trust that judgement, and pass all records
          // that were found.
                    
          foreach($this->m_matches as $keyword=>$matches)
          {
            for ($i=0, $_i=count($matches); $i<$_i; $i++)
            {
              // Make sure there are no duplicates
              if (!in_array($matches[$i], $resultset))
              {
                $resultset[] = $matches[$i];
              }
            }
          }          
        }
      }                
      
      if (count($resultset))
      {
        if (method_exists($this->m_ownerInstance, $this->m_callback))
        {
          $funcname = $this->m_callback;
          return $this->m_ownerInstance->$funcname($rec, $resultset);
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
    function dbFieldSize() {}
    function dbFieldType() {}
  }

?>
