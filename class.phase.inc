<?php

// When a new project is saved, you can base it on a template, i.e. select a set of 
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
}


class phase extends atkNode
{
  function phase()
  {
    $this->atkNode("phase",NF_AUTOSELECT); // node() constructor is *not* called automatically!

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->addAttribute(new atkManyToOneRelation("projectid","project",AF_OBLIGATORY|AF_HIDE_LIST|AF_HIDE_EDIT));
    
    $this->addAttribute(new atkTextAttribute("description",AF_HIDE_LIST));
    //$this->addAttribute(new atkListAttribute("status",Array("active","nonactive")));
    
    $this->addAttribute(new atkAttribute("max_phasetime"));
    $this->addAttribute(new atkAttribute("max_hours"));        
    
    $tmp = new atkManyBoolRelation("phase_activity","activity",AF_HIDE_LIST);
    $tmp->m_localKey = "phaseid";
    $tmp->m_remoteKey = "activityid";
    $this->addAttribute($tmp);
    
    // template *must* come after phase_activity, because phase_activity does a delete of all
    // records on store, so template must be stored after phase_activity.
    $this->addAttribute(new phasetemplateAttribute("template","tpl_phase", AF_HIDE_LIST|AF_HIDE_EDIT));

    $this->setOrder("name");
    $this->setTable("phase","phase");
  }
  
  function descriptor_def()
  {
    return "[name]";
  }

  function initial_values()
  {
    return Array("status"=>"active");
  }
  
  function postDel($rec)
  {
    global $g_db;
    $delphase = $rec["id"];
    
    // delete dependencies for this phase./
    $query = "DELETE FROM dependency WHERE phaseid_row = $delphase OR phaseid_col = $delphase";
    $g_db->query($query);
  }
}
?>