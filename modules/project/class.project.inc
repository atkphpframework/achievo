<?php

  include_once("achievotools.inc");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("project.dependencyattrib");
  useattrib("project.projecttemplateattribute");

  class project extends atkNode
  {
    function project()
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode("project",NF_EDITAFTERADD);

      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE));
      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_HIDE_LIST));

      $this->add(new atkManyToOneRelation("coordinator","employee.employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $this->add(new atkTextAttribute("description",AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD));
      $this->add(new atkDateAttribute("enddate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_HIDE_LIST));

      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      if ($ATK_VARS["atknodetype"]=="project"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit","search")))
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_SEARCHABLE));
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE));
        $this->addFilter("project.status","active");
      }

      $this->add(new atkOneToManyRelation("contacts","project.project_personcontact", "projectid", AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation("employees","project.project_personemployee", "projectid", AF_HIDE_LIST));

      $this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE));

      $this->add(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL|AF_HIDE_VIEW));
      $this->add(new projecttemplateAttribute("template","project.tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_HIDE_SEARCH));

      $this->setTable("project","project");

      $this->setOrder("name");
      $this->setIndex("name");

      //When a new project is selected add it to the selectedprj array
      //in stack
      if ($ATK_VARS!='add' || $ATK_VARS!='delete' || $ATK_VARS!='select')
        updateSelectedProjects();
      atkdebug("project::project()");
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
                   "status"=>"active");
    }

    function action_stats()
    {
      include_once moduleDir('project').'projectstats.inc';   // MILESTONES FUNCTIONS
    }

    function action_planning()
    {
      include_once moduleDir('project').'projectplanning.inc';   // MILESTONES FUNCTIONS
    }

    function action_billsetup()
    {
      include_once moduleDir("finance").'billsetup.inc';
    }

    function action_billing()
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

    }

    function fixed_price_edit($rec)
    {
      return $this->m_attribList["fixed_price"]->edit($rec)." ".atkconfig("currency_symbol");
    }

    function action_view()
    {
      global $g_layout, $g_db;

      $query = "SELECT project.*, person.lastname, person.firstname FROM project
                LEFT JOIN person ON person.id = project.coordinator
                WHERE ".$this->m_postvars["atkselector"];
      $project = $g_db->getRows($query);

      list($dummy, $projectid) = explode("=", $this->m_postvars["atkselector"]);

      $g_layout->initGUI();
      $g_layout->ui_top(text("project").' - '.text("view"));
      $g_layout->output($this->statusbar());
      $g_layout->output('<br>');

      $g_layout->table_simple();
      $g_layout->output('<tr>');
      $g_layout->td(text("name").": ");
      $g_layout->td($project[0]["name"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("abbreviation").": ");
      $g_layout->td($project[0]["abbreviation"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("coordinator").": ");
      $g_layout->td($project[0]["lastname"].", ".$project[0]["firstname"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("description").": ");
      $g_layout->td($project[0]["description"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("fixed_price").": ");
      $g_layout->td($project[0]["fixed_price"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("startdate").": ");
      $g_layout->td($project[0]["startdate"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("enddate").": ");
      $g_layout->td($project[0]["enddate"]);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("template").": ");
      $g_layout->td($project[0]["template"]);
      $g_layout->output('</tr><tr>');

      $node = &getNode("project.project_personcontact");
      $node->m_attribList["projectid"]->m_flags |= AF_HIDE_LIST;
      $node->m_flags |= NF_NO_VIEW;
      $data = $node->selectDb("projectid=".$projectid);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("contacts").": ", 'valign="top"');
      if (count($data) != 0) $g_layout->td($node->recordList($data, array(), false)); else $g_layout->td(text("none"));

      $node = &getNode("project.project_personemployee");
      $node->m_attribList["projectid"]->m_flags |= AF_HIDE_LIST;
      $data = $node->selectDb("projectid=".$projectid);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("employees").": ", 'valign="top"');
      if (count($data) != 0) $g_layout->td($node->recordList($data, array(), false)); else $g_layout->td(text("none"));

      $node = &getNode("project.phase");
      $node->m_flags |= NF_NO_SEARCH;
      $data = $node->selectDb("projectid=".$projectid);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("phase").": ", 'valign="top"');
      if (count($data) != 0) $g_layout->td($node->recordList($data, array(), false)); else $g_layout->td(text("none"));

      $node = &getNode("document.document");
      $data = $node->selectDb("project_id=".$projectid);
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("documents").": ", 'valign="top"');
      if (count($data) != 0) $g_layout->td($node->recordList($data, array(), false)); else $g_layout->td(text("none"));

      $node = &getNode("timereg.hours");
      $node->m_attribList["activityid"]->m_flags |= AF_HIDE_LIST;
      $node->m_attribList["remark"]->m_flags |= AF_HIDE_LIST;
      $query = "SELECT phase.id, phase.name, project.name as projname FROM phase LEFT JOIN project ON phase.projectid = project.id WHERE phase.projectid = ".$projectid;
      $phases = $g_db->getRows($query);
      $data = array();
      for ($i=0;$i<count($phases);$i++)
      {
        $sql = "SELECT sum(time) as sumtime FROM hours WHERE phaseid = ".$phases[$i]["id"];
        $res = $g_db->getRows($sql);
        $data[$i]["phaseid"]["phaseid"] = $phases[$i]["id"];
        $data[$i]["phaseid"]["phasename"] = $phases[$i]["name"];
        $data[$i]["phaseid"]["projname"] = $phases[$i]["projname"];
        $data[$i]["time"]["hours"] = floor($res[0]["sumtime"]/60);
        $data[$i]["time"]["minutes"] = $res[0]["sumtime"] - ($data[$i]["time"]["hours"]*60);
      }
      $g_layout->output('</tr><tr>');
      $g_layout->td(text("hours").": ", 'valign="top"');
      if (count($data) != 0) $g_layout->td($node->recordList($data, array(), false)); else $g_layout->td(text("none"));

      $g_layout->output('<br></tr></table>');
      $g_layout->output('<br><form method="get">');
      $g_layout->output(session_form(SESSION_BACK));
      $g_layout->output('<input type="submit" value="&lt;&lt; '.text("back").'"></form>');
      $g_layout->ui_bottom();
    }
  }

?>
