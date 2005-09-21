<?php

  class employeestats extends atkNode
  {   
    var $m_viewstart = ""; 
    var $m_viewend = "";
    var $m_zoom = "";
    var $m_employeeid = "";
    var $m_resolution = "";
  
    function employeestats()
    {
      global $g_sessionManager, $ATK_VARS;
      $this->atkNode("employeestats", NF_READONLY);
            
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
      
      if ($ATK_VARS["employee_sel"]!="")
      {        
        // A selection was made with the dropdown.
        userelation("atkmanytoonerelation");
        $empattr = new atkManyToOneRelation("employee_sel", "employee.employee");
        $sel = $empattr->fetchValue($ATK_VARS);
        $this->m_employeeid = $sel["id"];        
      }
      else
      {
        $this->m_employeeid = $g_sessionManager->stackVar("employeeid");
        if ($this->m_employeeid=="") 
        {
          $user=getUser();
          $this->m_employeeid = $user["id"];
        }      
      }
      
      $this->m_resolution = $g_sessionManager->stackVar("resolution");
      if ($this->m_resolution=="") $this->m_resolution = "auto";
      
      $this->setSecurityAlias("employee.employee");
    }
    
    function action_stats(&$handler)
    {
      atkimport("atk.layout");
      $layout = &layout::getInstance();
      
      include_once("achievotools.inc");
      
      $layout->initGui();
      $layout->ui_top(text("parameters", "employeestats", "employee")); 
      
      $startdate = $this->m_viewstart;
      $enddate = $this->m_viewend;
      $employeeid = $this->m_employeeid;
      
      $layout->output('<br>'.$this->navigator().'<br>');
      
      $layout->ui_bottom();
      
      $empnode = &getNode("employee.employee");
      
      $descriptor="";
      if ($employeeid!="")
      {
        // Get descriptor. 
        $recs = $empnode->selectDb("person.id=".$employeeid, "", "", "", $empnode->descriptorFields());
        $descriptor = $empnode->descriptor($recs[0]);
      }
      
      $layout->ui_top(text("stats").($descriptor!=""?" - ".$descriptor:""));
         
      $backbtn = "";
      if (atkLevel()>0)
      {
        $backbtn = '<br><form method="get">'.session_form(SESSION_BACK).
                '<input type="submit" value="&lt;&lt; '.text('back').'"></form>';
      }
      
      $output.=$backbtn;            
      
      $params["employeeid"] = $employeeid;
      $params["viewstart"] = $startdate;
      $params["viewend"] = $enddate;
      $params["resolution"] = $this->m_resolution;
      
      $detailparams["startdate"] = $startdate;
      $detailparams["enddate"] = $enddate;        
      $detailparams["userid"] = $employeeid;
      
      $prjdata = $empnode->graph_prjtime($params, true);
      
      if (count($prjdata)==0)
      {
        // There is no data.
        $output.='<br><br>'.sprintf(text('nodata', $this->m_type), $descriptor).'<br>';
      }
      else
      {
            
        useattrib("graph.dataGraphAttribute");            
                
        // PROJECT STATS
        $prjstats = &new dataGraphAttribute("prjtimedistribution", "prjtime", $params, "auto");
        $prjstats->setSource("employee.employee");                
        
        $output.= "<br><br>".$prjstats->showGraph();
        
        // Textual version:
        $output.= "<br><br>".$this->_tableStats(text("project"), $prjdata, array("name"), $detailparams, "projectid");        
        
        // ACTIVITY STATS
        $activitystats = &new dataGraphAttribute("activitytimedistribution", "activitytime", $params, "auto");      
        $activitystats->setSource("employee.employee");
        
        $output.= "<br><br>".$activitystats->showGraph();
        
        // Textual version:
        $output.= "<br><br>".$this->_tableStats(text("activity"), $empnode->graph_activitytime($params, true), array("name"), $detailparams, "activity_id");              
        
        // WEEKDAY STATS
        $dowstats = &new dataGraphAttribute("dowdistribution", "dowtime", $params, "auto");      
        $dowstats->setSource("employee.employee");
        
        $output.= "<br><br>".$dowstats->showGraph();
        
        // Textual version:
        $output.= "<br><br>".$this->_tableStats(text("day"), $empnode->graph_dowtime($params, true), array("dow"), $detailparams);
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
      userelation("atkmanytoonerelation");
      $startdate = $this->m_viewstart;
      $enddate = $this->m_viewend;
      
      $empattr = new atkManyToOneRelation("employee_sel", "employee.employee");
      
      $dummyrec = array("viewstart"=>array("year"=>substr($startdate,0,4),
                                           "month"=>substr($startdate,5,2),
                                           "day"=>substr($startdate,8,2)),
                        "viewend"=>array("year"=>substr($enddate,0,4),
                                         "month"=>substr($enddate,5,2),
                                         "day"=>substr($enddate,8,2)),
                        "employee_sel"=>array("id"=>$this->m_employeeid));
                                                

      $startdateatt = new atkDateAttribute("viewstart","F d Y","d F Y", 0, date("Ymd"));
      $enddateatt = new atkDateAttribute("viewend","F d Y","d F Y", 0, date("Ymd"));
      
      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">';
      $output.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
      
      $output.= '<table border="0"><tr><td class="table">'.text("timespan").': </td><td class="table">'.$startdateatt->edit($dummyrec).' &nbsp;'.
                text("until").'&nbsp; '.$enddateatt->edit($dummyrec).'</td></tr>';
                
      if ($this->allowed("view_all"))
      {
        $output.= '<tr><td class="table">'.text("employee").': </td><td>'.$empattr->edit($dummyrec).'</td></tr>';
      }
      
      $output.= '</table>';
                
      $output.= ' &nbsp;<input type="submit" value="'.text("refresh").'">';
      
      $output.= '</form>';
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
      
      if ($detailparams["userid"]=="") $detailparams["userid"]="all"; // workaround, the hoursurvey will select current user if no user is passed.
      
      $table = &atknew("atk.utils.atktablerenderer"); 
      
      $cells = array();
      
      $cells[0] = array($itemheader, text("time"), text("percentage"));
      if ($mayviewdetail) $cells[0][] = text("details");      
      
      for($i=0, $_i=count($dataset); $i<$_i; $i++)
      {
        $row = array();        
        
        $descriptor = array();
        // datasetdescriptors is an array contain information on which fields in $dataset are
        // the descriptors.
        foreach($datasetdescriptors as $field)
        {
          $descriptor[] = $dataset[$i][$field];
        }
        $value = $dataset[$i]["total"];
                
        $row[] = implode(" ", $descriptor);
        $row[] = time_format($value, true);
        $percentage = ($total<=0?0:(($value/$total)*100));
        $row[] = sprintf("%02.02f", $percentage)."%";
        if ($mayviewdetail)
        {          
          $detailparams[$idparam] = $dataset[$i]["id"];
          $detailurl = href(dispatch_url("reports.hoursurvey", "report", $detailparams), text("view"), SESSION_NESTED);
          $row[] = $detailurl;
        }
        $cells[] = $row;
      }
      
      $footer = array(text("total"),
                      time_format($total, true),
                      '100.00%');
      if ($mayviewdetail) $footer[] = "";
      
      $cells[] = $footer;
                  
      return $table->render($cells, TBL_DATA|TBL_FOOTER, "recordlist");
    }
    
    
  }

?>