<?php

userelation("atkmanytoonerelation");

// When a new phase is saved, you can base it on a template, i.e. select a set of 
// default activities. To accomplish this, we make a new attribute based on a ManyToOneRelation
// that isn't stored in the db, but used to add records to the projectactivities table.
class phasetemplateAttribute extends atkManyToOneRelation
{
  function phasetemplateAttribute($name, $target, $flags)
  {
    $this->atkManyToOneRelation($name, $target, $flags);
  }
  
  function store($db, $record, $type)
  {
     if ($type == "add")
    {
      if ($record['template']['id']!="")
      {
        // Make sure we don't activities that don't exist allready
        $query = "SELECT activityid FROM phase_activity WHERE phaseid='".$record['id']."'";
        $rows = $db->getrows($query);
        $where = '';
        if(count($rows))
        {
          $where = "AND activityid NOT IN (";
          for($i=0;$i<count($rows);$i++)
          {
            if($i>0) $where.=",";
            $where.=$rows[$i]['activityid'];
          }
          $where.=")";
        }
        $query = "INSERT INTO phase_activity (activityid, phaseid) SELECT activityid, ".$record['id']." FROM tpl_phase_activity WHERE phaseid = ".$record['template']['id']." ".$where;
        $db->query($query);
      }
    }
    return true;
  }
  
  function dbFieldType()
  {
    return "";
  }
  
  function storageType($mode)
  {
    return POSTSTORE;
  }
  
  function loadType($mode, $searching=false)
  {
    return NOLOAD;
  }
}

?>
