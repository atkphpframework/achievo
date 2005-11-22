<?php

  include_once("achievotools.inc");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("atkboolattribute");
  useattrib("project.dependencyattrib");
  useattrib("project.projecttemplateattribute");
  useattrib("atknumberattribute");
  useattrib("graph.datagraphattribute");
  useattrib("atkfuzzysearchattribute");
  useattrib("atkdummyattribute");

  define("PRJ_TIMEREG_ALL_USERS", 0);
  define("PRJ_TIMEREG_MEMBERS_ONLY", 1);

  class project extends atkNode
  {
    function project($name="project", $flags=0)
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode($name,$flags|NF_EDITAFTERADD|NF_EXT_SORT);

      $this->add(new atkAttribute("id",AF_AUTOKEY));

      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE, 50));
      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_HIDE_LIST, 10));

      $this->add(new atkManyToOneRelation("coordinator","employee.employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $this->add(new atkManyToOneRelation("contract_id","organization.contracts",AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkTextAttribute("description",TEXT_LARGE, AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkNumberAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD, 13, 5), "finance");

      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      $reportnodetype = $g_sessionManager->stackVar("reportnodetype");
      if ($ATK_VARS["atknodetype"]=="project.project"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit","search", "view")) || $reportnodetype=="hoursurvey")
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_HIDE_LIST, 15));
        $this->addFilter("project.status<>'archived'");
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE, 15));
        $this->addFilter("project.status='active'");
      }

      if (($ATK_VARS['atkaction'] == "edit" || $ATK_VARS['atkaction'] == "update") && atkconfig("project_contact_obligatory"))
      {
      	$flags = AF_OBLIGATORY;
      }
      else
      {
      	$flags = 0;
      }

      $this->add(new atkOneToManyRelation("contacts","project.project_personcontact", "projectid", AF_HIDE_LIST|AF_HIDE_ADD|$flags));
      $this->add(new atkOneToManyRelation("members","project.project_personemployee", "projectid", AF_HIDE_LIST), "planning");
      $this->add(new atkDummyAttribute("hint", atktext("project_member_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW|AF_HIDE_ADD), "planning");
      $this->add(new atkFuzzySearchAttribute("member_add", "employee.employee", "storeMembers", FSA_MULTISELECT, AF_HIDE_ADD|AF_BLANK_LABEL), "planning");

      // This is a listattrib and not a boolattrib, because more options may be added in the future.
      $this->add(new atkListAttribute("timereg_limit", array("all users", "members_only"), array(PRJ_TIMEREG_ALL_USERS, PRJ_TIMEREG_MEMBERS_ONLY), AF_HIDE_ADD|AF_HIDE_LIST), "planning");

      $this->add(new atkBoolAttribute("alwaysvisibleintimereg", AF_HIDE_LIST|AF_HIDE_ADD), "planning");
      $this->add(new atkDummyAttribute("separator", "<br><hr><br>", AF_HIDE_LIST|AF_HIDE_ADD), "planning");
      $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD), "planning");
      $this->add(new atkDateAttribute("enddate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_HIDE_LIST), "planning");

      $this->add(new atkOneToManyRelation("deliverables", "project.deliverable", "project_id", AF_HIDE_LIST), "planning");

      $this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE), "planning");

      $this->add(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL|AF_HIDE_VIEW), "planning");
      $this->add(new projecttemplateAttribute("template","project.tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_HIDE_SEARCH|AF_HIDE_VIEW));

      $this->add(new atkOneToManyRelation("todos", "todo.todo", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "todos");
      $this->add(new atkOneToManyRelation("notes", "notes.project_notes", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "notes");

      $timeline = &new dataGraphAttribute("timeline", "timeline", array("projectid"=>"[id]", "resolution"=>"auto"), "line", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT);
      $timeline->addDynamicParam("resolution", array("day", "week", "month", "auto"));
      $this->add($timeline, "stats");
      $this->add(new dataGraphAttribute("phasetimedistribution", "phasetime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      $this->add(new dataGraphAttribute("emptimedistribution", "emptime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      $this->add(new dataGraphAttribute("activitytimedistribution", "activitytime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");

      $this->setTable("project","project");

      $this->setOrder("name");
      $this->setIndex("name");

      atkdebug("project::project()");
    }

    function graph_topprojects($params)
    {
      $db = &atkGetDb();

      $start = $params["startdate"];
      $end   = $params["enddate"];
      $max   = $params["max"];
      $from  = $params["from"];

      $query = &$db->createQuery();

      $query->addTable("hours");
      $query->addJoin("phase", "", "phase.id = hours.phaseid", false);
      $query->addJoin("project", "", "phase.projectid = project.id", false);
      $query->addField("project.name");
      $query->addField("project.id");
      $query->addField("sum(time) as totaltime");
      $query->addCondition("hours.activitydate BETWEEN '".$start."' AND '".$end."'");
      if ($max)
      {
        $query->setLimit($from,$max);
      }
      $query->addOrderBy("totaltime DESC");
      $query->addGroupBy("project.name");
      $data = $db->getrows($query->buildSelect());

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["name"]] = $data[$i]["totaltime"];
      }
      return array(atktext("registeredtimeperproj")=>$dataset);
    }

    // We override the dispatch function, to intercept projectid's. We use this
    // to update the list of recent projects etc. So whatever action you perform
    // on the project node, the current project you are using (if any) is stored
    // in your history. (exceptions: actions add and delete do not add
    // the project to the recent list, for obvious reasons)
    function dispatch($postvars, $fullpage=true)
    {
      if ($postvars['atkaction']!='add' && $postvars['atkaction']!='delete')
      {
        updateSelectedProjects();
      }
      return parent::dispatch($postvars, $fullpage);
    }

    function descriptor_def()
    {
      return "[name]";
    }

    function initial_values()
    {
      return array("startdate"=>array("year"=>date(Y),
                                      "month"=>date(m),
                                      "day"=>date(d)),
                   "enddate"=>array("year"=>(date(Y)+1),
                                    "month"=>date(m),
                                    "day"=>date(d)),
                   "timereg_limit"=>PRJ_TIMEREG_MEMBERS_ONLY,
                   "status"=>"active");
    }

    function action_planning(&$handler)
    {
      //include_once moduleDir('project').'projectplanning.inc';   // MILESTONES FUNCTIONS
      atkimport("atk.layout");
      $layout = &layout::getInstance();

      $prjid = $this->m_postvars["selectedprojectid"];
      $imgurl = moduleDir("project")."ganttchart.php?projectid=".$prjid;

      $records = $this->selectDb("project.id=".$prjid, "", "", "", array("name", "coordinator"));
      $record = $records[0];

      $projectinfo = '<br><table border=0>';
      $projectinfo.= '<tr><td align="right"><b>'.atktext("name").': </b></td><td>'.$record['name'].'</td></tr>';
      $projectinfo.= '<tr><td align="right"><b>'.atktext("coordinator").': </b></td><td>'.$record['coordinator']['lastname'].','.$record['coordinator']['firstname'].'</td></tr>';
      $projectinfo.= '</table><br>';

      $legend = atktext("legend").": [".atktext("booked").", ".atktext("hoursleft")."]<br>";
      $legend.= atktext("ganttexplanation");

      $layout->ui_top(atktext("title_projectplanning"));
      $layout->output('<br>'.$projectinfo.'<br><img src="'.$imgurl.'"><br><br>'.$legend);
      $layout->ui_bottom();
    }

    function action_billsetup(&$handler)
    {
      include_once moduleDir("finance").'billsetup.inc';
    }

    function action_billing(&$handler)
    {
      include_once moduleDir("finance").'projectbilling.inc';
    }

    function postDel($rec)
    {
      // TODO: delete phases and dependencies, and hours (???)
      // Phases, dependancies, todo's, notes, activities are permenantly deleted
      // trough the AF_CASCADE_DELETE flag
      // At the moment a project is also permanently deleted. This wil corrupt de hours survey.
      // Therefore NF_DELETED_FLAG should be realised for hours, projects and
      // employees.
      return true;
    }

    function fixed_price_edit($rec)
    {
      return $this->m_attribList["fixed_price"]->edit($rec)." ".atkconfig("currency_symbol");
    }

    function graph_phasetime($params, $raw=false)
    {
      $db = &atkGetDb();
      $data = $db->getrows("SELECT
                                phase.id, phase.name, SUM(time) as total
                              FROM
                                hours, phase
                              WHERE
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            " GROUP BY
                                phase.name
                              ORDER BY
                                phase.name");

      if ($raw) return $data;

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["name"]] = $data[$i]["total"];
      }

      return array("registeredtimeperphase"=>$dataset);
    }

    function graph_emptime($params, $raw=false)
    {
      $db = &atkGetDb();
      $data = $db->getrows("SELECT
                                person.id, person.firstname, person.lastname, SUM(time) as total
                              FROM
                                hours, phase, person
                              WHERE
                                hours.phaseid = phase.id
                                AND hours.userid = person.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            " GROUP BY
                                person.id
                              ORDER BY
                                person.lastname");

      if ($raw)
      {
        return $data;
      }

       // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["firstname"]." ".$data[$i]["lastname"]] = $data[$i]["total"];
      }

      return array("registeredtimeperemp"=>$dataset);
    }

    function graph_activitytime($params, $raw=false)
    {
      $db = &atkGetDb();
      $data = $db->getrows("SELECT
                                activity.id, activity.name, SUM(time) as total
                              FROM
                                hours, phase, activity
                              WHERE
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            "   AND hours.activityid = activity.id
                              GROUP BY
                                activity.name
                              ORDER BY
                                activity.name");
      // In raw mode the data is sufficient.
      if ($raw) return $data;

      // convert records to graph-compatible array.
      $dataset = array();

      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["name"]] = $data[$i]["total"];
      }

      return array("registeredtimeperactivity"=>$dataset);
    }

    function graph_dowtime($params, $raw=false)
    {
      $db = &atkGetDb();

      $days = array("sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday");

      $data = $db->getrows("SELECT
                                date_format(hours.activitydate, '%w') as dow, SUM(time) as total
                              FROM
                                hours, phase, activity
                              WHERE
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            "   AND hours.activityid = activity.id
                              GROUP BY
                                date_format(hours.activitydate, '%w')
                              ORDER BY
                                dow");

      // Add weekday names.
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $data[$i]["dow"] = atktext($days[$data[$i]["dow"]]);
      }

      // In raw mode the data is sufficient.
      if ($raw) return $data;

      // convert records to graph-compatible array.
      $dataset = array();

      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["dow"]] = $data[$i]["total"];
      }

      return array("registeredtimeperweekday"=>$dataset);
    }

    function graph_timeline($params)
    {
      $db = &atkGetDb();

      // First find out if a timeline would make more sense per week, per
      // month or per day. This all depends on the amount of time between the
      // first and last time entry.
      $range = $db->getrows("SELECT
                                 min(activitydate) as minimum,
                                 max(activitydate) as maximum
                               FROM
                                 hours, phase
                               WHERE
                                 hours.phaseid = phase.id
                                 AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":""));
      $maxdate = array();
      $mindate = array();
      list ($maxyear, $maxmonth, $maxday) = explode("-", $range[0]["maximum"]);
      list ($minyear, $minmonth, $minday) = explode("-", $range[0]["minimum"]);

      $secondsinaday = 24*60*60;

      $resolution = $params["resolution"];

      // Do we need to autodetect a resolution?
      if ($resolution=="auto"||$resolution=="")
      {
        $daysbetween = (adodb_mktime(12,0,0,$maxmonth,$maxday,$maxyear)-adodb_mktime(12,0,0,$minmonth,$minday,$minyear))/$secondsinaday;
        if ($daysbetween <= 31)
        {
          // one month range. Daybased stat makes the most sense.
          $resolution="day";
        }
        else if ($daysbetween <= 5*31)
        {
          // five month range. Week based stat makes the most sense.
          $resolution="week";
        }
        else
        {
          // > 5 month range. Month based stat makes most sense.
          $resolution="month";
        }
      }

      switch ($resolution)
      {
        case "month": $groupbystmt = "DATE_FORMAT(activitydate,'%Y%m')"; break;
        case "week": $groupbystmt = "DATE_FORMAT(activitydate,'%Y%V')"; break;
        default: $groupbybystmt = "";
      }

      $query = "SELECT
                  ".($groupbystmt==""?"activitydate":$groupbystmt)." as label, sum(time) as total
                FROM
                  hours, phase
                WHERE
                  hours.phaseid = phase.id
                  AND phase.projectid = ".$params["projectid"].
                  ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                  ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
              " GROUP BY
                  ".($groupbystmt==""?"activitydate":$groupbystmt);
      $data = $db->getrows($query);

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$this->_graphLabel($data[$i]["label"], $resolution)] = sprintf("%.2f", ($data[$i]["total"]/60));
      }

      return array("registeredtimeper".$resolution=>$dataset);

    }

    function _graphLabel($label, $scale)
    {
      if ($scale=="month")
      {
        // output nice monthnames
        $months = array("jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct",  "nov", "dec");
        $shortyear = substr($label, 2, 2);
        $month = sprintf("%d", substr($label, 4, 2));
        return atktext($months[$month-1])." '".$shortyear;
      }
      else
      {
        return $label;
      }
    }

    function member_add_edit($record, $mode)
    {
      $org = $this->m_attribList["member_add"]->edit($record, $mode);

      $rolesel = new atkManyToOneRelation("member_add_role", "project.role", AF_OBLIGATORY);

      $dummy = array();
      $org.= " ".$rolesel->edit($dummy);
      return $org;
    }

    function storeMembers($rec, $members)
    {
      $role_id = $this->fetchRoleId();

      for ($i=0, $_i=count($members); $i<$_i; $i++)
      {
        $this->_addMemberRecord($rec["id"], $members[$i]["id"], $role_id);
      }
    }

    function fetchRoleId()
    {
      //$raw = $this->m_postvars["member_add_role"];
      $rolesel = new atkManyToOneRelation("member_add_role", "project.role", AF_OBLIGATORY);
      $role_id = $rolesel->fetchValue($this->m_postvars);
      return $role_id["id"];
    }

    function _addMemberRecord($project_id, $person_id, $role_id)
    {
      $db = &atkGetDb();

      // attendee may already exist.
      $recs = $db->getrows("SELECT count(*) AS cnt FROM project_person WHERE projectid = $project_id AND personid = $person_id");
      if (!count($recs)||$recs[0]["cnt"]==0)
      {
        $db->query("INSERT INTO project_person (projectid, personid, role) VALUES ($project_id, $person_id, $role_id)");
      }
    }

     /**
     * Are we in 'active' emps or 'archive' mode?
     */
    function getView()
    {
      global $g_sessionManager;
      $view = $g_sessionManager->stackVar("view");
      if ($view=="")
      {
        $view = "active";
      }
      return $view;
    }

    function action_admin(&$handler)
    {
      $view = $this->getView();
      if ($view=="active")
      {
        $this->addFilter("project.status","active");
      }
      else
      {
        $this->addFilter("project.status<>'active'");
      }
      return $handler->action_admin();
    }

    function adminFooter()
    {
      $view = $this->getView();

      if ($view=="active")
      {
        return atktext("onlyactiverecordsareshown")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                    '&view=nonactive', atktext('clicktoviewarchive', $this->m_module, $this->m_type))."<br>";
      }
      else
      {
        return atktext("youareinthearchive")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                    '&view=active', atktext('clicktoviewactiverecords', $this->m_module, $this->m_type))."<br>";
      }
    }

    function contract_id_edit($record, $mode)
    {
      $fromdate = $this->m_attribList["startdate"]->value2db($record);

      $filter = "(contract.status='active'";
      if ($fromdate!="NULL")
      {
        $filter.= " AND '$fromdate' BETWEEN contract.startdate AND contract.enddate";
      }
      $filter.=") ";
      if ($record["contract_id"]["id"]!="")
      {
        // if a current record is selected, we must include that record, even if it falls outside the filter
        $filter.= " OR contract.id=".$record["contract_id"]["id"];
      }

      $this->m_attribList["contract_id"]->m_destinationFilter = $filter;

      return $this->m_attribList["contract_id"]->edit($record, $mode);
    }

  }

?>