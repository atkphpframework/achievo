<?php

  /** 
   * the timeCountAttribute can be added to the project node (or one of its 
   * derivatives), to add a pseudocolumn with the total number of hours spend
   * on the project.
   */
  
  class timeCountAttribute extends atkAttribute
  {
    function timeCountAttribute($flags=0)
    {
      $this->atkAttribute("timecount", $flags|AF_READONLY|AF_HIDE_ADD);
    }
    
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode!="add"&&$mode!="update")
      {
        $query->addField("sum(hours.time)");        
        $query->addJoin("phase","phase","phase.projectid = project.id",true);
        $query->addJoin("hours","hours","hours.phaseid = phase.id",true);
        
        $query->addGroupBy("project.id");
      }
    }
    
    function db2value($record)
    {      
      return $record["sum(hours.time)"];
    }
    
    function display($record, $mode)
    {
      include_once("achievotools.inc");
      return time_format($record[$this->fieldName()], true);
    }
    
  }

?>