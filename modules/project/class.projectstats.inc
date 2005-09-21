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
      $this->m_viewstart = date("Y-m-d",adodb_mktime(0,0,0,date("m")-1,date("d"),date("Y")));
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

  function action_stats(&$handler)
  {
    include_once("achievotools.inc");

    atkimport("atk.layout");
    $layout = &layout::getInstance();

    $layout->initGui();
    $layout->ui_top(text("parameters", "projectstats", "project"));

    $startdate = $this->m_viewstart;
    $enddate = $this->m_viewend;
    $projectid = $this->m_projectid;

    $layout->output('<br>'.$this->navigator().'<br>');

    $layout->ui_bottom();

    $prjnode = &getNode("project.project");

    $descriptor="";
    if ($projectid!="")
    {
      // Get descriptor.
      $recs = $prjnode->selectDb("project.id=".$projectid, "", "", "", $prjnode->descriptorFields());
      $descriptor = " - ".$prjnode->descriptor($recs[0]);
    }

    $layout->ui_top(text("stats").$descriptor);

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

      $params["startdate"] = $startdate;
      $params["enddate"] = $enddate;
      $params["max"] = ($this->m_zoom=="topprojects"?20:10);
      $max = ($this->m_zoom=="topprojects"?0:10);
      useattrib("graph.dataGraphAttribute");
      
      $db = &atkGetDb();

      $query = &$db->createQuery();
      $query->addTable("hours");
      $query->addJoin("phase", "", "phase.id = hours.phaseid", false);
      $query->addJoin("project", "", "phase.projectid = project.id", false);
      $query->addField("project.name");
      $query->addField("project.id");
      $query->addField("sum(time) as totaltime");
      $query->addCondition("hours.activitydate BETWEEN '".$startdate."' AND '".$enddate."'");
      if ($max)
      {
        $query->setLimit(0,$max);
      }
      $query->addOrderBy("totaltime DESC");
      $query->addGroupBy("project.name");      
      $projects = $db->getrows($query->buildSelect());

      for ( $i=0; $i < count($projects); $i=$i+20 )
      {
        $params["from"] = ($this->m_zoom=="topprojects"?$i:0);
        $topProjects = &new dataGraphAttribute("topprojectsdistribution", "topprojects", $params, "pie");
        $topProjects->setSource("project.project");
        atkdebug("<H1>DRAWING TOPPROJECTS!</H1>");

        $output.= "<br>".$topProjects->showGraph();
      }
    }
    else
    {

      $params["projectid"] = $projectid;
      $params["viewstart"] = $startdate;
      $params["viewend"] = $enddate;
      $params["resolution"] = $this->m_resolution;

      $detailparams["projectid"] = $projectid;
      $detailparams["startdate"] = $startdate;
      $detailparams["enddate"] = $enddate;
      $detailparams["userid"] = "all";

      useattrib("graph.dataGraphAttribute");


      // TIMELINE
      $timeline = &new dataGraphAttribute("timeline", "timeline", $params, "line");
      $timeline->setSource("project.project");
      $timeline->addDynamicParam("resolution", array("day", "week", "month", "auto"));

      $output.= "<br>".$timeline->showGraph();

      // PHASE STATS
      $phasestats = &new dataGraphAttribute("phasetimedistribution", "phasetime", $params, "auto");
      $phasestats->setSource("project.project");

      $output.= "<br>".$phasestats->showGraph();

      // Textual version:
      $output.= "<br><br>".$this->_tableStats(text("phase"), $prjnode->graph_phasetime($params, true), array("name"), $detailparams, "phaseid");

      // EMPLOYEE STATS
      $empstats = &new dataGraphAttribute("emptimedistribution", "emptime", $params, "auto");
      $empstats->setSource("project.project");

      $output.= "<br><br>".$empstats->showGraph();

      // Textual version:
      $output.= "<br><br>".$this->_tableStats(text("employee"), $prjnode->graph_emptime($params, true), array("firstname", "lastname"), $detailparams, "userid");

      // ACTIVITY STATS
      $activitystats = &new dataGraphAttribute("activitytimedistribution", "activitytime", $params, "auto");
      $activitystats->setSource("project.project");

      $output.= "<br><br>".$activitystats->showGraph();

      // Textual version:
      $output.= "<br><br>".$this->_tableStats(text("activity"), $prjnode->graph_activitytime($params, true), array("name"), $detailparams, "activity_id");

      // DOW STATS
      $dowstats = &new dataGraphAttribute("dowtimedistribution", "dowtime", $params, "auto");
      $dowstats->setSource("project.project");

      $output.= "<br><br>".$dowstats->showGraph();

      // Textual version:
      $output.= "<br><br>".$this->_tableStats(text("day"), $prjnode->graph_dowtime($params, true), array("dow"), $detailparams);
    }

    $output.="<br>".$backbtn."<br>";

    $layout->output($output);

    $layout->ui_bottom();
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
    $db = &atkGetDb();
    atkdebug("niet in graph maar in topprojects");
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
      $query->setLimit(0,$max);
    }
    $query->addOrderBy("totaltime DESC");
    $query->addGroupBy("project.name");
    $projects = $db->getrows($query->buildSelect());

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
    
    atkimport("atk.layout");
    $layout = &layout::getInstance();

    $output.= $layout->data_top();

    $output.= $layout->tr_top();
    $output.= $layout->ret_td_datatitle("#");
    $output.= $layout->ret_td_datatitle(text("project"));
    $output.= $layout->ret_td_datatitle(text("time"));
    $output.= $layout->tr_bottom();

    for ($i=0, $_i=count($projects); $i<$_i; $i++)
    {
      $output.= $layout->tr_top($i%2);
      $output.= $layout->ret_td($i+1);
      $output.= $layout->ret_td(href("dispatch.php?atknodetype=".$this->atkNodeType().
      "&atkaction=".$this->m_action.
      "&projectid=".$projects[$i]["id"],
      $projects[$i]["name"],
      SESSION_NESTED));
      $output.= $layout->ret_td(time_format($projects[$i]["totaltime"], true));
      $output.= $layout->tr_bottom();
    }

    $output.= $layout->data_bottom();
    return $output;
  }

  function _tableStats($itemheader, $dataset, $datasetdescriptors, $detailparams, $idparam="")
  {
    global $g_securityManager;

    // Totals count
    $total = 0;
    for($i=0, $_i=count($dataset); $i<$_i; $i++)
    {
      $total+=$dataset[$i]["total"];
    }

    $mayviewdetail = ($idparam!="" && $g_securityManager->allowed("reports.hoursurvey", "view_all"));
    
    atkimport("atk.layout");
    $layout = &layout::getInstance();

    $res = $layout->data_top();
    $res.= $layout->tr_top();
    $res.= $layout->ret_td_datatitle($itemheader);
    $res.= $layout->ret_td_datatitle(text("time"));
    $res.= $layout->ret_td_datatitle(text("percentage"));
    if ($mayviewdetail) $res.= $layout->ret_td_datatitle(text("details"));
    $res.= $layout->tr_bottom();

    for($i=0, $_i=count($dataset); $i<$_i; $i++)
    {
      $res.= $layout->tr_top($i%2!=0);

      $descriptor = array();
      // datasetdescriptors is an array contain information on which fields in $dataset are
      // the descriptors.
      foreach($datasetdescriptors as $field)
      {
        $descriptor[] = $dataset[$i][$field];
      }
      $value = $dataset[$i]["total"];

      $res.= $layout->ret_td(implode(" ", $descriptor));
      $res.= $layout->ret_td(time_format($value, true), 'align="right"');
      $percentage = ($total<=0?0:(($value/$total)*100));
      $res.= $layout->ret_td(sprintf("%02.02f", $percentage)."%", 'align="right"');
      if ($mayviewdetail)
      {
        $detailparams[$idparam] = $dataset[$i]["id"];
        $detailurl = href(dispatch_url("reports.hoursurvey", "report", $detailparams), text("view"), SESSION_NESTED);
        $res.= $layout->ret_td($detailurl);
      }
      $res.= $layout->tr_bottom();
    }

    $res.= $layout->tr_top();
    $res.= $layout->ret_td_datatitle(text("total"));
    $res.= $layout->ret_td_datatitle(time_format($total, true), 'align="right"');
    $res.= $layout->ret_td_datatitle('100.00%', 'align="right"');
    if ($mayviewdetail) $res.= $layout->ret_td_datatitle("");
    $res.= $layout->tr_bottom();

    $res.= $layout->data_bottom();

    return $res;
  }


}

?>
