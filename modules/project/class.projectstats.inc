<?php

  class projectstats extends atkNode
  {
    var $m_view = "month"; // can be set to month or year.
    var $m_viewyear = ""; 
    var $m_viewmonth = "";
    var $m_zoom = "";
  
    function projectstats()
    {
      global $g_sessionManager;
      $this->atkNode("projectstats", NF_READONLY);
      
      $this->m_view = $g_sessionManager->pageVar("view");
      if ($this->m_view=="") $this->m_view = "month";
      $this->m_viewyear = $g_sessionManager->pageVar("viewyear");
      if ($this->m_viewyear=="") $this->m_viewyear = date("Y");
      $this->m_viewmonth = $g_sessionManager->pageVar("viewmonth");
      if ($this->m_viewmonth=="") $this->m_viewmonth = date("m");
      $this->m_zoom = $g_sessionManager->pageVar("zoom");      
      
      $this->setSecurityAlias("project.project");
    }
    
    function action_stats()
    {
      global $g_layout;
      
      include_once("achievotools.inc");
      
      $g_layout->initGui();
      $g_layout->ui_top(text("stats"));          
      
      $startdate = $this->rangeStart();
      $enddate = $this->rangeEnd();
      
      $output = $this->navigator();
      
      $output.= $this->topProjects($startdate, $enddate, ($this->m_zoom=="topprojects"?0:10));

      $g_layout->output($output);
      
      $g_layout->ui_bottom();
    }
    
    function navigator()
    {      
    }
    
    function topProjects($start, $end, $max=0)
    {    
      global $g_db, $g_layout;
      
      $query = &atkQuery::create();
      
      $query->addTable("hours");
      $query->addJoin("phase", "", "phase.id = hours.phaseid", false);
      $query->addJoin("project", "", "phase.projectid = project.id", false);
      $query->addField("project.name");
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
        $output.= $g_layout->ret_td($projects[$i]["name"]);
        $output.= $g_layout->ret_td(time_format($projects[$i]["totaltime"], true));
        $output.= $g_layout->tr_bottom();
      }
      
      $output.= $g_layout->data_bottom();
      return $output;
    }
    
    function rangeStart()
    {
      if ($this->m_view == "month")
      {
        return sprintf("%04d-%02d-%02d", $this->m_viewyear, $this->m_viewmonth, 1);
      }
      else
      {
        return sprintf("%04d-%02d-%02d", $this->m_viewyear, $this->m_viewmonth, date("t", mktime(12,0,0,$this->m_viewmonth,1,$this->m_viewyear)));
      }
    }
    
    function rangeEnd()
    {
      if ($this->m_view == "month")
      {
        return sprintf("%04d-%02d-%02d", $this->m_viewyear, $this->m_viewmonth, date("t", mktime(12,0,0,$this->m_viewmonth,1,$this->m_viewyear)));
      }
      else
      {
        return sprintf("%04d-%02d-%02d", $this->m_viewyear, $this->m_viewmonth, date("t", mktime(12,0,0,12,1,$this->m_viewyear)));
      }
    }
  }

?>