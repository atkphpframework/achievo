<?php

  class projectstats extends atkNode
  {   
    var $m_viewstart = ""; 
    var $m_viewend = "";
    var $m_zoom = "";
    var $m_projectid = "";
    var $m_resolution = "";
  
    function projectstats()
    {
      global $g_sessionManager;
      $this->atkNode("projectstats", NF_READONLY);
            
      $this->m_viewstart = $g_sessionManager->stackVar("viewstart");    
      
      if ($this->m_viewstart=="") 
      {
        $this->m_viewstart = date("Y-m-d",mktime(0,0,0,date("m")-1,date("d"),date("Y")));
      }
      else if (is_array($this->m_viewstart))
      {
        $this->m_viewstart = sprintf("%04d-%02d-%02d", $this->m_viewstart["year"], 
                                                       $this->m_viewstart["month"],
                                                       $this->m_viewstart["day"]);
      }
      
      $this->m_viewend = $g_sessionManager->stackVar("viewend");
      if ($this->m_viewend=="") 
      {
        $this->m_viewend = date("Y-m-d");
      }
      else if (is_array($this->m_viewend))
      {
        $this->m_viewend = sprintf("%04d-%02d-%02d", $this->m_viewend["year"], 
                                                     $this->m_viewend["month"],
                                                     $this->m_viewend["day"]);
      }
      
      $this->m_zoom = $g_sessionManager->stackVar("zoom");      
      
      $this->m_projectid = $g_sessionManager->stackVar("projectid");
      
      $this->m_resolution = $g_sessionManager->stackVar("resolution");
      if ($this->m_resolution=="") $this->m_resolution = "auto";
      
      $this->setSecurityAlias("project.project");
    }
    
    function action_stats()
    {
      global $g_layout;
      
      include_once("achievotools.inc");
      
      $g_layout->initGui();
      $g_layout->ui_top(text("parameters", "projectstats", "project"));          
      
      $startdate = $this->m_viewstart;
      $enddate = $this->m_viewend;
      $projectid = $this->m_projectid;
      
      $g_layout->output('<br>'.$this->navigator().'<br>');
      
      $g_layout->ui_bottom();
      
      $g_layout->ui_top(text("stats"));
         
      $backbtn = "";
      if (atkLevel()>0)
      {
        $backbtn = '<br><form method="get">'.session_form(SESSION_BACK).
                '<input type="submit" value="&lt;&lt; '.text('back').'"></form>';
      }
      
      $output.=$backbtn;
      
      if ($this->m_projectid=="")
      {      
        $output.= "<br>".$this->topProjects($startdate, $enddate, ($this->m_zoom=="topprojects"?0:10));
      }
      else
      {
      
        $params["projectid"] = $projectid;
        $params["viewstart"] = $startdate;
        $params["viewend"] = $enddate;
        $params["resolution"] = $this->m_resolution;
        
        useattrib("graph.dataGraphAttribute");
        
        $timeline = &new dataGraphAttribute("timeline", "timeline", $params, "line");
        $timeline->setSource("project.project");
        $timeline->addDynamicParam("resolution", array("day", "week", "month", "auto"));
        
        $output.= "<br>".$timeline->showGraph();                      
            
        $phasestats = &new dataGraphAttribute("phasetimedistribution", "phasetime", $params, "auto");
        $phasestats->setSource("project.project");
        
        $output.= "<br>".$phasestats->showGraph();
      
        $empstats = &new dataGraphAttribute("emptimedistribution", "emptime", $params, "auto");
        $empstats->setSource("project.project");
        
        $output.= "<br><br>".$empstats->showGraph();
        
        $activitystats = &new dataGraphAttribute("activitytimedistribution", "activitytime", $params, "auto");      
        $activitystats->setSource("project.project");
        
        $output.= "<br><br>".$activitystats->showGraph();
      }      
      
      $output.="<br>".$backbtn."<br>";
      
      $g_layout->output($output);   
      
      $g_layout->ui_bottom();
    }
    
    function _paramForm($contents)
    {
      return '<form method="get">'.session_form().$contents.
                '<input type="submit" value="'.text('refresh').'"></form>';
    }
    
    function navigator()
    {      
      useattrib("atkdateattribute");
      $startdate = $this->m_viewstart;
      $enddate = $this->m_viewend;
      $dummyrec = array("viewstart"=>array("year"=>substr($startdate,0,4),
                                           "month"=>substr($startdate,5,2),
                                           "day"=>substr($startdate,8,2)),
                        "viewend"=>array("year"=>substr($enddate,0,4),
                                         "month"=>substr($enddate,5,2),
                                         "day"=>substr($enddate,8,2)));

      $startdateatt = new atkDateAttribute("viewstart","F d Y","d F Y", 0, date("Ymd"));
      $enddateatt = new atkDateAttribute("viewend","F d Y","d F Y", 0, date("Ymd"));
      
      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">';
      $output.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
      
      $output.= text("timespan").': '.$startdateatt->edit($dummyrec).' &nbsp;'.
                text("until").'&nbsp; '.$enddateatt->edit($dummyrec);
                
      $output.= ' &nbsp;<input type="submit" value="'.text("refresh").'">';
      
      $output.= '</form>';
      return $output;
    }
    
    function topProjects($start, $end, $max=0)
    {    
      global $g_db, $g_layout;
      
      $query = &atkQuery::create();
      
      $query->addTable("hours");
      $query->addJoin("phase", "", "phase.id = hours.phaseid", false);
      $query->addJoin("project", "", "phase.projectid = project.id", false);
      $query->addField("project.name");
      $query->addField("project.id");
      $query->addField("sum(time) as totaltime");
      $query->addCondition("hours.activitydate BETWEEN '".$start."' AND '".$end."'");
      if ($max) 
      {
        $query->setLimit(0,$max);
      }
      $query->addOrderBy("totaltime DESC");
      $query->addGroupBy("project.name");
      $projects = $g_db->getrows($query->buildSelect());
            
      if ($max)
      {
        $output ="<b>".sprintf(text("top_projects"), $max).'</b> (';
        $output.=href(dispatch_url($this->atkNodeType(), "stats", array("zoom"=>"topprojects")), text("complete_list"));
      }
      else
      {
        $output ="<b>".text("allprojects").'</b> (';
        $output.=href(dispatch_url($this->atkNodeType(), "stats", array("zoom"=>"0")), sprintf(text("top_projects"), 10));
      }
      $output.=')<br>';
      
      $output.= $g_layout->data_top();
   
      $output.= $g_layout->tr_top();
      $output.= $g_layout->ret_td_datatitle("#");
      $output.= $g_layout->ret_td_datatitle(text("project"));
      $output.= $g_layout->ret_td_datatitle(text("time"));
      $output.= $g_layout->tr_bottom();
      
      for ($i=0, $_i=count($projects); $i<$_i; $i++)
      {        
        $output.= $g_layout->tr_top($i%2);
        $output.= $g_layout->ret_td($i+1);
        $output.= $g_layout->ret_td(href("dispatch.php?atknodetype=".$this->atkNodeType().
                                                     "&atkaction=".$this->m_action.
                                                     "&projectid=".$projects[$i]["id"], 
                                         $projects[$i]["name"],
                                         SESSION_NESTED));
        $output.= $g_layout->ret_td(time_format($projects[$i]["totaltime"], true));
        $output.= $g_layout->tr_bottom();
      }
      
      $output.= $g_layout->data_bottom();
      return $output;
    }
    
    
  }

?>