<?php

  include_once("achievotools.inc");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("project.dependencyattrib");
  useattrib("project.projecttemplateattribute");
  useattrib("atknumberattribute");
  useattrib("graph.datagraphattribute");

  class project extends atkNode
  {
  	// when set to true the contact relation will be obligatory in the edit screen
  	// but not in the add screen (because it is hidden there)
  	var $m_contact_obligatory = true;
  	
    function project()
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode("project",NF_EDITAFTERADD);

      $this->add(new atkAttribute("id",AF_AUTOKEY));
      
      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE, 50));
      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_HIDE_LIST, 10));

      $this->add(new atkManyToOneRelation("coordinator","employee.employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $this->add(new atkTextAttribute("description",TEXT_LARGE, AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkNumberAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD, 13, 5));      

      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      if ($ATK_VARS["atknodetype"]=="project.project"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit","search", "view")))
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_SEARCHABLE, 15));
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE, 15));
        $this->addFilter("project.status","active");
      }
	
		  if (($ATK_VARS['atkaction'] == "edit" || $ATK_VARS['atkaction'] == "update") && $this->m_contact_obligatory)
		  {
		  	$flags = AF_OBLIGATORY;
	  	}
      $this->add(new atkOneToManyRelation("contacts","project.project_personcontact", "projectid", AF_HIDE_LIST|AF_HIDE_ADD|$flags));
      $this->add(new atkOneToManyRelation("employees","project.project_personemployee", "projectid", AF_HIDE_LIST), "planning");
      
      $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD), "planning");      
      $this->add(new atkDateAttribute("enddate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_HIDE_LIST), "planning");      

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
                   "status"=>"active");
    }

    function action_stats()
    {
      include_once moduleDir('project').'projectstats.inc';   // MILESTONES FUNCTIONS
    }

    function action_planning()
    {
      //include_once moduleDir('project').'projectplanning.inc';   // MILESTONES FUNCTIONS
      global $g_layout;            
      $prjid = $this->m_postvars["selectedprojectid"];
      $imgurl = moduleDir("project")."ganttchart.php?projectid=".$prjid;
      
      $records = $this->selectDb("project.id=".$prjid);
      $record = $records[0];
      
      $projectinfo = '<br><table border=0>';
      $projectinfo.= '<tr><td align="right"><b>'.text("name").': </b></td><td>'.$record['name'].'</td></tr>';
      $projectinfo.= '<tr><td align="right"><b>'.text("coordinator").': </b></td><td>'.$record['coordinator']['lastname'].','.$record['coordinator']['firstname'].'</td></tr>';
      $projectinfo.= '</table><br>';   
      
      $legend = text("legend").": [".text("booked").", ".text("hoursleft")."]<br>";
      $legend.= text("ganttexplanation");
      
      $g_layout->ui_top(text("title_projectplanning"));
      $g_layout->output('<br>'.$projectinfo.'<br><img src="'.$imgurl.'"><br><br>'.$legend);
      $g_layout->ui_bottom();
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
      return true;
    }

    function fixed_price_edit($rec)
    {
      return $this->m_attribList["fixed_price"]->edit($rec)." ".atkconfig("currency_symbol");
    }
    
    function graph_phasetime($params)
    {          
      global $g_db;
      $data = $g_db->getrows("SELECT 
                                phase.name, SUM(time) as total
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
      
      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {        
        $dataset[$data[$i]["name"]] = $data[$i]["total"];        
      }
      
      return array("registeredtimeperphase"=>$dataset);
    }
    
    function graph_emptime($params)
    {      
      global $g_db;
      $data = $g_db->getrows("SELECT 
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
                         
       // convert records to graph-compatible array.
      $dataset = array();                         
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {        
        $dataset[$data[$i]["firstname"]." ".$data[$i]["lastname"]] = $data[$i]["total"];        
      }
      
      return array("registeredtimeperemp"=>$dataset);
    }
    
    function graph_activitytime($params)
    {            
      global $g_db;
      $data = $g_db->getrows("SELECT 
                                activity.name, SUM(time) as total
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
       
      // convert records to graph-compatible array.
      $dataset = array();       
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {        
        $dataset[$data[$i]["name"]] = $data[$i]["total"];        
      }
      
      return array("registeredtimeperactivity"=>$dataset);
    }
    
    function graph_timeline($params)
    {
      global $g_db;
   
      // First find out if a timeline would make more sense per week, per 
      // month or per day. This all depends on the amount of time between the 
      // first and last time entry.
      $range = $g_db->getrows("SELECT 
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
        $daysbetween = (mktime(12,0,0,$maxmonth,$maxday,$maxyear)-mktime(12,0,0,$minmonth,$minday,$minyear))/$secondsinaday;      
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
      $data = $g_db->getrows($query);
            
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
        return text($months[$month-1])." '".$shortyear;
      }
      else
      {
        return $label;
      }
    }
  }

?>
