<?php

  include_once("achievotools.inc"); // for time_format

 /**
  * Small attribute to calculate the amount of spend hours
  * on a phase
  */
  class spendHoursAttribute extends atkAttribute
  {
    /**
     * The field that contains the phaseid
     */
    var $m_phaseid_field = "";
    
    /**
     * Constructor
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags of the attribute
     */
    function spendHoursAttribute($name,$phaseid_field,$flags=0)
    {
      $this->m_phaseid_field = $phaseid_field;
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH|AF_READONLY|AF_HIDE_ADD);
    }
    
    /**
     * Load values
     *
     * @param atkDb $db pointer to the database
     * @param array $record Record
     * @param string $mode Mode for loading (admin, select, copy, etc)
     */
    function load(&$db, $record, $mode="")
    {
      $phaseattrib = $this->m_ownerInstance->m_attribList[$this->m_phaseid_field];
      
      $sql = "SELECT sum(time) as spend_minutes FROM hours WHERE phaseid=".$phaseattrib->value2db($record);
      if(is_array($record["personid"])) $sql.=" AND userid='".$record["personid"]["id"]."'";
      $nrow = $db->getrows($sql);
      return ($nrow[0]["spend_minutes"]/60);
    }
    
    /**
     * Display override. If possible, displays a link to the hoursurvey to view details.
     */
    function display($record, $mode)
    {      
      $value = $record[$this->fieldname()];
      $org = time_format(($value*60), true);
      if ($value>0 && $record[$this->m_phaseid_field]!="")
      {
        $phaseattrib = $this->m_ownerInstance->m_attribList[$this->m_phaseid_field];
        $params["phaseid"] = $phaseattrib->value2db($record);
        $params["startdate"] = "2000-01-01";
        $params["enddate"] = date("Y-m-d");
        if(is_array($record["personid"]))
        {
          $params["userid"] = $record["personid"]["id"];
        }
        else
        {
          $params["userid"] = "all";
        }
        
        $org = href(dispatch_url("reports.hoursurvey", "report", $params), $org, SESSION_NESTED, ($mode!="list"));
      }
      return $org;
    }
    
    /**
    * No function, but is neccesary
    */
    function store($a, $b)
    {
    }

    /**
    * No function, but is neccesary
    */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="",$level)
    {
    }
  }

?>