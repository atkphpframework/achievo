<?php

  class projectutils 
  {
    function getAvailableActivityIds($phaseid)
    {
      if(!isset($phaseid)) return array();
  
      $phaseactivityNode = &atkGetNode("project.phase_activity");
      $selector = sprintf("phaseid='%s'",$phaseid);
      $recs = $phaseactivityNode->selectDb($selector);
      
      //get the ids
      $ids = array();
      foreach ($recs as $rec)
      {
        if(isset($rec["activityid"]["id"]) && $rec["activityid"]["id"]!='' && $rec["activityid"]["id"]!=0)
          $ids[]= $rec["activityid"]["id"];
      }
      
      return $ids;
    } 
    
    /**
     * Determine the number of available activities
     */    
    function getActivityCount()
    {
      $db = &atkGetDb();
      $result = $db->getrows("SELECT count(*) as activitycount FROM activity");
      return $result[0]['activitycount'];
    }      
  }
?>