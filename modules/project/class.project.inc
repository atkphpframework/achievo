<?php
  include_once("achievotools.inc");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  userelation("atkmanyboolrelation");
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
  useattrib("atknumberattribute");
  useattrib("docmanager.projectdocumentmanager");


  define("PRJ_TIMEREG_ALL_USERS", 0);
  define("PRJ_TIMEREG_MEMBERS_ONLY", 1);

  class project extends atkNode
  {
    function project($name="project", $flags=0)
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode($name,$flags|NF_EDITAFTERADD|NF_IMPORT);
      if($name=="project")
      {
        $listener = &atknew("modules.crm.listeners.eventlistener");
        $this->addListener($listener);
      }
      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));

      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_UNIQUE, 10));
      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE, 200));

      $this->add(new atkManyToOneRelation("project_category","project.project_category",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));

      $attr = &$this->add(new atkManyToOneRelation("coordinator","employee.employeeselector",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $attr->setDestinationFilter("person.status='active'");
      $attr->setAutoLinkDestination('employee.employee');

      $p_attr = &$this->add(new atkManyToOneRelation("master_project","project.projectselector",AF_RELATION_AUTOCOMPLETE|AF_HIDE_LIST|AF_HIDE_ADD));
      $p_attr->addDestinationFilter("project.id<>[id] AND (project.master_project=0 OR project.master_project IS NULL)");

      $this->add(new atkManyToOneRelation("contract_id","organization.contracts",AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkTextAttribute("description",TEXT_LARGE, AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkAttribute("quotation_number", AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkCurrencyAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD, 13,"", 2), "finance");

      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      if (!isset($_SERVER["PWD"]))
        $reportnodetype = $g_sessionManager->stackVar("reportnodetype");
      else
        $reportnodetype = "";

      /** @todo: Remove filters from project node constructor */
      if (in_array($ATK_VARS["atknodetype"], array("project.project", "project.project_personcontact", "project.project_personemployee", "project.phase", "search.search"))&& in_array($ATK_VARS["atkaction"],array("admin","add","save","update","edit","search", "view")) || $reportnodetype=="hoursurvey")
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_LIST_NO_NULL_ITEM|AF_HIDE_LIST, 15));
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE|AF_LIST_NO_NULL_ITEM, 15));
        $this->addFilter("project.status='active'");
      }

      if (($ATK_VARS['atkaction'] == "edit" || $ATK_VARS['atkaction'] == "update") && atkconfig::get("project","project_contact_obligatory"))
      {
        $flags = AF_OBLIGATORY;
      }
      else
      {
        $flags = 0;
      }

      // Add the organization relation
      $this->add(new atkManyToOneRelation("customer", "organization.organizationselector", AF_HIDE_LIST|AF_RELATION_AUTOLINK|AF_HIDE_ADD|AF_SEARCHABLE|AF_FORCE_LOAD), "", 500)
                 ->setAutoLinkDestination('organization.organization');

      $this->add(new atkOneToManyRelation("contacts","project.project_personcontact", "projectid", AF_HIDE_LIST|AF_CASCADE_DELETE|AF_HIDE_ADD|$flags));

      $this->add(new atkOneToManyRelation("members","project.project_personemployee", "projectid", AF_HIDE_LIST|AF_CASCADE_DELETE), "planning");
      $this->add(new atkDummyAttribute("hint", atktext("project_member_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW|AF_HIDE_ADD), "planning");
      $this->add(new atkFuzzySearchAttribute("member_add", "employee.employee", "storeMembers", "multiselect", AF_HIDE_ADD|AF_BLANK_LABEL), "planning");

      // This is a listattrib and not a boolattrib, because more options may be added in the future.
      $this->add(new atkListAttribute("timereg_limit", array("all_users", "members_only"), array(PRJ_TIMEREG_ALL_USERS, PRJ_TIMEREG_MEMBERS_ONLY), AF_LIST_NO_NULL_ITEM|AF_HIDE_ADD|AF_HIDE_LIST), "planning");

      $this->add(new atkBoolAttribute("alwaysvisibleintimereg", AF_HIDE_LIST|AF_HIDE_ADD), "planning");
      $this->add(new atkDummyAttribute("separator", "<br><hr><br>", AF_HIDE_LIST|AF_HIDE_ADD), "planning");
      $this->add(new atkDateAttribute("startdate","","",0,0,AF_HIDE_ADD), "planning");
      $this->add(new atkDateAttribute("enddate","","",0,0,AF_HIDE_ADD), "planning");

      $this->add(new atkOneToManyRelation("deliverables", "project.deliverable", "project_id", AF_HIDE_LIST), "planning");

      //$tmp = &new atkManyBoolRelation("phasetemplatematrix","project.phase","project.tpl_phase", AF_HIDE_LIST|AF_HIDE_SEARCH);
      //$tmp->m_localKey = "projectid";
      //$tmp->m_remoteKey = "template";
      //$tmp->setStoreDeletionFilter("phase.template IS NOT NULL AND phase.template!='' AND phase.template!=0");
      //The phasetemplatematrix is in add mode visible on tab 'default' and in other modes on the 'planning' tab
      //$tab = ($ATK_VARS["atkaction"]=="view"||$ATK_VARS["atkaction"]=="edit"||$ATK_VARS["atkaction"]=="update"?"planning":"default");
      //$this->add($tmp, $tab);

      //all phases (including manyally added ones) are shown in this relation.
      $p_attr = &$this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE),"planning");
      $p_attr->setHeader("phaseLink");

      $this->add(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL|AF_HIDE_VIEW), "planning");
      $this->add(new projecttemplateAttribute("template","project.tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_FORCE_LOAD|AF_HIDE_SEARCH|AF_HIDE_VIEW));

      $this->add(new atkOneToManyRelation("todos", "todo.todo", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "todos");
      $this->add(new atkOneToManyRelation("notes", "notes.project_notes", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "notes");
      $this->add(new projectDocumentManager("documents", AF_HIDE_LIST|AF_HIDE_SEARCH), "documents");

      $timeline = &new dataGraphAttribute("timeline", "timeline", array("projectid"=>"[id]", "resolution"=>"auto"), "line", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT);
      $timeline->addDynamicParam("resolution", array("day", "week", "month", "auto"));
      $this->add($timeline, "stats");
      $this->add(new dataGraphAttribute("phasetimedistribution", "phasetime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      $this->add(new dataGraphAttribute("emptimedistribution", "emptime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      $this->add(new dataGraphAttribute("activitytimedistribution", "activitytime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $this->add(new atkListAttribute("contact_restrict",array("restrict_project","restrict_eligible"),array(1,2),AF_HIDE_LIST|AF_FORCE_LOAD|AF_OBLIGATORY));
      }

      $this->setTable("project","project");

      $this->setOrder("project.abbreviation, project.name");
      $this->setIndex("name");
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

    function descriptorFields()
    {
      return array("abbreviation", "name");
    }

    function descriptor($record)
    {
      if (empty($record["abbreviation"]))
        return $record["name"];
      else
        return sprintf("%s: %s", $record["abbreviation"], $record["name"]);
    }

    function postDelete($record)
    {
      $eventlog = &atkGetNode("crm.eventlog");
      $eventlog->delete_event_history($this->atknodetype(),$record["atkprimkey"]);
      return true;
    }

    function getInitialProjectcode($record)
    {
      $projectnumbermodule = atkConfig::get("project", "projectcode_module", atkConfig('projectnumbermodule'));
      if (!empty($projectnumbermodule))
      {
        atkdebug("Getting alternative project code.");
        return getModule($projectnumbermodule)->getInitialProjectcode($record);
      }
      else
      {
        atkdebug("Getting projectcode.");
        $template = atkConfig::get("project", "projectcode_prefix", "");
        $parser = new atkStringParser($template);
        $projectcode = $parser->parse($record);
        if (atkConfig::get("project", "projectcode_autonumber", false))
        {
          $result = $this->getDb()->getrows("SELECT abbreviation FROM project WHERE abbreviation LIKE '$prefix%' ORDER BY abbreviation DESC LIMIT 1");
          $number = count($result) ? ((int)substr($result[0]["abbreviation"], strlen($prefix)))+1 : 1;
          $digits = atkConfig::get("project", "projectcode_autonumberdigits", 1);
          $projectcode .= sprintf("%0".$digits."d", $number);
        }
        return $projectcode;
      }
    }

    function initial_values()
    {
      $nextyear = strtotime("+1 year");
      $rec = array("startdate"=>array("year"=>date("Y"),
                                      "month"=>date("m"),
                                      "day"=>date("d")),
                   "enddate"=>array("year"=>(date("Y",$nextyear)),
                                    "month"=>date("m",$nextyear),
                                    "day"=>date("d",$nextyear)),
                   "timereg_limit"=>PRJ_TIMEREG_MEMBERS_ONLY,
                   "status"=>"active",
                   "coordinator"=>getUser());
      $rec["abbreviation"] = $this->getInitialProjectcode($rec);
      return $rec;
    }

    /**
     * Shows the planning for a project
     *
     * @param atkActionHandler &$handler Default atk action handler
     */
    function action_planning(&$handler)
    {
      atkimport("atk.ui.atktheme");
      // Get a singleton reference to the page, theme and ui
      $page = &$this->getPage();
      $theme = &atkTheme::getInstance();
      $ui = &$this->getUi();

      // Register the default style with this page
      $page->register_style($theme->stylePath("style.css"));

      // Determine the selected project
      $projectid = $this->m_postvars["selectedprojectid"];

      // Compose the image html code
      $imageurl = moduleDir("project")."ganttchart.php?projectid=".$projectid;
      $image = '<img src="'.$imageurl.'" alt="'.atktext("title_projectplanning").'"><br>';

      // Select the name and coordinator of the selected project
      $records = $this->selectDb("project.id=".$projectid, "", "", "", array("name", "coordinator"));
      $record = $records[0];

      // Put the projectname and coordinator in a html layout
      $projectinfo = '<table border="0">';
      $projectinfo.= '<tr><td align="right"><b>'.atktext("name").': </b></td><td>'.$record["abbreviation"].': '.$record['name'].'</td></tr>';
      $projectinfo.= '<tr><td align="right"><b>'.atktext("coordinator").': </b></td><td>'.$record['coordinator']['lastname'].', '.$record['coordinator']['firstname'].'</td></tr>';
      $projectinfo.= '</table>';

      // Make a text-based legend and an explanation
      $legend = atktext("legend").": [".atktext("booked").", ".atktext("hoursleft")."]<br>";
      $legend.= atktext("ganttexplanation");

      // Compose the complete content
      $content = $projectinfo . '<br>';
      $content.= $image . '<br>';
      $content.= $legend . '<br>';

      // Put the result into a box
      $boxedcontent = $ui->renderBox(array("title"=>atktext("title_projectplanning"),"content"=>$content));
      $actionpage = $this->renderActionPage("admin", array($boxedcontent));

      // Add the boxed content to the page
      $page->addContent($actionpage);
    }

    function action_billsetup(&$handler)
    {
      include_once moduleDir("finance").'billsetup.inc';
    }

    function action_billing(&$handler)
    {
      include_once moduleDir("finance").'projectbilling.inc';
    }

    function handleTemplateMatrix($record)
    {
      //when in add mode, if no phases were selected in the matrix, we do
      //not need to update.
      if(count($record["phasetemplatematrix"])==0)
      {
        atkdebug("no matrix template items selected in mode add.");
        return true;
      }

      $this->updatePhaseFields($record, "add");
      return true;
    }

    /**
     * One line description of the function
     *
     * Full description of the function
     *
     * @param type name description
     * @return type description
     */
    function postAdd($rec)
    {
      $this->handleTemplateMatrix($rec);

      $skel = atkconfig::get("project",'project_dir_skel');
      $dest = atkconfig::get("project",'project_dir_destination');

    	if ($skel != '' && $dest != '' && is_dir($skel))
    	{
    	  atkdebug("Copying skell: $skel to destination: $dest started.");
    	  atkimport("atk.utils.atkfileutils");


    	  if (!is_dir($dest))
    	  {
    	    atkdebug("Destination isn't a existing directory. First create the destination.");
    	    $succes = atkFileUtils::mkdirRecursive($dest);
    	  }
    	  else
    	  {
    	    atkdebug("Destination is a existing directory.");
    	    $succes = true;
    	  }

    	  $dirname = atkFileUtils::parseDirectoryName(atkconfig::get("project",'project_dir_name_template'), $rec);

    	  if (atkFileUtils::copyDirRecursive($skel, $dest, $dirname) && $succes)
    	  {
    	    atkdebug("The skell is copied to the destination.");

    	    $mailer = &atknew('atk.utils.atkmailer');

    	    $body = sprintf(atktext("project_body"), $rec['abbreviation'], getcwd()."/".$dest."/".$dirname);

    	    switch (atkconfig::get("project","project_formatmail",'htmlplain'))
    	    {
    	      default:
    	        break;

    	      case 'html':
    	        $mailer->isHTML(true);
    	        break;

    	      case 'htmlplain':
    	        $mailer->isHTML(true);
    	        $mailer->AltBody = strip_tags($body, '<a>');
    	        break;
    	    }

    	    $mailer->From = atkconfig::get("project","mail_sender", "achievo");
    	    $mailer->AddAddress(atkconfig::get("project","project_sendto"));
    	    $mailer->Subject = atktext("project_subject").$rec['abbreviation'];
    	    $mailer->Body = $body;
    	    $mailer->Send();
    	    atkdebug("Mail send to ".atkconfig::get("project","project_sendto")."?");
    	  }
    	  else
    	    atkerror("No write permisson on destination: ".$dest);
    	}
    	else
    	  atkdebug("Configs are empty or skell isn't a directory.");

    	return true;
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

      return true;
    }

    function fetchRoleId()
    {
      //$raw = $this->m_postvars["member_add_role"];
      $rolesel = new atkManyToOneRelation("member_add_role", "project.role", AF_OBLIGATORY);
      $role_id = $rolesel->fetchValue($this->m_postvars);
      if($role_id["id"]!="")
      {
        return $role_id["id"];
      }
      else
      {
        return 0;
      }
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
    function getView($varname)
    {
      $sessionManager = &atkGetSessionManager();
      $value = $sessionManager->stackVar($varname);
      if ($value=="")
      {
        $value = "active";
      }
      return $value;
    }

    /**
     * Action select, add filter if user doens't have
     * the any project right.
     *
     * @param object $handler Handler object
     * @return string Select page
     */
    function action_select(&$handler)
    {
    	if (!$this->_checkAnyProjectRight())
      {
		  	$pids = implode(",", $this->get_user_projects());
        if(!empty($pids))
          $this->addFilter("project.id IN ($pids) OR project.timereg_limit = ".PRJ_TIMEREG_ALL_USERS);
        else
          $this->addFilter("project.timereg_limit = ".PRJ_TIMEREG_ALL_USERS);
      }
      return $handler->action_select();
    }

    /**
     * Get user project id's
     *
     * @return array Array with project id's
     */
    function get_user_projects()
    {
      $db = &atkGetDb();
      $sql = "SELECT DISTINCT projectid
	               FROM project_person
	               WHERE personid = '".atkGetUserId()."'";
  	  $records = $db->getrows($sql);
  	  $id_array = array();
  	  foreach($records as $row)
  	  {
  	    array_push($id_array, $row["projectid"]);
  	  }
  	  array_push($id_array, 0);
  	  return $id_array;
    }

    function action_admin(&$handler, $record=null)
    {
      $view = $this->getView("view");
      if ($view=="active")
      {
        $this->addFilter("project.status","active");
      }
      else
      {
        $this->addFilter("project.status<>'active'");
      }

      // Access control
      if (!$this->allowed("any_project"))
      {
        $user = atkGetUser();
        $pids = implode(",", $this->get_user_projects());

        $this->addFilter("project.id IN ($pids) OR project.coordinator=".$user["id"]);

        // we can hide the coordinator column from the list in this case.
        //$attr = &$this->getAttribute("coordinator");
        //$attr->addFlag(AF_HIDE_LIST);
      }

      return $handler->action_admin($record);
    }

    /**
     * Edit action handler override
     *
     * Sets the Abbreviation attribute to readonly if the user doesn't have the changeabbreviation right
     *
     * @param atkEditHandler $handler
     * @return string HTML
     */
    function action_edit(&$handler)
    {
      if (!$this->allowed("changeabbreviation"))
      {
        $abbrattr = &$this->getAttribute("abbreviation");
        $abbrattr->addFlag(AF_READONLY);
      }
      return $handler->action_edit();
    }

    function adminFooter()
    {
      $view = $this->getView("view");

      if ($view=="active")
      {
        return atktext("onlyactiverecordsareshown")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("view"=>"nonactive")),
                                                        atktext('clicktoviewarchive', $this->m_module, $this->m_type))."<br>";
      }
      else
      {
        return atktext("youareinthearchive")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("view"=>"active")),
                                                     atktext('clicktoviewactiverecords', $this->m_module, $this->m_type))."<br>";
      }
    }

    /**
     * Function removes double selected templates (which happens when
     * using the projecttemplate and selecting additional phasetemplates.
     *
     * @param array() $record description
     * @return boolean succeeded
     */
    function preAdd(&$record)
    {
      //get all tpl_project_phase combinations with the selected
      //projecttemplate id.
      $tpl_project_phase_Node = &atkGetNode("project.tpl_project_phase");
      $selector = sprintf("projectid='%s'",$record["template"]["id"]);
      $phase_tpls = $tpl_project_phase_Node->selectDb($selector);

      //retrieve all the tpl_phase_ids
      $ids = array();
      foreach($phase_tpls as $tpl)
        $ids[] = $tpl["phaseid"]["id"];

      //Add the unique ones in a new array.
      $newmatrix = array();
      for($i=0,$_i=count($record["phasetemplatematrix"]);$i<$_i;$i++)
      {
        $matrix_tpl_id = $record["phasetemplatematrix"][$i]["template"];

        if(!in_array($matrix_tpl_id,$ids))
          $newmatrix[] = $record["phasetemplatematrix"][$i];
      }
      //replace the old array with a the new (filtered) array.
      $record["phasetemplatematrix"] = $newmatrix;

      return true;
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

    function postUpdate($record)
    {
      $this->updatePhaseFields($record, "update");
      return true;
    }

    /**
    * This function updates the templates that are added using
    * the atkManyBoolRelation.
    *
    * Function copies the name field from the phase-templates to
    * the corresponding phases.
    *
    * @todo This code belongs logically in the phase node.
    * @param array $record the added record.
    * @return type description
    */
    function updatePhaseFields($record, $mode)
    {
     $phaseTplNode = &atkGetNode("project.tpl_phase");
     $phaseNode = &atkGetNode("project.phase");
/*
     foreach($record["phasetemplatematrix"] as $tpl)
     {
       atkdebug("Using template ".$tpl["template"]);
       //get the template
       $template_id = $tpl["template"];
       $selector = sprintf("tpl_phase.id='%s'",$template_id);
       $template = $phaseTplNode->selectDb($selector);

       //get the corresponding phases for this project
       $selector = sprintf("phase.template='%s' AND phase.projectid='%s'",$template_id,$record["id"]);
       $phases = $phaseNode->selectDb($selector);

       //update each phase with the name and description fields.
       //and add the activities
       foreach($phases as $phase)
       {
         $template_id = $phase["template"]["id"];
         $selector = "tpl_phase.id='$template_id'";
         $template = $phaseTplNode->selectDb($selector);

         $newphase["id"]           = $phase["id"];
         $newphase["atkprimkey"]   = $phase["atkprimkey"];
         $newphase["status"]       = 'active';

         //if we are updating a project, we cannot overwrite existing phase data
         if($mode != "update")
         {
           atkdebug("updating name and description fields");
           $newphase["name"]         = $template[0]["name"];
           $newphase["description"]  = $template[0]["description"];
           $phaseNode->updateDb($newphase,false,"",array("name","description", "status"));
         }
         $this->addActivitiesPerPhase($template_id, $newphase["id"]);
       }
     } */
    }

    function addActivitiesPerPhase($tpl_phase_id, $phaseid)
    {
      atkdebug("Adding activities per phase");

      //Add activities per phase (if they do not exist yet).
      $node = &atkGetNode("project.phase_activity");
      /* @var $node atkNode */
      $selector = "phaseid='$phaseid'";
      $records = $node->selectDb($selector,"","","",array("activityid"));

      $ids = array();
      foreach($records as $rec)
        $ids[] = $rec["activityid"]["id"];

      $and_where = count($ids) ? " AND activityid NOT IN (".implode(",",$ids).")" : "";

      //activities per phase
      $db = &atkGetDb();
      $query = "INSERT INTO phase_activity (activityid, phaseid)
                SELECT activityid, ".$phaseid."
                FROM tpl_phase_activity
                WHERE phaseid = '".$tpl_phase_id."'".$and_where;

      $db->query($query);
    }

    /**
     * If the current project is a master project, make it impossible to
     * set a master project for this project by hiding the master_project
     * field.
     *
     * @param Integer $id The project Id
     */
    function _hideMasterProject($id)
    {
      $node = &newNode($this->atkNodeType());
      $rows = &$node->selectDb("project.master_project='$id'","","","",array('id'));

      if(count($rows)>0)
      {
        $p_attr = &$this->getAttribute("master_project");
        $p_attr->addFlag(AF_HIDE);
      }
    }

    function editPage(&$handler, $record, $locked=FALSE)
    {
      $this->_hideMasterProject($record['id']);
      return $handler->editPage($record, $locked);
    }

    function viewPage(&$handler, $record, $locked=FALSE)
    {
      $this->_hideMasterProject($record['id']);
      return $handler->viewPage($record, $locked);
    }

    function _checkAnyProjectRight()
    {
      $sessionManager = &atkGetSessionManager();
      $session = &atkSessionManager::getSession();
      $prevlevel = atkPrevLevel() - ($this->isPartial() ? 1 : 0);
      $session_prev = $session[$sessionManager->getNameSpace()]["stack"][atkStackID()][$prevlevel];
      $prevnode = atkArrayNvl($session_prev,"atknodetype");
      if(($prevnode == 'timereg.hours') || ($prevnode == 'reports.hoursurvey'))
      {
        //if we come from timereg or hoursurvey screen - check timereg right
        $securityManager = &atkGetSecurityManager();
        return $securityManager->allowed("timereg.hours", "any_project");
      }
      else return $this->allowed("any_project");
    }

    function phaseLink($record, $childrecords, $p_attr)
    {
      $page = $this->getPage();
      $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkattribute.js');

      $status = $this->getView("state");

      if($status == 'active')
      {
        $url = addslashes(partial_url($this->atkNodeType(), "edit", "attribute.phase.refresh", array("state"=>"nonactive")));
        $text = atktext("show_nonactive_phases");

        $url1 = addslashes(partial_url($this->atkNodeType(), "edit", "attribute.phase.refresh", array("state"=>"all")));
        $text1 = atktext("show_all_phases");
      }
      elseif($status == "nonactive")
      {
        $url = addslashes(partial_url($this->atkNodeType(), "edit", "attribute.phase.refresh", array("state"=>"active")));
        $text = atktext("show_active_phases");

        $url1 = addslashes(partial_url($this->atkNodeType(), "edit", "attribute.phase.refresh", array("state"=>"all")));
        $text1 = atktext("show_all_phases");
      }
      else
      {
        $url = addslashes(partial_url($this->atkNodeType(), "edit", "attribute.phase.refresh", array("state"=>"nonactive")));
        $text = atktext("show_nonactive_phases");

        $url1 = addslashes(partial_url($this->atkNodeType(), "edit", "attribute.phase.refresh", array("state"=>"active")));
        $text1 = atktext("show_active_phases");

      }

      $code = "ATK.Attribute.refresh('$url');";
      $code1 = "ATK.Attribute.refresh('$url1');";

      $link = '<a href="javascript:'.$code.'">'.$text.'</a>';
      $link1 = '<a href="javascript:'.$code1.'">'.$text1.'</a>';
      return $link." ".$link1;
    }

    function phase_edit($record, $fieldprefix, $mode)
    {
      $status = $this->getView("state");
      $p_attr = &$this->getAttribute("phase");
      if ($status == "active")
      {
        $p_attr->addDestinationFilter("phase.status='active'");
      }
      elseif($status == "nonactive")
      {
        $p_attr->addDestinationFilter("phase.status='nonactive'");
      }
      else
      {
        $p_attr->setDestinationFilter("");
      }
      $p_attr->setUseFilterForAddLink(false);
      $p_attr->setUseFilterForEditLink(false);
      return $p_attr->edit($record, $fieldprefix, $mode);
    }


  }

?>