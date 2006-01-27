<?php

  useattrib("project.phasetemplateattribute");
  useattrib("atktextattribute");
  useattrib("atklistattribute");
  useattrib("atknumberattribute");
  useattrib("atkdurationattribute");
  useattrib("atkdateattribute");
  useattrib("atkfuzzysearchattribute");
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
      $this->add(new atkListAttribute("status",array("active","nonactive"), "", AF_OBLIGATORY));

      $tmp = new phaseManyBoolRelation("phase_activity", "project.phase_activity", "project.activity", AF_HIDE_LIST|AF_OBLIGATORY|AF_MANYBOOL_AUTOLINK);
      $tmp->m_localKey = "phaseid";
      $tmp->m_remoteKey = "activityid";
      $this->add($tmp);

      // template *must* come after phase_activity, because phase_activity does a delete of all
      // records on store, so template must be stored after phase_activity.
      $this->add(new phasetemplateAttribute("template","project.tpl_phase", AF_HIDE_LIST|AF_HIDE_EDIT));

      $this->add(new atkDateAttribute("startdate", "F d Y", "d F Y"));
      $this->add(new atkDateAttribute("enddate", "F d Y", "d F Y"));

      // deliverable dependency
      $depends = &$this->add(new atkManyToOneRelation("dependsondeliverable", "project.deliverable"));
      $depends->setDestinationFilter("project_id=[projectid.id]");

      //$this->add(new atkNumberAttribute("max_phasetime"));
      $this->add(new atkDurationAttribute("initial_planning",'','', ($g_securityManager->allowed("project.project","initialplanning")?0:AF_READONLY_EDIT)|AF_TOTAL|AF_DURATION_STRING));
      $this->add(new atkDurationAttribute("current_planning",'','', AF_HIDE_ADD|AF_TOTAL|AF_DURATION_STRING));
      $this->add(new spendHoursAttribute("spend_hours","id",AF_TOTAL));

      $this->add(new atkOneToManyRelation("phaseplanning", "project.phaseplanning", "phaseid", AF_HIDE_LIST));

      $this->add(new atkFuzzySearchAttribute("phaseplanning_add", "employee.employee", "storePhaseplanning", "multiselect", AF_HIDE_ADD|AF_BLANK_LABEL, 20));

      $this->setOrder("phase.startdate, phase.name");
      $this->setTable("phase","phase");
    }

    function descriptor_def()
    {
      return "[name]";
    }

    function name_display($record)
    {
      return href(dispatch_url("project.phase", "edit", array("atkselector"=>"phase.id='".$record["id"]."'")), $record["name"], SESSION_NESTED);
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
      $node = &getNode("project.dependency");
      $delphase = $rec["id"];
      $deprec = $node->deleteDb("phaseid_row = $delphase OR phaseid_col = $delphase");
      return true;
    }

    function action_select(&$handler)
    {
      // In select mode, we place a filter, because we may only select active phases.
      $this->addFilter("phase.status='active'");

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

    /**
     * Validates a phase record before saving its changes.
     *
     * Validates unique fields, required fields, dataformat etc.
     *
     * @param array &$record The record to validate
     * @param String $mode The mode for which validation is performed ('add' or 'update')
     * @param array $ignoreList The list of attributes that should not be validated
     */
    function validate(&$record, $mode, $ignoreList=array())
    {
      // Get the date of the first registration for this node (or set $earliesttimeregdate to null if not found)
      $hoursNode = getNode("timereg.hours");
      $earliesttimeregforphase = $hoursNode->selectDb("phaseid='" . $record["id"] . "'", "hours.activitydate ASC", 1);
      atkimport("module.utils.dateutil");
      $earliesttimeregdate = empty($earliesttimeregforphase) ? null : dateutil::arr2stamp($earliesttimeregforphase["0"]["activitydate"]);

      // Get the start date of this phase record
      $phasestartdate = dateutil::arr2stamp($record["startdate"]);

      // Trigger an error on the startdate attribute if the earliesttimeregdate is not null and the startdate of this record is higher than de first earliesttimeregdate
      if ((!is_null($earliesttimeregdate)) && ($phasestartdate > $earliesttimeregdate))
      {
        triggerError($record, "startdate", "error_cannot_set_startdate_later_than_first_timereg");
      }

      // Call the parents validate function to perform the default validation on the record
      atkNode::validate($record, $mode, $ignoreList);
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
      $db = &atkGetDb();

      // attendee may already exist.
      $recs = $db->getrows("SELECT count(*) AS cnt FROM project_phaseplanning WHERE phaseid = $phase_id AND personid = $person_id");
      if (!count($recs)||$recs[0]["cnt"]==0)
      {
        $db->query("INSERT INTO project_phaseplanning (phaseid, personid, current_planning, initial_planning)
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
      $db = &atkGetDb();
      // person may allready exist
      $recs = $db->getrows("SELECT count(*) as cnt
                              FROM phase, project,project_person
                              WHERE phase.projectid = project.id
                                AND project.id = project_person.projectid
                                AND phase.id =$phase_id
                                AND project_person.personid = $person_id
                              GROUP BY project.id");
      if(!count($recs)|| $recs[0]["cnt"]==0)
      {
        $recs = $db->getrows("SELECT project.id FROM phase,project
                                WHERE phase.projectid = project.id
                                  AND phase.id = $phase_id");
        $project_id = $recs[0]["id"];
        // Add person to members, role will be empty
        $db->query("INSERT INTO project_person (projectid,personid)
                      values ($project_id,$person_id)");
      }
    }

    function rowColor($record)
    {
      if($record["current_planning"]>0)
      {
        if($record["spend_hours"]>($record["current_planning"]/60))
        {
          return COLOR_ERROR;
        }
        elseif($record["spend_hours"]>=($record["current_planning"]/60)*0.9)
        {
          return COLOR_WARNING;
        }
      }
    }
  }
?>
