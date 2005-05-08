<?php

  userelation("atkmanytoonerelation");  
  useattrib("atkdurationattribute");
  useattrib("atkboolattribute");
  useattrib("atklistattribute");
  
  // special case of the duration attrib, that uses a sum(), to facilitate aggregations
  class sumAttribute extends atkDurationAttribute 
  {
    var $m_groupbyfields;
    
    function sumAttribute($name, $groupbyfields)
    {      
      $this->atkDurationAttribute($name, atkconfig("timereg_resolution", "15m"), '10h',AF_OBLIGATORY|AF_TOTAL);
      
      $this->m_groupbyfields = $groupbyfields;
    }
    
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {      
      $query->addField("SUM(".$tablename.".".$this->fieldName().")","","",$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));    
      
      foreach($this->m_groupbyfields as $field)
      {
        if ($field!=$this->fieldName()) // can't aggregate on ourselves
        {
          $query->addGroupBy($tablename.".".$field);
        }
      }
    }
    
    function db2value($rec)
    {
      $rec["time"] = $rec['SUM(hours.time)'];
      return parent::db2value($rec);
    }
  }
  
  // hack: the activity attribute is a hack, and not a relation. It is populated by the project phase attrib.
  // Because of this, it is not very well sortable.
  class sortableActivity extends atkListAttribute
  {
    function sortableActivity()
    {
      $this->atkListAttribute("activityid",array("dummy","dummy","dummy","dummy","dummy"),array(0,0,0,0,0),AF_OBLIGATORY, 10);
    }
    
    function getOrderByStatement()
    {
      return "C.name";
    }
  }
  
  class fakeAttrib extends atkAttribute 
  {
    function fakeAttrib($name, $flags=0, $size="")
    {
      $this->atkAttribute($name, $flags|AF_TOTAL, $size);
    }
    
    function load() {}
    function store() {}  
    function addtoeditarray() {}
  }
  
  class hoursurvey extends atkNode
  {
    function hoursurvey()
    {
      $this->atkNode("hoursurvey");
    }

    function action_report(&$handler)
    {
      global $g_layout, $g_user, $g_sessionManager;

      // Manual sort config
      $orderby =            $this->m_postvars["orderby"];
      $orderdirection =     $this->m_postvars["orderdirection"];
      $subtotal =           $this->m_postvars["subtotal"];

      $activity_id =        $g_sessionManager->pageVar("activity_id");
      $projectid =          $g_sessionManager->pageVar("projectid");
      $selectedprojectid =  $g_sessionManager->pageVar("selectedprojectid");
      $phaseid =            $g_sessionManager->pageVar("phaseid");
      $userid =             $g_sessionManager->pageVar("userid");
      $nameswitch =         $g_sessionManager->pageVar("nameswitch");
      $roleid =             $g_sessionManager->pageVar("roleid");
      $remark =             $g_sessionManager->pageVar("remark");
      $aggregate =          $g_sessionManager->pageVar("aggregate");
      $startdate =          $g_sessionManager->pageVar("startdate");
      $enddate =            $g_sessionManager->pageVar("enddate");
      $col =                $g_sessionManager->pageVar("col");
      // the selected customer_id is found as "customer.id='3'" in the postvars
      $organizationarr =    decodeKeyValuePair($this->m_postvars["organizationid"]);
      $organizationid =     $organizationarr["organization.id"];
      $department_name =    $g_sessionManager->pageVar("departmentid");
      $functionlevelswitch =$g_sessionManager->pageVar("functionlevelswitch");
      $outputType =         $this->m_postvars["outputType"];

      if (empty($outputType))
      {
        $outputType=0;   //default=standard
      }

      $hoursnode = &getNode("timereg.hours");
      $hoursnode->m_postvars = $this->m_postvars; // Share postvars.

      // Hack: make sure the activities, which are populated by the project/phase column, are properly loaded.
      $hoursnode->add(new sortableActivity());


      $columnConfig = &$hoursnode->getColumnConfig();

      if (is_array($orderby))
      {
        $columnConfig->clearOrder();
        for ($i=0, $_i=count($orderby); $i<$_i; $i++)
        {
          if ($orderby[$i]!="-")
          {
            $columnConfig->setSortDirection($orderby[$i], $orderdirection[$i]);
            $columnConfig->setSortOrder($orderby[$i], ($i+1));

            $columnConfig->setSubTotal($orderby[$i], ($subtotal[$i]==1));
          }
        }
      }

      // Determine collumn suppresslist:
      $arr_col = $hoursnode->getHourSurveyColumns();

      $suppress = array();
      if (!is_array($col)||count($col)==0)
      {
        // No collumns selected. Default->select all.
        for($i=0,$_i=count($arr_col);$i<$_i;$i++)
        {
          $col[]=$arr_col[$i];
        }
      }
      for($i=0,$_i=count($arr_col);$i<$_i;$i++)
      {
        if (!in_array($arr_col[$i],$col)) $suppress[]= $arr_col[$i];
      }

      if (is_array($startdate))
      {
        $startdate = $startdate["year"]."-".sprintf("%02d",$startdate["month"])."-".sprintf("%02d",$startdate["day"]);
      }
      else
      {
        if (!isset($startdate)||$startdate=="")
        {
          $startdate =  date("Y-m-d" ,time()-(86400*7));
        }
      }
      if (is_array($enddate))
      {
        $enddate = $enddate["year"]."-".sprintf("%02d",$enddate["month"])."-".sprintf("%02d",$enddate["day"]);
      }
      else
      {
        if (!isset($enddate)||$enddate=="")
        {
          $enddate = date('Y-m-d');
        }
      }

      if (!isset($userid)||$userid=="")
      {
        $userid=$g_user["id"];
      }

      if (!isset($nameswitch)||$nameswitch=="")
      {
        $nameswitch="name";
      }

      // If we came from 'search all projects', a selectedprojectid var is filled.
      if (isset($selectedprojectid)&&$selectedprojectid!="")
      {
        $projectid=$selectedprojectid;
      }

      $view_all = $this->allowed('view_all');

      if ($aggregate==1)
      {
        // determine group by fields.
        $groupfields = $columnConfig->getOrderFields();

        $hoursnode->add(new sumAttribute("time", $groupfields));

        $suppress = array();

        // We must suppress everything we don't sort on.
        foreach(array_keys($hoursnode->m_attribList) as $field)
        {
          if ($columnConfig->getOrder($field)<=0 && $field!="time") // time is displayed even in aggregate mode
          {
            $suppress[] = $field;
          }
        }
      }

      // DIRTY HACK:
      // userid and activitydate are HIDE_LIST by default.
      // we need to show them however (unless they are unchecked, but
      // that is handled by the suppresslist). userid is shown, because
      // it is readded as a manytoonerelation later on.
      //
      // TODO FIXME: AF_HIDE_LIST, AF_HIDE_EDIT etc are not implemented cleanly.
      // workarounds like these should not be necessary.
      $hoursnode->m_attribList["activitydate"]->m_flags = ($hoursnode->m_attribList["activitydate"]->m_flags&(~AF_HIDE_LIST));

      if ($outputType==0)
      {
        $g_layout->initGUI();
        $g_layout->ui_top(text('parameters', $this->m_type, $this->m_module));

        $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');
        $g_layout->output(session_form());

        $this->_addHiddenFields($g_layout);
        $g_layout->table_simple();
        $g_layout->output('<tr>');
        $g_layout->td('<b>'.text('sethoursfilter').'</b>', 'colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td('<hr>'.text('searchbyprojectorphase').':','colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('project').':');
        
        // we have to pass a 'dummy' record to the attributes to set their default value.
        $dummyrec = array("projectid"=>$projectid,
        "organizationid"=>array("id"=>$organizationid), // manytoonerelation needs subarray
        "phaseid"=>Array("phaseid"=>$phaseid), // is implemented as an array in projphaseattrib.
        "startdate"=>array("year"=>substr($startdate,0,4),
        "month"=>substr($startdate,5,2),
        "day"=>substr($startdate,8,2)),
        "enddate"=>array("year"=>substr($enddate,0,4),
        "month"=>substr($enddate,5,2),
        "day"=>substr($enddate,8,2)),
        "aggregate"=>$aggregate,
        "remark"=>$remark);

        $projectatt = new atkProjectAttribute('projectid');

        $sortfieldcount = max(3, $columnConfig->countSortAttribs());

        for ($i=0; $i<$sortfieldcount; $i++)
        {
          $dummyrec["orderby[$i]"]=$columnConfig->getAttributeByOrder($i+1);
          $dummyrec["orderdirection[$i]"]=$columnConfig->getDirectionByOrder($i+1);
          $dummyrec["subtotal[$i]"]=($columnConfig->hasSubTotalByOrder($i+1)?1:0);
        }

        $g_layout->td($projectatt->edit($dummyrec));
        $g_layout->output('</tr>');
        $this->_addOr($g_layout);
        $g_layout->output('<tr>');
        $g_layout->td(text('phase').':');
        $phaseatt = $hoursnode->m_attribList["phaseid"];
        $phaseatt->m_flags = $phaseatt->m_flags&~AF_JS_UPDATEACTIVITY;
        $phaseatt->m_nested = false;
        $g_sessionManager->stackVar("reportnodetype","hoursurvey");
        $g_layout->td($phaseatt->edit($dummyrec));

        $g_layout->output('</tr>');
        $this->_addOr($g_layout);
        $g_layout->output('<tr>');

        $g_layout->td(text("organization").':');
        $organizationatt = new atkManyToOneRelation("organizationid", "organization.organization");
        $g_layout->td('<SELECT name="roleid">'.$this->get_roles($roleid).'</SELECT> '.

        $organizationatt->edit($dummyrec));
        $g_layout->output('</tr>');
        $this->_addOr($g_layout);
        $g_layout->output('<tr>');

        $g_layout->td(text("department").':');
        $departmentatt = new atkManyToOneRelation("departmentid","employee.department");
        $g_layout->td('<SELECT name="departmentid">'.$this->get_departments($department_name).'</SELECT>');
        $g_layout->output('</tr><tr>');

        $g_layout->td('<hr>','colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('employee').':');

        if($view_all)
        {
          $g_layout->td('<SELECT name="nameswitch"><OPTION VALUE="name">'.text("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.text("supervisor").': </SELECT>'.
          ' <SELECT name="userid">'.$this->get_employees($userid).'</SELECT>');
        }
        else
        {
          $userid = $g_user["id"];
          $g_layout->td($g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">');
        }
        $g_layout->output('</tr>');
        $this->_addOr($g_layout);
        $g_layout->output('<tr>');
        
        // selecting functionlevel:
        $g_layout->td(text('functionlevel').':');
        $g_layout->td('<SELECT name="functionlevelswitch">' . $this->get_functionlevels($functionlevelswitch).'</SELECT>');
        
        $g_layout->output('</tr><tr>');

        
        $g_layout->td('<hr>','colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('activity').': ');
        $g_layout->td('<SELECT name="activity_id">'.$this->get_activities($activity_id).'</SELECT>');
        $g_layout->output('</tr><tr>');
        $g_layout->td('<hr>','colspan="2"');
        $g_layout->output('</tr><tr>');

        $g_layout->td(text("timespan").': ');

        $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY);
        $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY);

        $g_layout->td('<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td>'.$startdateatt->edit($dummyrec).'</td><td class="table"> &nbsp;'.
        text("until").
        '&nbsp; </td><td align="right">'.$enddateatt->edit($dummyrec).'</td></tr></table>');

        $g_layout->output('</tr><tr>');
        $g_layout->register_script(atkconfig("atkroot")."javascript/timereg.js");
        $g_layout->td(text("lookbackperiod").': ');
        $output = '<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td width="33%">';
        $output .= '<select name="startperiod" onchange="setStartEndDates(\'enddate\', \'startperiod\', '.date("Ymd").'); document.entryform.endperiod.value = 0; setStartEndDates(\'startdate\', \'endperiod\', '.date("Ymd").');">
                        <option value="0" selected>---</option> 
                        <option value="1">1 '.strtolower(text("week")).'</option> 
                        <option value="2">2 '.strtolower(text("weeks")).'</option>
                        <option value="3">3 '.strtolower(text("weeks")).'</option>
                        <option value="4">4 '.strtolower(text("weeks")).'</option>
                        <option value="5">1 '.strtolower(text("month")).'</option>
                        <option value="6">2 '.strtolower(text("months")).'</option>
                        <option value="7">3 '.strtolower(text("months")).'</option>
                        <option value="8">6 '.strtolower(text("months")).'</option>
                       </select></td>';

        $output .= '<td width="34%" align="center">&nbsp;</td>';
        $output .= '<td width="33%" align="right"><select name="endperiod" onchange="setStartEndDates(\'startdate\', \'endperiod\', '.date("Ymd").'); document.entryform.startperiod.value = 0; setStartEndDates(\'enddate\', \'startperiod\', '.date("Ymd").');">
                        <option value="0" selected>---</option> 
                        <option value="1">1 '.strtolower(text("week")).'</option> 
                        <option value="2">2 '.strtolower(text("weeks")).'</option>
                        <option value="3">3 '.strtolower(text("weeks")).'</option>
                        <option value="4">4 '.strtolower(text("weeks")).'</option>
                        <option value="5">1 '.strtolower(text("month")).'</option>
                        <option value="6">2 '.strtolower(text("months")).'</option>
                        <option value="7">3 '.strtolower(text("months")).'</option>
                        <option value="8">6 '.strtolower(text("months")).'</option>
                       </select></td></tr></table>';

        $g_layout->td($output);
        $g_layout->output('</tr><tr>');
        $g_layout->td('<hr>','colspan="2"');
        $g_layout->output('</tr><tr>');

        if (moduleExists("billing"))
        {
          $rateattr = new atkListAttribute("ratetype", array(text("salary"), text("commercial")),array(0,1), AF_OBLIGATORY);
          $g_layout->td(text('ratetype').': ');
          $g_layout->td($rateattr->edit($dummyrec));
          $g_layout->output('</tr><tr>');
          $g_layout->td('<hr>','colspan="2"');
          $g_layout->output('</tr><tr>');
        }

        $remarkattr = new atkAttribute("remark", 0, 80);
        $g_layout->td(text('remark').': ');
        $g_layout->td($remarkattr->edit($dummyrec));

        $g_layout->output('</tr><tr>');
        $g_layout->td('<hr>','colspan="2"');
        $g_layout->output('</tr>');

        // Get default selection items.
        $selitems = $hoursnode->getSelectionItems();

        if (is_array($selitems))
        {
          foreach ($selitems as $label=>$html)
          {
            $g_layout->output('<tr>');
            $g_layout->td(text($label, $hoursnode->m_type).":");
            $g_layout->td($html);
            $g_layout->output('</tr><tr>');
            $g_layout->td('<hr>','colspan="2"');
            $g_layout->output('</tr>');
          }
        }


        $g_layout->output('<tr>');

        $g_layout->td('<b>'.text("report_output_options").'</b>','colspan="2"');

        $g_layout->output('</tr><tr>');

        $g_layout->td(text("report_output_type").": ");
        $g_layout->td('<select name="outputType">
                         <option VALUE="0" selected>'.text("report_output_standard").'
                         <option value="1">'.text("report_output_printable").'
                         <option value="2">'.text("report_output_export").'
                         </select>');

        $g_layout->output('</tr><tr>');

        $g_layout->td(text("report_output_showcollumns"));

        $cell="";
        for($i=0,$_i=count($arr_col);$i<$_i;$i++)
        {
          $checked = (in_array($arr_col[$i],$col) ? "checked" : "");
          $cell.='<input type="checkbox" name="col[]" value="'.$arr_col[$i].'" '.$checked.'>'.text($arr_col[$i]).'&nbsp;';
        }
        $g_layout->td($cell);

        $g_layout->output('</tr><tr>');

        $g_layout->td(text("orderby").': ', 'valign="top"');

        //$listcol = array();
        //for($i=0,$_i=count($arr_col);$i<$_i;$i++) $listcol[] = "hours.".$arr_col[$i];

        $orderattrs = array();
        $orderstr = "";

        for ($i=0; $i<$sortfieldcount; $i++)
        {
          if ($i==1)
          {
            // Add empty option after the first.
            array_unshift($arr_col, "-");
            //  array_unshift($listcol, "");
          }

          $field = new atkListAttribute("orderby[$i]",$arr_col);
          $direction =new atkListAttribute("orderdirection[$i]",array('column_asc', 'column_desc'), array('asc', 'desc'));
          $total = new atkBoolAttribute("subtotal[$i]",0);
          $orderstr.=($i+1).". ".$field->edit($dummyrec)." ".$direction->edit($dummyrec)." ".$total->edit($dummyrec)." ".text('subtotal')."<br>";
        }

        $g_layout->td($orderstr);

        $g_layout->output('</tr><tr>');

        $g_layout->td(text("aggregate").": ");

        $boolattrib = new atkListAttribute("aggregate", array("do_not_aggregate", "aggregate_by_order"), array(0,1), AF_OBLIGATORY);
        $g_layout->td($boolattrib->edit($dummyrec));

        $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');

      }

      // Create Filter
      $filter="activitydate>='".$startdate."' AND activitydate<='".$enddate."'";
      if(!empty($activity_id)&&$activity_id!="") $filter.=" AND hours.activityid=".$activity_id;
      if(!empty($phaseid)&&$phaseid!="") $filter.=" AND hours.phaseid=".$phaseid;
      if(!empty($projectid)&&$projectid!="") $filter.=" AND A.projectid=".$projectid;
      if(!empty($remark)&&$remark!="") $filter.=" AND hours.remark LIKE '%$remark%'";
      //if(!empty($customerid)&&$customerid!="") $filter.= " AND B.customer=".$customerid;
      if($department_name && $department_name != "all") $filter.=" AND userid.department='".escapeSQL($department_name)."'";
      if($functionlevelswitch && $functionlevelswitch != "all") $filter.=" AND userid.functionlevel='$functionlevelswitch'";
      
      // employee table is not yet added to the query. Force that by adding a manytoonerel,
      // so we display a users name.
      $hoursnode->add(new atkManyToOneRelation("userid", "employee.employee"));

      // organization and person_organization tables are not yet added to the query.
      // Since this is a many:many thing between projects and organizations, with the
      // contactpersons als intermediairy, we have to add a special attribute which
      // nests itself nicely in the query.
      if ($organizationid!="")
      {
        useattrib("project.filterprojectbyorganizationattrib");
        $hoursnode->add(new filterProjectByOrganizationAttrib($organizationid, $roleid, "B")); // the projectphaseattribute uses 'B' as alias for the project table.
      }

      if ($nameswitch=="supervisor")
      {
        if ($userid!="all")
        {
          $filter.= " AND userid.supervisor = '$userid' ";
        }
      }
      else
      {
        if ($userid!="all") $filter.= " AND hours.userid ='$userid' ";
      }
      
      $orderbystatement = array();
      if ($orderby)
      {
        foreach($orderby as $key => $itsOrderby)
        {
          if ($itsOrderby && $itsOrderby != "-")
            $orderbystatement[] = $itsOrderby . " " . $orderdirection[$key];
        }
      }
      $data = $hoursnode->selectDb($filter,implode(",",$orderbystatement), "");

      if (moduleExists("billing"))
      {
        $hoursnode->add(new fakeAttrib("rate"));
        $hoursnode->add(new fakeAttrib("costs"));

        $this->_addRateAndCosts($data);
      }
        
      if ($outputType=="0")
      {
        $g_layout->ui_bottom();
        $g_layout->ui_top(text("title_hoursurvey_hours")." ".text("report_intimespan")." ".$startdate." t/m ".$enddate);

        $g_layout->output('<br>'.(atkLevel()>0?atkButton('<< '.text("back"), "", SESSION_BACK)."<br>":"").'<br>');

        $actions = array();
        if (!$aggregate)
        {
          $actions = $hoursnode->defaultActions("list");
        }

        $g_layout->output($hoursnode->normalRecordList($data, $actions, RL_EXT_SORT, $suppress));
        $g_layout->output('<br>'.(atkLevel()>0?atkButton('<< '.text("back"), "", SESSION_BACK)."<br>":"").'<br>');

        $g_layout->ui_bottom();
      }

      if ($outputType=="1")
      {
        $g_layout->output($hoursnode->specialRecordList($data,"<tr>","<td>","</td>","</tr>", "0", "", $suppress));
      }
      if ($outputType=="2")
      {
        // specail RecordList can export to file
        $hoursnode->specialRecordList($data, "", '"', '";', "\r\n", "1", "", $suppress);
      }
    }
    
    /**
     * Gets the rates for the current timeregistration item
     * @param array    $timereg         The data of the timeregistration
     * @return array                    The rates associated with that timeregistration
     */
    function _getRatesPerTimereg($timereg)
    {
      global $g_db, $g_sessionManager;
      static $phase_project= array();
      
      $organizationarr = decodeKeyValuePair($this->m_postvars["organizationid"]);
      $organizationid =  $organizationarr["organization.id"];
      $ratetype =        $g_sessionManager->pageVar("ratetype");

      if (!$phase_project[$timereg["phaseid"]["phaseid"]])
      {
        $projectid = $g_db->getrows("SELECT projectid
                        FROM phase 
                        WHERE phase.id = '".$timereg["phaseid"]["phaseid"]."'");
        $phase_project[$timereg["phaseid"]["phaseid"]] = $projectid[0]["projectid"];
      }

      if (!atkconfig("use_duration", true))
      {
        $duration = $timereg["time"];
        convertDurationToHours($timereg);
        $timereg["time"] = $duration;
      }

      $rates = $this->_getRate($timereg['userid']['id'], $timereg['activitydate']['year'].$timereg['activitydate']['month'],
      $timereg['activityid'],   $phase_project[$timereg['phaseid']['phaseid']],
      $organizationid,          $ratetype, $timereg["phaseid"]["phaseid"]);

      return $rates;
    }
    
    function _addRateAndCosts(&$data)
    {
      global $g_sessionManager;
      
      $timeregs = array();
      $counter = 0;
      
      foreach ($data as $key => $timereg)
      {
        $oldtime = $timereg["time"];
        convertDurationToHours($timereg);
        $timereg["time"] = $oldtime;
        
        $rateused = false;
        $timereg["rate"] = $this->_getRatesPerTimereg($timereg);
       
    		foreach ($timereg["rate"] as $rate)
    		{
    		  if (!$rateused) $rateused = $this->_addRateAndCostsPerRate($timeregs, $timereg, $rate, $counter);
    		}
    		
    		if (!$rateused && $timereg["starttime"]!==$timereg["endtime"])
    		{
    		  if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		    $counter++;
    		  $timereg["rate"]=0;
    		  $timereg["costs"]=0; 
    		  $timeregs[$counter]=$timereg;
    		}
      }
      $data = $timeregs;
    }

    function _addRateAndCostsPerRate(&$timeregs,$timereg, $rate, &$counter)
    {
//      atkdebug("<h1>situation 1 if: ".$timereg["starttime"]["hours"].":".$timereg["starttime"]["minutes"].":".$timereg["starttime"]["seconds"].">=".$rate["starttime"]." && ".$timereg["endtime"]["hours"].":".$timereg["endtime"]["minutes"].":".$timereg["endtime"]["seconds"]."<=".$rate["endtime"]."</h1>");
//    	atkdebug("<h1>situation 2 if: ".$timereg["starttime"]["hours"].":".$timereg["starttime"]["minutes"].":".$timereg["starttime"]["seconds"]."<=".$rate["starttime"]." && ".$timereg["endtime"]["hours"].":".$timereg["endtime"]["minutes"].":".$timereg["endtime"]["seconds"].">=".$rate["endtime"]."</h1>");
//    	atkdebug("<h1>situation 3 if: (".$timereg["endtime"]["hours"].":".$timereg["endtime"]["minutes"].":".$timereg["endtime"]["seconds"]."<".$rate["endtime"]." && ".$timereg["starttime"]["hours"].":".$timereg["starttime"]["minutes"].":".$timereg["starttime"]["seconds"].">".$rate["endtime"].")
//    	&& ".$timereg["starttime"]["hours"].":".$timereg["starttime"]["minutes"].":".$timereg["starttime"]["seconds"].">".$rate["starttime"]."</h1>");
//    	atkdebug("<h1>situation 4 if: (".$timereg["starttime"]["hours"].":".$timereg["starttime"]["minutes"].":".$timereg["starttime"]["seconds"].">".$rate["starttime"]." && ".$timereg["endtime"]["hours"].":".$timereg["endtime"]["minutes"].":".$timereg["endtime"]["seconds"]."<".$rate["endtime"].")
//    	&& ".$timereg["starttime"]["hours"].":".$timereg["starttime"]["minutes"].":".$timereg["starttime"]["seconds"].">".$rate["starttime"]."</h1>");      
//    	atk_var_dump($timereg, "<h1>timereg</h1>");
    	$this->_makeTimeStamps($rate,$timereg["starttime"],$timereg["endtime"], $timereg["activitydate"]);
    	$this->_makeTimeStamps($timereg["rate"],$timereg["starttime"],$timereg["endtime"], $timereg["activitydate"]);
    	
//    	atk_var_dump($timereg, "<h1>timereg</h1>");
//    	atkdebug("<h1>timereg[starttime]: ".$timereg["starttime"]."</h1>");
//    	atkdebug("<h1>timereg[endtime]: ".$timereg["endtime"]."</h1>");
//    	atkdebug("<h1>rate[starttime]: ".$rate["starttime"]."</h1>");
//    	atkdebug("<h1>rate[endtime]: ".$rate["endtime"]."</h1>");
//    	atkdebug("<h1>situation 1 if: ".$timereg["starttime"].">=".$rate["starttime"]." && ".$timereg["endtime"]."<=".$rate["endtime"]."</h1>");
//    	atkdebug("<h1>situation 2 if: ".$timereg["starttime"]."<=".$rate["starttime"]." && ".$timereg["endtime"].">=".$rate["endtime"]."</h1>");
//    	atkdebug("<h1>situation 3 if: (".$timereg["starttime"]."<".$rate["endtime"]." && ".$timereg["starttime"].">".$rate["starttime"].")".
//    	"&& ".$timereg["starttime"].">".$rate["starttime"]."</h1>");
//    	atkdebug("<h1>situation 4 if: (".$timereg["starttime"].">".$rate["starttime"]." && ".$timereg["endtime"].">".$rate["starttime"].")
//    	&& ".$timereg["starttime"].">".$rate["starttime"]."</h1>");
    	
    	if ($timeregs[$counter]["starttime"] && $timeregs[$counter]["id"]) $counter++;
//    	atkdebug("<h1>counter: $counter</h1>");
//    	atk_var_dump($timereg, "<h1>called with timereg for counter $counter</h1>");
//      atk_var_dump($rate, "<h1>called with rate for counter $counter</h1>");
    	
    	// We have 5 situations
    	if ($timereg['starttime']==$timereg['endtime'])
    	{
    	  // Plain ridiculous, we don't add this
    	}
    	// 1. The timeregistration completely falls in this rate
    	else if ($timereg["starttime"]>=$rate["starttime"] && $timereg["endtime"]<=$rate["endtime"])
    	{
//    		atkdebug("<H1>situation 1</H1>");
    		$timeregs[$counter] = $timereg;
    		$this->_addTimeRateAndCosts($timeregs[$counter], $rate["rate"]);
    		return true;
    	}

    	// 2. The timeregistration encompasses this rate
    	elseif ($timereg["starttime"]<=$rate["starttime"] && $timereg["endtime"]>=$rate["endtime"])
    	{
//    		atkdebug("<H1>situation 2</H1>");
    		
    		$beforerate = $timereg;
    		$beforerate["endtime"]= $rate["starttime"];
    		$beforerate["time"] = ($beforerate["endtime"]-$beforerate["starttime"])/60;
    		
    		foreach ($timereg["rate"] as $subrate)
    		{
    		  if (!$rateused) $rateused = $this->_addRateAndCostsPerRate(&$timeregs, $beforerate, $subrate, $counter);
    		}
    		if (!$rateused && $beforerate["starttime"]!==$beforerate["endtime"])
    		{
    		  if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		    $counter++;
    		  $this->_addTimeRateAndCosts($beforerate, 0);
    		  $timeregs[$counter]=$beforerate;
    		}
    		$rateused=false;
        
    		if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		  $counter++;
    		
    		$timeregs[$counter] = $timereg;
    		$timeregs[$counter]["starttime"] = $rate["starttime"];
    		$timeregs[$counter]["endtime"] = $rate["endtime"];
    		
    		$this->_addTimeRateAndCosts($timeregs[$counter], $rate["rate"]);
    		
    		$afterrate = $timereg;
    		$afterrate["starttime"]= $rate["endtime"];
    		$afterrate["time"] = ($afterrate["endtime"]-$afterrate["starttime"])/60;
    		
    		foreach ($timereg["rate"] as $subrate)
    		{
    		  if (!$rateused) $rateused = $this->_addRateAndCostsPerRate(&$timeregs, $afterrate, $subrate, $counter);
    		}
    		if (!$rateused && $afterrate["starttime"]!==$afterrate["endtime"])
    		{
    		  if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		    $counter++;
    		  $this->_addTimeRateAndCosts($afterrate, 0);
    		  $timeregs[$counter]=$afterrate;
    		}
    		return true;
    	}

    	// 3. The end of the rate falls in the timeregistration but not the start
    	elseif (($timereg["starttime"]<$rate["endtime"] && $timereg["starttime"]>$rate["starttime"])
    	&& $timereg["starttime"]>$rate["starttime"])
    	{
//    		atkdebug("<H1>situation 3</H1>");
    		
    		$timeregs[$counter] = $timereg;
    		$timeregs[$counter]["endtime"] = $rate["endtime"];
    		
    		$this->_addTimeRateAndCosts($timeregs[$counter], $rate["rate"]);
    		
    		$afterrate = $timereg;
    		$afterrate["starttime"]= $rate["endtime"];
    		$afterrate["time"] = ($afterrate["endtime"]-$afterrate["starttime"])/60;
    		
    		foreach ($timereg["rate"] as $subrate)
    		{
    		  if (!$rateused) $rateused = $this->_addRateAndCostsPerRate(&$timeregs, $afterrate, $subrate, $counter);
    		}
    		if (!$rateused && $afterrate["starttime"]!==$afterrate["endtime"])
    		{
    		  if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		    $counter++;
    		  $this->_addTimeRateAndCosts($afterrate, 0);
    		  $timeregs[$counter]=$afterrate;
    		}
    		return true;
    	}

    	// 4. The start of the rate falls in the timeregistration but not the end
    	elseif (($timereg["starttime"]>$rate["starttime"] && $timereg["starttime"]<$rate["endtime"])
    	&& $timereg["endtime"]>$rate["endtime"])
    	{
//    	  atkdebug("<H1>situation 4</H1>");
    	  $beforerate = $timereg;
    		$beforerate["endtime"]= $rate["starttime"];
    		$beforerate["time"] = ($beforerate["endtime"]-$beforerate["starttime"])/60;
    		
    		foreach ($timereg["rate"] as $subrate)
    		{
    		  if (!$rateused) $rateused = $this->_addRateAndCostsPerRate(&$timeregs, $beforerate, $subrate, $counter);
    		}
    		if (!$rateused && $beforerate["starttime"]!==$beforerate["endtime"])
    		{
    		  if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		    $counter++;
    		  $this->_addTimeRateAndCosts($beforerate, 0);
    		  $timeregs[$counter]=$beforerate;
    		}
    	  $rateused=false;
    		if ($timeregs[$counter]['starttime'] && $timeregs[$counter]['id'])
    		  $counter++;
    		
    		$timeregs[$counter] = $timereg;
    		$timeregs[$counter]["starttime"] = $rate["starttime"];
    		foreach ($timereg["rate"] as $rate)
    		{
    		  $this->_addTimeRateAndCosts($timeregs[$counter], $rate["rate"]);
    		}
    		return true;
    	}

    	// And that should be it
    	else
    	{
//    		atk_var_dump($timereg, "timereg for ignored rate");
//    		atk_var_dump($rate, "rate for ignored rate");
    	}
    }
    
    function _addTimeRateAndCosts(&$timereg, $rate)
    {
    	$timereg["time"] = ($timereg["endtime"]-$timereg["starttime"])/60;
    	$timereg["rate"] = $rate;
    	$timereg["costs"]= $rate*($timereg["time"]/60);
    	
    	$this->_makeTime($timereg["starttime"], $timereg["endtime"], $timereg["activitydate"]);
    }

   /**
    * Get user level
    * @param string $userid User-id
    * @return int Level
    */
    function _getLevel($userid)
    {
      $users = $this->_getUsers();      
      return $users[$userid]["level"];
    }
    
    
    /**
    * Get rates
    * @param string $userid           User-id
    * @param string $month            Month
    * @param int    $activityid       Activity-id
    * @param int    $projectid        Project-id
    * @param int    $organization_id  Organization-id
    * @param int    $ratetype         The type of the rate
    * @return array User Rates
    */
    function _getRate($userid, $month, $activityid, $projectid, $organization_id, $ratetype=0, $phaseid=-1)
    {     
      static $rates = array();
      $currentrates = &$rates[$userid][$month][$activityid][$projectid][$organization_id][$ratetype][$phaseid][0];
      
      if (!$currentrates)
      {      	
        $raterecs = $this->_getRatesFromDb($userid, $month, $activityid, $projectid, $organization_id, $ratetype, $phaseid);
        $currentrates = $raterecs;
      }
      
      return $currentrates;
    }
    
    function _getRatesFromDb($userid, $month, $activityid, $projectid, $organization_id, $ratetype=0, $phaseid=-1)
    {
      global $g_db;
      $date = $month."01";
        
      if (atkconfig("use_duration", true)) $fields = "rate";
      else $fields = "rate, starttime, endtime";
              
      $raterecs = $g_db->getrows("SELECT $fields FROM 
                                      billing_rate
                                    WHERE
                                           (startdate IS NULL OR '$date' > date_format(startdate, '%Y%m%d') OR '$date' = date_format(startdate, '%Y%m%d'))
                                       AND (enddate IS NULL OR '$date' < date_format(enddate, '%Y%m%d') OR '$date' = date_format(enddate, '%Y%m%d'))
                                       AND (user_id IS NULL OR user_id=0 OR user_id = '$userid') ".($activityid!=''?
                                       "AND (activity_id IS NULL OR activity_id = 0 OR activity_id = '$activityid')":"")."
                                       AND (project_id IS NULL OR project_id=0 OR project_id='$projectid')
                                       AND (customer_id IS NULL OR customer_id=0 OR customer_id='$organization_id')
                                       AND (phaseid IS NULL".(($phaseid!==-1)?" OR phaseid='$phaseid'":'').")
                                       AND ratetype='$ratetype'
                                       AND functionlevel_id = 0
                                       
                                    ORDER BY priority DESC");
                                       
      if(empty($raterecs) )
      {
        global $g_db;    	     	
        $PersonFunctionLevel = $g_db->getrows("SELECT functionlevel  from person where person.id = '".
      										$userid . "'");
        $PersonFunctionLevel = $PersonFunctionLevel[0]["functionlevel"];
      
      	// no personal rate so get the rate for the functionlevel
      	$raterecs = $g_db->getrows("SELECT $fields FROM 
                                      billing_rate
                                    WHERE
        								  (startdate IS NULL OR '$date' > date_format(startdate, '%Y%m%d' OR '$date' = date_format(startdate, '%Y%m%d')))
                                       AND (enddate IS NULL OR '$date' < date_format(enddate, '%Y%m%d') OR '$date' = date_format(enddate, '%Y%m%d'))
                                       AND (user_id IS NULL OR user_id=0 OR user_id = '$userid') ".($activityid!=''?
                                       "AND (activity_id IS NULL OR activity_id = 0 OR activity_id = '$activityid')":"")."
                                       AND (project_id IS NULL OR project_id=0 OR project_id='$projectid')
                                       AND (customer_id IS NULL OR customer_id=0 OR customer_id='$organization_id')
                                       AND (phaseid IS NULL".(($phaseid!==-1)?" OR phaseid=0 OR phaseid='$phaseid'":'').")
                                       AND ratetype='$ratetype'
                                       AND functionlevel_id ='$PersonFunctionLevel'
                                       
                                    ORDER BY priority DESC");     	
      	
      }
                                  atk_var_dump($raterecs, "<h1>returning</h1>");     
      return $raterecs;
    }
    
    function _makeTimeStamps(&$rates, &$starttime, &$endtime, $activitydate=array("year"=>2005, "month"=>01, "day"=>01))
    {
      if ($starttime["hours"]!==null)
      {
        $starttime = mktime($starttime["hours"], $starttime["minutes"],$starttime["seconds"],"1","1","1970");
        $endtime =   mktime($endtime["hours"], $endtime["minutes"],$endtime["seconds"],"1","1","1970");
      }
      if (!$rates['starttime'])
      {
        foreach ($rates as $ratekey=>$rate)
        {
          $explodedtime = explode(":", $rate['starttime']);
          if ($explodedtime[1])
          {
            $rates[$ratekey]['starttime'] = mktime($explodedtime[0], $explodedtime[1],$explodedtime[2],"1","1","1970");

            $explodedtime = explode(":", $rate['endtime']);
            $rates[$ratekey]['endtime'] =   mktime($explodedtime[0], $explodedtime[1],$explodedtime[2],"1","1","1970");
          }
        }
      }
      else
      {
        $explodedtime = explode(":", $rates['starttime']);
        if ($explodedtime[1])
        {
          $rates['starttime'] = mktime($explodedtime[0], $explodedtime[1],$explodedtime[2],"1","1","1970");

          $explodedtime = explode(":", $rates['endtime']);
          $rates['endtime'] =   mktime($explodedtime[0], $explodedtime[1],$explodedtime[2],"1","1","1970");
        }
      }
    }
    
    function _makeTime(&$starttime, &$endtime)
    {
      $oldstarttime = $starttime;
      $oldendtime = $endtime;
      
      $starttime = array();
    	$starttime['hours'] 	= strftime('%H',$oldstarttime);
    	$starttime['minutes'] = strftime('%M',$oldstarttime);
    	$starttime['seconds'] = strftime('%S',$oldstarttime);
    	
      $endtime = array();
    	$endtime['hours'] 	= strftime('%H',$oldendtime);
    	$endtime['minutes'] = strftime('%M',$oldendtime);
    	$endtime['seconds'] = strftime('%S',$oldendtime);
    }
    
    function get_activities($act_id)
    {
      global $g_db;
      // Get the activities
      $sql = "SELECT id,name
              FROM activity
              ORDER BY name
             ";
      $records = $g_db->getrows($sql);
      if($act_id==-1) { $sel="SELECTED"; } else { $sel=""; }
      $activity_code='<OPTION VALUE="" '.$sel.'>'.text('allactivities').'</OPTION>';
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if($act_id==$records[$i]["id"]) { $sel="SELECTED"; } else { $sel=""; }
        $activity_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].'</OPTION>';
      }
      return $activity_code;
    }
    
    function get_departments($depname)
    {
      global $g_db;
      
      $sql = "SELECT name
              FROM employee_department
              ORDER BY name";
      
      $records = $g_db->getrows($sql);
      $depcode = "<OPTION VALUE='all'>".text('alldepartments')."</OPTION>";
      foreach ($records as $onedep)
      {
        $sel = $onedep['name'] == $depname ? " SELECTED" : "";
        $depcode .= "<OPTION VALUE='".$onedep['name']."'$sel>".$onedep['name']."</OPTION>";
      } 
      return $depcode;
    }

    function get_employees($user_id)
    {
      global $g_db;

      $sql = "SELECT id,lastname,userid, firstname
              FROM person
              WHERE status='active' AND role='employee'
              ORDER BY lastname
             ";

      $records = $g_db->getrows($sql);
      if ($user_id == "new" || !$user_id || $user_id=="") $sel = " SELECTED";
      $employee_code='<OPTION VALUE="all"'.$sel.'>'.text("allusers");
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if($user_id==$records[$i]["id"]) { $sel="SELECTED"; } else { $sel=""; }
        $employee_code.='<OPTION VALUE="'.addslashes($records[$i]["id"]).'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      return $employee_code;
    }

    function get_roles($role)
    {
      global $g_db;

      $sql = "SELECT name, id
              FROM role
              ORDER BY name";

      $records = $g_db->getrows($sql);
      $role_code='<OPTION VALUE="">'.text("allroles");
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if ($role == $records[$i]["id"]) $sel = "SELECTED"; else $sel = "";
        $role_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].':</OPTION>';
      }
      return $role_code;
    }
    
    function get_functionlevels($selected)
    {
      global $g_db;
      $sql = "  SELECT DISTINCT functionlevel.id as id, functionlevel.name as name
                FROM functionlevel
                LEFT JOIN person ON (person.functionlevel = functionlevel.id)";
      $records = $g_db->getrows($sql);
      
      $sel = !$selected || $selected == "all" ? " selected" : "";
      $func_code = '<OPTION VALUE="all"'.$sel.'>'.text("allfunctionlevels").'</OPTION>';
      foreach($records as $record)
      {
        $sel = $selected == $record['id'] ? " selected" : "";
        $func_code .= '<OPTION VALUE="'.$record['id'].'"'.$sel.'>'.$record['name'].'</OPTION>';
      }
      return $func_code;
    }
    
    /**
     * adds a row with 'or' in it
     * @param g_layout object the layout object we have to hook ourselves in to
     * @return void
     */
    function _addOr(&$g_layout)
    {
      $g_layout->output('<tr>');
      $g_layout->td(text('or'),'colspan="2"');
      $g_layout->output('</tr>');
    }
    
    /**
     * adds all required hidden fields to the form
     * @param g_layout object the layout object we have to hook ourselves in to
     * @return void
     */
    function _addHiddenFields(&$g_layout)
    {
      $g_layout->output('<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">');
      $g_layout->output('<input type="hidden" name="atkaction" value="report">'); 
    }
  }

?>
