<?php

include_once "class.dependencyattrib.inc";

class tpl_dependencyAttribute extends dependencyAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function tpl_dependencyAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->dependencyAttribute($name, $flags);
  }
   
  function getPhases($projectid)
  {
    global $g_db;
    
    $query = "SELECT 
                id, name
              FROM 
                tpl_phase, tpl_project_phase
              WHERE
                id = phaseid
                AND projectid = ".$projectid." ORDER BY name";

    $recs = $g_db->getrows($query);
    return $recs;
  }
      
  function load($notused, $rec)
  {
    global $g_db;
    
    $query = "SELECT 
                phaseid_row as first, 
                ROW.name as firstname, 
                phaseid_col as last, 
                COL.name as lastname
              FROM 
                tpl_dependency, tpl_phase ROW, tpl_phase COL
              WHERE                 
                projectid = ".$rec["id"]."
                AND ROW.id = phaseid_row
                AND COL.id = phaseid_col
                ORDER BY phaseid_row";
    
    return array("deps"=>$g_db->getrows($query));
  }
  
  function store($notused, $rec)
  {
    global $g_db;
        
    $values = $rec[$this->fieldName()];
    
    // First delete deleted items.
    $delarr = $values["del"];
    if (is_array($delarr))
    {
      for ($i=0;$i<count($delarr);$i++)
      {
        $delquery = "DELETE 
                     FROM 
                       tpl_dependency 
                     WHERE projectid = ".$rec["id"]."
                       AND phaseid_row = ".$delarr[$i]["first"]."
                       AND phaseid_col = ".$delarr[$i]["last"];
        
        $g_db->query($delquery);
      }
    }
    
    // Now add a new item..
    if ($values["first"]!="" && $values["last"]!="")
    {
      $query = "INSERT 
                  INTO tpl_dependency (projectid,phaseid_row,phaseid_col)
                  VALUES (".$rec["id"].",".$values["first"].",".$values["last"].")";

      $g_db->query($query);
    }
  }
}
?>