<?php

atkimport("atk.handlers.atkabstractsearchhandler");
class hourSurveyHandler extends atkAbstractSearchHandler
{
  protected $m_userid;
  
  function __construct()
  {
    parent::atkActionHandler();
    $this->m_userid = atkGetUserId();
  }
  
    /**
   * List criteria.
   *
   * @return Array criteria list
   */
  function listCriteria()
  {
    if(!$this->tableExist()) return array();
    
    $db = &$this->m_node->getDb();
    $query = "
      SELECT c.name 
      FROM {$this->m_table} c 
      WHERE 
        c.nodetype = '%s' AND 
        (c.userid = '%s' OR c.is_for_all='1') 
      ORDER BY UPPER(c.name) AND handlertype = '%s'";
    $rows = $db->getRows(sprintf($query, $this->m_node->atkNodeType(), $this->m_userid, $this->getSearchHandlerType()));

    $result = array();
    foreach ($rows as $row)
      $result[] = $row['name'];

    return $result;
  }

  /**
   * Remove search criteria.
   *
   * @param String $name name of the search criteria
   */
  function forgetCriteria($name)
  {
    if(!$this->tableExist()) return false;
    
    $db = &$this->m_node->getDb();
    $query = "
      DELETE FROM {$this->m_table} 
      WHERE 
        nodetype = '%s' AND 
        UPPER(name) = UPPER('%s') AND 
        handlertype = '%s' AND
        userid='%s'";
        
    $db->query(sprintf($query, $this->m_node->atkNodeType(), escapeSQL($name), $this->getSearchHandlerType(),$this->m_userid));
    $db->commit();
  }
  
  /**
   * Save search criteria.
   *
   * NOTE:
   * This method will overwrite existing criteria with the same name.
   *
   * @param String $name     name for the search criteria
   * @param Array  $criteria search criteria data
   */
  function saveCriteria($name, $criteria)
  {
    if(!$this->tableExist()) return false;
    
    $this->forgetCriteria($name);
    $db = &$this->m_node->getDb();
    $query = "INSERT INTO {$this->m_table} (nodetype, name, criteria, handlertype,userid,is_for_all) VALUES('%s', '%s', '%s', '%s', '%s', '%s')";
    $db->query(sprintf($query, $this->m_node->atkNodeType(), escapeSQL($name), escapeSQL(serialize($criteria)), $this->getSearchHandlerType(), $this->m_userid,$criteria['all_users']));
    $db->commit();
  }
  
    /**
   * Load search criteria.
   *
   * @param String $name name of the search criteria
   * @return Array search criteria
   */
  function loadCriteria($name)
  {
    if(!$this->tableExist()) return array();
    
    $db = &$this->m_node->getDb();
    $query = "SELECT c.criteria FROM {$this->m_table} c WHERE c.nodetype = '%s' AND UPPER(c.name) = UPPER('%s') AND handlertype = '%s' AND (userid='%s' OR is_for_all='1')";
    
    list($row) = $db->getRows(sprintf($query, $this->m_node->atkNodeType(), escapeSQL($name), $this->getSearchHandlerType(),atkGetUserId()));
    $criteria = $row == NULL ? NULL : unserialize($row['criteria']);
    
    return $criteria;
  }

    /**
     * Fetch posted criteria.
     *
     * @return Array fetched criteria
     */
    function fetchCriteria()
    { 
      $sessionManager = &atkGetSessionManager();
      $record = array('orderby' => $this->m_postvars['orderby'],
                    'orderdirection' => $this->m_postvars['orderdirection'],
                    'subtotal' => $this->m_postvars['subtotal'],
                    'outputType' => atkArrayNvl($this->m_postvars, "outputType", ""),
                    'subProject' => atkArrayNvl($this->m_postvars, "subproject", 0),
                    'all_users' => atkArrayNvl($this->m_postvars, "all_users",0),
                    'dynamic_dates' => atkArrayNvl($this->m_postvars, "dynamic_dates",0));
      
      foreach (array('userid','nameswitch','roleid','remark','aggregate','startdate','enddate','col',
        'functionlevelswitch','employer_id') as $fieldname)
      {
        $record[$fieldname] = $sessionManager->pageVar($fieldname);
      }

      $record['department_name'] =    $sessionManager->pageVar("departmentid");

      foreach (array('projectid','phaseid') as $fieldname)
      {
        $pair = explode("=",$sessionManager->pageVar($fieldname));
        $id =          stripQuotes($pair[1]);
        $record[$fieldname] = $id;
      }

      $organizationpair =   explode("=",$sessionManager->pageVar("organizationid"));
      $organizationid =     stripQuotes($organizationpair[1]);
      if(!isset($organizationid)) $organizationid = "";
      $record['organizationid'] = $organizationid;

      $activitypair =       explode("=",$sessionManager->pageVar("activityid"));
      $activity_id =        stripQuotes($activitypair[1]);
      $record['activity_id'] = $activity_id;
      
      $categorypair =       explode("=",$sessionManager->pageVar("project_category"));
      $category =           stripQuotes($categorypair[1]);
      if(!isset($category)) $category = "";
      $record['category'] = $category;

      if (moduleExists("billing"))
      {
        $billing_projecttypepair = $g_sessionManager->pageVar("billing_projecttypeid");
        $billing_projecttypeid = (!empty($billing_projecttypepair)) ? atkArrayNvl(decodeKeyValuePair($billing_projecttypepair), "billing_projecttype.id", "") : "";
        $record['billing_projecttypeid'] = $billing_projecttypeid;
      }

      if (moduleExists('advancedsecurity'))
      {
        $lowerlevels = atkArrayNvl($this->m_postvars, "lowerlevels", 'off');
        $record['lowerlevels'] = $lowerlevels;
      }

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $contact_pair =       explode("=",$sessionManager->pageVar("contact_id"));
        $contact_id =         stripQuotes($contact_pair[1]);
        if(!isset($contact_id)) $contact_id = "";
        $record['contact_id'] = $contact_id;
      }

      $externalHourSurveyColumns = atkHarvestModules("getHourSurveyColumns", "", true);
      foreach(array_keys($externalHourSurveyColumns) as $module)
      {
        $attrlist = $externalHourSurveyColumns[$module];
        if (is_string($attrlist))
        $attrlist = array($attrlist);
        foreach($attrlist as $attr)
        {
          eval("\$record['external$attr'] = '".$sessionManager->pageVar($attr)."';");
        }
      }
      
      return $record;
    }
    
    /**
     * Returns an array with all the saved criteria
     * information. This information will be parsed
     * to the different 
     *
     * @param string $current 
     * @return array
     */
    function getSavedCriteria($current)
    {
      $result = parent::getSavedCriteria($current);
      if(!empty($result))
      {
        $result['label_dynamic_dates'] = atk_htmlentities(atktext('label_dynamic_dates'));
        $result['dynamic_dates'] = $this->getDynamicDates();

        if($this->m_node->allowed("save_for_all_users"))
        {
          $result['label_all_users'] = atk_htmlentities(atktext('label_all_users'));
          $result['all_users'] = $this->getForAllUsers();
        }
      }
      return $result;
    }
    
    private function getDynamicDates()
    { 
      return '<input id="dynamic_dates" name="dynamic_dates" type="checkbox" value="1"/>';
    }

    private function getForAllUsers()
    { 
      return '<input id="all_users" name="all_users" type="checkbox" value="1"/>';
    }
  
}