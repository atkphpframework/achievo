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

  function addToQuery($query)
  {
    // do nothing
  }
  
  function store($db, $record, $type)
  {
    // Todo, here we must save the activities from the template to the projectactivities table
    if ($type == "add")
    {
      if ($record['template']['id']!="")
      {
        $query = "INSERT INTO phase_activity (activityid, phaseid) SELECT activityid, ".$record['id']." FROM tpl_phase_activity WHERE phaseid = ".$record['template']['id'];
        $db->query($query);
      }
    }
  }
  
  function dbFieldType()
  {
    return "";
  }
}

?>