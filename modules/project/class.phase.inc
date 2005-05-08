<?php

  useattrib("project.phasetemplateattribute");
  useattrib("atktextattribute");
  useattrib("atklistattribute");
  useattrib("atknumberattribute");
  useattrib("atkdurationattribute");
  useattrib("atkdateattribute");
  useattrib("utils.fuzzysearchattribute");  
  useattrib("project.spendhoursattribute");
  userelation("atkmanytoonerelation");
  userelation("atkmanyboolrelation");
  userelation("atkonetomanyrelation");
  
  
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
      global $g_securityManager;
      
      $this->atkNode("phase",NF_AUTOSELECT|NF_EDITAFTERADD); // node() constructor is *not* called automatically!
  
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE, 50));
      $this->add(new atkManyToOneRelation("projectid","project.project",AF_OBLIGATORY|AF_HIDE_LIST|AF_HIDE_EDIT));
  
      $this->add(new atkTextAttribute("description", TEXT_LARGE, AF_HIDE_LIST));
      $this->add(new atkListAttribute("status",array("active","nonactive")));
      
      $tmp = new phaseManyBoolRelation("phase_activity", "project.phase_activity", "project.activity", AF_HIDE_LIST|AF_OBLIGATORY);
      $tmp->m_localKey = "phaseid";
      $tmp->m_remoteKey = "activityid";
      $this->add($tmp);
  
      // template *must* come after phase_activity, because phase_activity does a delete of all
      // records on store, so template must be stored after phase_activity.
      $this->add(new phasetemplateAttribute("template","project.tpl_phase", AF_HIDE_LIST|AF_HIDE_EDIT));
      
      $this->add(new atkDateAttribute("startdate", "F d Y", "d F Y"));
      $this->add(new atkDateAttribute("enddate", "F d Y", "d F Y"));
  
      //$this->add(new atkNumberAttribute("max_phasetime"));
      $this->add(new atkDurationAttribute("initial_planning",'','', ($g_securityManager->allowed("project.project","initialplanning")?0:AF_READONLY_EDIT)|AF_TOTAL|AF_DURATION_STRING));
      $this->add(new atkDurationAttribute("current_planning",'','', AF_HIDE_ADD|AF_TOTAL|AF_DURATION_STRING));
      $this->add(new spendHoursAttribute("spend_hours","id",AF_TOTAL));
      
      $this->add(new atkOneToManyRelation("phaseplanning", "project.phaseplanning", "phaseid", AF_HIDE_LIST));
      
      $this->add(new fuzzySearchAttribute("phaseplanning_add", "employee.employee", "storePhaseplanning", FSA_MULTISELECT, AF_HIDE_ADD|AF_BLANK_LABEL, 20));
  
      $this->setOrder("phase.startdate, phase.name");
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
    
    function addDb(&$record, $exectrigger=true, $mode="add")
    {
      // This override makes sure that the current_planning is set equal to the number of initial_planning.
      $record["current_planning"] = $record["initial_planning"];
      return parent::addDb($record, $exectrigger, $mode);
    }
  
    function postDel($rec)
    {
      global $g_db;
      $delphase = $rec["id"];
  
      // delete dependencies for this phase./
      $query = "DELETE FROM dependency WHERE phaseid_row = $delphase OR phaseid_col = $delphase";
      $g_db->query($query);
    }  
  
    function action_select(&$handler)
    {
      // In select mode, we place a filter, because we may only select active phases.
      $this->addFilter("status='active'");
  
      // Also, we hide the status field (we're showing only active phases,
      // so this column is not necessary.
      $this->m_attribList["status"]->m_flags|=AF_HIDE;
  
      // call parent method.
      return $handler->action_select();
    }
    
    function phaseplanning_add_edit($record, $mode)
    {
      $org = $this->m_attribList["phaseplanning_add"]->edit($record, $mode);
      
      $hours = new atkAttribute("phaseplanning_initial");
      
      $dummy = array();
      return text("plan_employee", "phase")." ".$org." ".text("for")." ".$hours->edit($dummy)." ".text("hours");
      
    }
    
    function storePhaseplanning($rec, $people)
    {     
      $initial_planning = $this->m_postvars["phaseplanning_initial"];
      
      if ($initial_planning>0)
      {
        for ($i=0, $_i=count($people); $i<$_i; $i++)
        {        
          $this->_addPlanningRecord($rec["id"], $people[$i]["id"], $initial_planning);
        }
      }
    }
    
    function _addPlanningRecord($phase_id, $person_id, $initial_planning)
    {
      global $g_db;
      
      // attendee may already exist.
      $recs = $g_db->getrows("SELECT count(*) AS cnt FROM project_phaseplanning WHERE phaseid = $phase_id AND personid = $person_id");
      if (!count($recs)||$recs[0]["cnt"]==0)
      {
        $g_db->query("INSERT INTO project_phaseplanning (phaseid, personid, current_planning, initial_planning)
                           VALUES ($phase_id, $person_id, $initial_planning, $initial_planning)");
      }
      $this->_addTeamMemberRecord($phase_id,$person_id);
    }
    
   /**
    * Check if the person is allready a teammember
    * if not, add the person
    *
    * @param int $phase_id
    * @param int $person_id
    */
    function _addTeamMemberRecord($phase_id,$person_id)
    {
      global $g_db;
      // person may allready exist
      $recs = $g_db->getrows("SELECT count(*) as cnt 
                              FROM phase, project,project_person
                              WHERE phase.projectid = project.id
                                AND project.id = project_person.projectid
                                AND phase.id =$phase_id
                                AND project_person.personid = $person_id
                              GROUP BY project.id");
      if(!count($recs)|| $recs[0]["cnt"]==0)
      {
        $recs = $g_db->getrows("SELECT project.id FROM phase,project
                                WHERE phase.projectid = project.id
                                  AND phase.id = $phase_id");
        $project_id = $recs[0]["id"];
        // Add person to members, role will be empty
        $g_db->query("INSERT INTO project_person (projectid,personid)
                      values ($project_id,$person_id)");
      }
    }

  
    function rowColor($record)
    { 
      if($record["current_planning"]["hours"]>0)  
      {   
        if($record["spend_hours"]>($record["current_planning"]["hours"])) 
        {
          return COLOR_ERROR;
        }
        elseif($record["spend_hours"]>=($record["current_planning"]["hours"])*0.9)
        {
          return COLOR_WARNING;
        }
      }
    }
  }

  function postDel($rec)
  {
    global $g_db;
    $delphase = $rec["id"];

    // delete dependencies for this phase./
    $query = "DELETE FROM dependency WHERE phaseid_row = $delphase OR phaseid_col = $delphase";
    $g_db->query($query);
    return true;
  }  

  function action_select(&$handler)
  {
    // In select mode, we place a filter, because we may only select active phases.
    $this->addFilter("status='active'");

    // Also, we hide the status field (we're showing only active phases,
    // so this column is not necessary.
    $this->m_attribList["status"]->m_flags|=AF_HIDE;

    // call parent method.
    return $handler->action_select();
  }
?>
