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
  useattrib("graph.graphattribute");

  class project extends atkNode
  {
    function project()
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode("project",NF_EDITAFTERADD);

      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE, 50));
      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_HIDE_LIST, 10));

      $this->add(new atkManyToOneRelation("coordinator","employee.employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $this->add(new atkTextAttribute("description",AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkNumberAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD, 13, 5));      

      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      atk_var_dump($ATK_VARS, "atkvars");
      if ($ATK_VARS["atknodetype"]=="project.project"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit","search", "view")))
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_SEARCHABLE, 15));
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE, 15));
        $this->addFilter("project.status","active");
      }

      $this->add(new atkOneToManyRelation("contacts","project.project_personcontact", "projectid", AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation("employees","project.project_personemployee", "projectid", AF_HIDE_LIST), "planning");
      
      $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD), "planning");      
      $this->add(new atkDateAttribute("enddate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_HIDE_LIST), "planning");      

      $this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE), "planning");

      $this->add(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL|AF_HIDE_VIEW), "planning");
      $this->add(new projecttemplateAttribute("template","project.tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_HIDE_SEARCH));
      
      $this->add(new atkOneToManyRelation("todos", "todo.todo", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "todos");
            
      $this->add(new graphAttribute("phasetimedistribution", "phasetime", array("projectid"=>"[id]"), AF_HIDE_ADD|AF_HIDE_LIST), "stats");
      $this->add(new graphAttribute("emptimedistribution", "emptime", array("projectid"=>"[id]"), AF_HIDE_ADD|AF_HIDE_LIST), "stats");

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
      parent::dispatch($postvars, $fullpage);
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
    
    function graph_phasetime($params)
    {      
      usegraph("pie");

      global $g_db;
      $data = $g_db->getrows("SELECT 
                                phase.name, SUM(time) as total
                              FROM 
                                hours, phase
                              WHERE 
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"]."
                              GROUP BY 
                                phase.name
                              ORDER BY 
                                phase.name");
                                
      $values = array();
      $legend = array();
            
      // Determine lookup
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {        
        $values[] = $data[$i]["total"]/60;
        $legend[] = $data[$i]["name"];
      }
      /*
      // Plos x-axis.
      $walker = mktime(12,0,0,substr($data[0]["activitydate"],5,2),
                             substr($data[0]["activitydate"],8,2),
                             substr($data[0]["activitydate"],0,4));
      $lastdate = $data[count($data)-1]["activitydate"];
      $end = mktime(12,0,0,substr($lastdate,5,2),
                           substr($lastdate,8,2),
                           substr($lastdate,0,4));                             

      for ($i=1; $walker<=$end; $i++)
      {       
        $date = date("Y-m-d", $walker);
        $val = $values[$date];
        $y[] = ($val!=""?$val:"");
        $x[] = $date;
        $walker = mktime(12,0,0,date("m", $walker), date("d", $walker)+1, date("Y", $walker));
      }
        */            
      $graph = new PieGraph(450,250,"auto");
      
      global $g_theme;      
      $graph->SetColor($g_theme["BgColor"]);

      // Use an integer X-scale
      $graph->SetScale("textlin");
 
       // Set title 
      $graph->title->Set(text("registeredtimeperphase"));

      // Use built in font
      $graph->title->SetFont(FF_FONT1,FS_BOLD);

      // Slightly adjust the legend from it's default position in the
      // top right corner to middle right side
      //$graph->legend->Pos(0.05,0.5,"right","center");

       // Display every 10:th datalabel
      // $graph->xaxis->SetTextTickInterval(1);
       //$graph->xaxis->SetTextLabelInterval(14);
       //$graph->xaxis->SetTickLabels($x);
       //$graph->xaxis->SetLabelAngle(90);

       // Create a red line plot
       $p1 = new PiePlot($values);
       $p1->setCenter(0.3,0.5);
       $p1->SetLegends($legend);
       //$p1->SetColor("red");
  //     $p1->SetLegend("Total project time");
      
       $graph->Add($p1);     

       // Finally output the  image
       $graph->Stroke();
       return true;
    }
    
    function graph_emptime($params)
    {      
      usegraph("pie");

      global $g_db;
      $data = $g_db->getrows("SELECT 
                                person.id, person.firstname, person.lastname, SUM(time) as total
                              FROM 
                                hours, phase, person
                              WHERE 
                                hours.phaseid = phase.id
                                AND hours.userid = person.id
                                AND phase.projectid = ".$params["projectid"]."
                              GROUP BY 
                                person.id
                              ORDER BY 
                                person.lastname");
                                
      $values = array();
      $legend = array();
            
      // Determine lookup
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {        
        $values[] = $data[$i]["total"]/60;
        $legend[] = $data[$i]["firstname"].' '.$data[$i]["lastname"];
      }
      /*
      // Plos x-axis.
      $walker = mktime(12,0,0,substr($data[0]["activitydate"],5,2),
                             substr($data[0]["activitydate"],8,2),
                             substr($data[0]["activitydate"],0,4));
      $lastdate = $data[count($data)-1]["activitydate"];
      $end = mktime(12,0,0,substr($lastdate,5,2),
                           substr($lastdate,8,2),
                           substr($lastdate,0,4));                             

      for ($i=1; $walker<=$end; $i++)
      {       
        $date = date("Y-m-d", $walker);
        $val = $values[$date];
        $y[] = ($val!=""?$val:"");
        $x[] = $date;
        $walker = mktime(12,0,0,date("m", $walker), date("d", $walker)+1, date("Y", $walker));
      }
        */            
      $graph = new PieGraph(450,250,"auto");
      
      global $g_theme;      
      $graph->SetColor($g_theme["BgColor"]);

      // Use an integer X-scale
      $graph->SetScale("textlin");
 
       // Set title 
      $graph->title->Set(text("registeredtimeperemp"));

      // Use built in font
      $graph->title->SetFont(FF_FONT1,FS_BOLD);

      // Slightly adjust the legend from it's default position in the
      // top right corner to middle right side
      //$graph->legend->Pos(0.05,0.5,"right","center");

       // Display every 10:th datalabel
      // $graph->xaxis->SetTextTickInterval(1);
       //$graph->xaxis->SetTextLabelInterval(14);
       //$graph->xaxis->SetTickLabels($x);
       //$graph->xaxis->SetLabelAngle(90);

       // Create a red line plot
       $p1 = new PiePlot($values);
       $p1->setCenter(0.3,0.5);
       $p1->SetLegends($legend);
       //$p1->SetColor("red");
  //     $p1->SetLegend("Total project time");
      
       $graph->Add($p1);     

       // Finally output the  image
       $graph->Stroke();
       return true;
    }
  }

?>
