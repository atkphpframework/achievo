<?php

useattrib("project.phasetemplateattribute");
useattrib("atktextattribute");
useattrib("atklistattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");
userelation("atkmanyboolrelation");

/** 
 * Override of default atkManyBoolRelation because isEmpty depends not only on
 * setting of phase_activity but also of template.
 */
class phaseManyBoolRelation extends atkManyBoolRelation 
{
  function isEmpty($record)
  {    
    return (parent::isEmpty($record) && $record["template"]["id"]=="");
  }
}

class phase extends atkNode
{
  function phase()
  {
    $this->atkNode("phase",NF_AUTOSELECT); // node() constructor is *not* called automatically!

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE, 50));
    $this->add(new atkManyToOneRelation("projectid","project.project",AF_OBLIGATORY|AF_HIDE_LIST|AF_HIDE_EDIT));

    $this->add(new atkTextAttribute("description", TEXT_LARGE, AF_HIDE_LIST));
    $this->add(new atkListAttribute("status",array("active","nonactive")));

    $this->add(new atkNumberAttribute("max_phasetime"));
    $this->add(new atkNumberAttribute("max_hours"));

    $tmp = new phaseManyBoolRelation("phase_activity", "project.phase_activity", "project.activity", AF_HIDE_LIST|AF_OBLIGATORY);
    $tmp->m_localKey = "phaseid";
    $tmp->m_remoteKey = "activityid";
    $this->add($tmp);

    // template *must* come after phase_activity, because phase_activity does a delete of all
    // records on store, so template must be stored after phase_activity.
    $this->add(new phasetemplateAttribute("template","project.tpl_phase", AF_HIDE_LIST|AF_HIDE_EDIT));

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

  function action_select()
  {
    // In select mode, we place a filter, because we may only select active phases.
    $this->addFilter("status='active'");

    // Also, we hide the status field (we're showing only active phases,
    // so this column is not necessary.
    $this->m_attribList["status"]->m_flags|=AF_HIDE;

    // call parent method.
    return parent::action_select();
  }
}
?>
