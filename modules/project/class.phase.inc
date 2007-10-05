<?php

  useattrib("project.phasetemplateattribute");
  useattrib("atktextattribute");
  useattrib("atklistattribute");
  useattrib("atknumberattribute");
  useattrib("atkdurationattribute");
  useattrib("atkdateattribute");
  useattrib("atkdummyattribute");
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
    function phase($name="phase", $flags=0)
    {
      $securitymanager = &atkGetSecurityManager();
      $initialplanningallowed = isset($securitymanager) && $securitymanager->allowed("project.project","initialplanning");

      $this->atkNode($name,$flags|NF_AUTOSELECT|NF_EDITAFTERADD); // node() constructor is *not* called automatically!

      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
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
      $this->add(new phasetemplateAttribute("template","project.tpl_phase", AF_HIDE_LIST|AF_HIDE_EDIT|AF_FORCE_LOAD));

      $this->add(new atkDateAttribute("startdate", "", ""));
      $this->add(new atkDateAttribute("enddate", "", ""));
      $this->add(new atkDummyAttribute("duration","",AF_HIDE_ADD|AF_HIDE_EDIT));

      // deliverable dependency
      $depends = &$this->add(new atkManyToOneRelation("dependsondeliverable", "project.deliverable"));
      $depends->setDestinationFilter("project_id=[projectid.id]");

      //$this->add(new atkNumberAttribute("max_phasetime"));
      $this->add(new atkDurationAttribute("initial_planning",'','', ($initialplanningallowed?0:AF_READONLY_EDIT)|AF_TOTAL|AF_DURATION_STRING));
      $this->add(new atkDurationAttribute("current_planning",'','', AF_HIDE_ADD|AF_TOTAL|AF_DURATION_STRING));
      $this->add(new spendHoursAttribute("spend_hours","id",AF_TOTAL));

      $this->add(new atkOneToManyRelation("phaseplanning", "project.phaseplanning", "phaseid", AF_HIDE_LIST));

      $this->add(new atkFuzzySearchAttribute("phaseplanning_add", "employee.employee", "storePhaseplanning", "multiselect", AF_HIDE_ADD|AF_BLANK_LABEL, 20));

      $cascadingdeletehours = atkConfig::get("project", "project_cascading_delete_hours", false);
      if($cascadingdeletehours)
        $this->add(new atkOneToManyRelation("hours","timereg.hours","phaseid",AF_HIDE|AF_CASCADE_DELETE));

      $this->addSecurityMap("view","project.admin");
      $this->setOrder("phase.startdate, phase.name");
      $this->setTable("phase","phase");
    }

    function descriptor_def()
    {
      return "[name]";
    }


    function name_display($record, $mode)
    {
      // Do not return a html link in plain or csv mode or select screen
      if (($mode=="plain") || ($mode=="csv"))
        return $record["name"];

      // Special select handling
      if (atkArrayNvl($this->m_postvars,"atkaction")=="select")
      {
        // Decode the given target url
        $url = atkArrayNvl($this->m_postvars, "atktarget");
        $url = atkurldecode($url);
        $url = str_replace("%5B", "[", $url);
        $url = str_replace("%5D", "]", $url);
        $url = str_replace("_15B", "[", $url);
        $url = str_replace("_15D", "]", $url);

        // Parse it using the record
        atkimport("atk.utils.atkstringparser");
        $parser = &new atkStringParser($url);
        $url = $parser->parse($record,true);

        // Return a link
        return href($url, $record["name"], SESSION_NESTED);
      }
      else
      {
        // Make all project links clickable
        $defaultaction = $this->allowed("edit") ? "edit" : "view";
        $url = dispatch_url("project.phase", $defaultaction, array("atkselector"=>"phase.id='{$record["id"]}'"));
        return href($url, $record["name"], SESSION_NESTED);
      }
    }

    function duration_display($record,$mode)
    {
      if(is_array($record["startdate"]) && is_array($record["enddate"]))
      {
        $startdate = mktime(12,0,0,$record["startdate"]["month"],$record["startdate"]["day"],$record["startdate"]["year"]);
        $enddate = mktime(12,0,0,$record["enddate"]["month"],$record["enddate"]["day"],$record["enddate"]["year"]);

        $seconds_in_a_day = 86400;
        $sunday_val = "0";
        $saturday_val = "6";
        $workday_counter = 0;

        $ptr_year = intval(date("Y", $start_date));

        for($day_val = $startdate; $day_val <= $enddate; $day_val+=$seconds_in_a_day)
        {
          $pointer_day = date("w", $day_val);
          if(($pointer_day != $sunday_val) && ($pointer_day != $saturday_val))
            $workday_counter++;

        }
        return $workday_counter." ".($workday_counter==1?atktext("day","project"):atktext("days","project"));
      }
      return "";
    }

    function initial_values()
    {
      // Compose the default initual values array
      $initial_values = array("status"=>"active");

      // If there is just one activity, it should be selected by default:
      $activitynode = &atkGetNode("project.activity");
      $count = $activitynode->countDb("");
      if ($count == 1)
      {
        $activity = $activitynode->selectDb("", "", "", "", array("id"));
        $initial_values["phase_activity"] = array(array("activityid"=>$activity[0]["id"]));
      }

      // Return the initual values:
      return $initial_values;
    }

    function addDb(&$record, $exectrigger=true, $mode="add")
    {
      // This override makes sure that the current_planning is set equal to the number of initial_planning.
      $record["current_planning"] = $record["initial_planning"];
      return parent::addDb($record, $exectrigger, $mode);
    }

    function postDel($rec)
    {
      if(isset($rec["id"]))
      {
        $node = &atkGetNode("project.dependency");
        $delphase = $rec["id"];
        $deprec = $node->deleteDb("phaseid_row = $delphase OR phaseid_col = $delphase");
      }
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
      useattrib("atkDurationAttribute");
      $hours = new atkDurationAttribute("phaseplanning_initial",'','',AF_DURATION_STRING);

      $dummy = array();
      return atktext("plan_employee", "phase")." ".$org." ".atktext("for")." ".$hours->edit($dummy)." ".atktext("hours");

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
      // This check can only be done when updating an existing phase, not when adding a new one
      if ($mode == "update")
      {

        // Get the date of the first registration for this node (or set $earliesttimeregdate to null if not found)
        $hoursNode = &atkGetNode("timereg.hours");
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

      }
      // Check if the enddate isn't before the startdate
      if(($record["startdate"]["day"]!=0 && $record["startdate"]["day"]!=0 && $record["startdate"]["year"]!='') &&
         ($record["enddate"]["day"]!=0 && $record["enddate"]["day"]!=0 && $record["enddate"]["year"]!=''))
      {
        $startdate = sprintf("%04d-%02d-%02d",$record["startdate"]["year"],$record["startdate"]["month"],$record["startdate"]["day"]);
        $enddate = sprintf("%04d-%02d-%02d",$record["enddate"]["year"],$record["enddate"]["month"],$record["enddate"]["day"]);
        if($enddate<$startdate)
        {
          triggerError($record,"enddate","error_enddate_cannot_start_before_startdate");
        }
      }
      // Call the parents validate function to perform the default validation on the record
      atkNode::validate($record, $mode, $ignoreList);
    }

    function storePhaseplanning($rec, $people)
    {
      useattrib("atkDurationAttribute");
      $tmp_attrib = new atkDurationAttribute("phaseplanning_initial");
      $initial_planning = $tmp_attrib->_string2minutes($this->m_postvars["phaseplanning_initial"]);

      if ($initial_planning>0)
      {
        for ($i=0, $_i=count($people); $i<$_i; $i++)
        {
          $this->_addPlanningRecord($rec["id"], $people[$i]["id"], $initial_planning);
        }
      }
      return true;
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

    function recordActions($record, &$actions, &$mraactions)
    {
      //when we have a phase that belongs to a template, the records cannot be deleted in an admin screen.
      if(isset($record["template"]["id"]) && ($record["template"]["id"]!="") && $record["template"]["id"]!=0)
      {
        unset($actions["delete"]);
      }
    }

    function preAdd(&$rec)
    {
      //if the phase is added, check if the name is filled in and the template is
      if(empty($rec["name"]))
      {
        if(!empty($rec["template"]["id"]))
        {
          $node = &atkGetNode("project.tpl_phase");
          /* @var $node atkNode */
          $select = sprintf($node->m_table.".".$node->primaryKeyField()."='%s'",$rec["template"]["id"]);
          list($record) = $node->selectDb($select,"","","",array("name"));
          if($record!=NULL)
            $rec["name"] = $record["name"];
        }
      }
      return true;
    }
  }
?>
