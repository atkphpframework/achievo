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

  class hoursurvey extends atkNode
  {
    function hoursurvey()
    {
      $this->atkNode("hoursurvey");
    }

    function action_report(&$handler)
    {
      global $g_user, $g_sessionManager;

      $this->addStyle("style.css");

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

      $externalHourSurveyColumns = atkHarvestModules("getHourSurveyColumns", "", true);
     
      //The orderby postvar is only filled when clicking on the refresh button.
      //Atk uses the postvar atkorderby when a user clicks on a column to sort on it. Powers combined
      if(!isset($orderby) && isset($this->m_postvars["atkorderby"])) $orderby[] = $this->m_postvars["atkorderby"];
      
      foreach($externalHourSurveyColumns as $module => $attr)
      {
        eval("\$external$attr = '".$g_sessionManager->pageVar($attr)."';");
      }

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

      atkimport("atk.layout");
      $layout = &layout::getInstance();

      if ($outputType==0)
      {
        $layout->initGUI();
        $layout->ui_top(atktext("parameters", $this->m_module, $this->m_type));

        $layout->output('<form action="dispatch.php" method="get" name="entryform">');
        $layout->output(session_form());

        $this->_addHiddenFields($layout);
        $layout->table_simple();
        $layout->output('<tr>');
        $layout->td('<b>'.atktext('sethoursfilter').'</b>', 'colspan="2"');
        $layout->output('</tr><tr>');
        $layout->td('<hr>'.atktext('searchbyprojectorphase').':','colspan="2"');
        $layout->output('</tr><tr>');
        $layout->td(atktext('project').':');

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

        foreach ($externalHourSurveyColumns as $module => $attr)
        {
          $var = "external$attr";
          $dummyrec[$attr] = $$var;
        }

        $projectatt = new atkProjectAttribute('projectid');

        $sortfieldcount = max(3, $columnConfig->countSortAttribs());

        for ($i=0; $i<$sortfieldcount; $i++)
        {
          $dummyrec["orderby[$i]"]=$columnConfig->getAttributeByOrder($i+1);
          $dummyrec["orderdirection[$i]"]=$columnConfig->getDirectionByOrder($i+1);
          $dummyrec["subtotal[$i]"]=($columnConfig->hasSubTotalByOrder($i+1)?1:0);
        }

        $layout->td($projectatt->edit($dummyrec));
        $layout->output('</tr>');
        $this->_addOr($layout);
        $layout->output('<tr>');
        $layout->td(atktext('phase').':');
        $phaseatt = $hoursnode->m_attribList["phaseid"];
        $phaseatt->m_flags = $phaseatt->m_flags&~AF_JS_UPDATEACTIVITY;
        $phaseatt->m_nested = false;
        $g_sessionManager->stackVar("reportnodetype","hoursurvey");
        $layout->td($phaseatt->edit($dummyrec));

        $layout->output('</tr>');
        $this->_addOr($layout);
        $layout->output('<tr>');

        $layout->td(atktext("organization").':');
        $organizationatt = new atkManyToOneRelation("organizationid", "organization.organization");
        $layout->td('<SELECT name="roleid">'.$this->get_roles($roleid).'</SELECT> '.

        $organizationatt->edit($dummyrec));
        $layout->output('</tr>');
        $this->_addOr($layout);
        $layout->output('<tr>');

        $layout->td(atktext("department").':');
        $departmentatt = new atkManyToOneRelation("departmentid","employee.department");
        $layout->td('<SELECT name="departmentid">'.$this->get_departments($department_name).'</SELECT>');
        $layout->output('</tr><tr>');

        $layout->td('<hr>','colspan="2"');
        $layout->output('</tr><tr>');
        $layout->td(atktext('employee').':');

        if($view_all)
        {
          $layout->td('<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>'.
          ' <SELECT name="userid">'.$this->get_employees($userid).'</SELECT>');
        }
        else
        {
          $userid = $g_user["id"];
          $layout->td($g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">');
        }
        $layout->output('</tr>');
        $this->_addOr($layout);
        $layout->output('<tr>');

        // selecting functionlevel:
        $layout->td(atktext('functionlevel').':');
        $layout->td('<SELECT name="functionlevelswitch">' . $this->get_functionlevels($functionlevelswitch).'</SELECT>');

        $layout->output('</tr><tr>');


        $layout->td('<hr>','colspan="2"');
        $layout->output('</tr><tr>');
        $layout->td(atktext('activity').': ');
        $layout->td('<SELECT name="activity_id">'.$this->get_activities($activity_id).'</SELECT>');
        $layout->output('</tr><tr>');
        $layout->td('<hr>','colspan="2"');
        $layout->output('</tr><tr>');

        $layout->td(atktext("timespan").': ');

        $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY);
        $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY);

        $layout->td('<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td>'.$startdateatt->edit($dummyrec).'</td><td class="table"> &nbsp;'.
        atktext("until").
        '&nbsp; </td><td align="right">'.$enddateatt->edit($dummyrec).'</td></tr></table>');

        $layout->output('</tr><tr>');
        $layout->register_script(atkconfig("atkroot")."javascript/timereg.js");
        $layout->td(atktext("lookbackperiod").': ');
        $output = '<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td width="33%">';
        $output .= '<select name="startperiod" onchange="setStartEndDates(\'enddate\', \'startperiod\', '.date("Ymd").'); document.entryform.endperiod.value = 0; setStartEndDates(\'startdate\', \'endperiod\', '.date("Ymd").');">
                        <option value="0" selected>---</option>
                        <option value="1">1 '.strtolower(atktext("week")).'</option>
                        <option value="2">2 '.strtolower(atktext("weeks")).'</option>
                        <option value="3">3 '.strtolower(atktext("weeks")).'</option>
                        <option value="4">4 '.strtolower(atktext("weeks")).'</option>
                        <option value="5">1 '.strtolower(atktext("month")).'</option>
                        <option value="6">2 '.strtolower(atktext("months")).'</option>
                        <option value="7">3 '.strtolower(atktext("months")).'</option>
                        <option value="8">6 '.strtolower(atktext("months")).'</option>
                       </select></td>';

        $output .= '<td width="34%" align="center">&nbsp;</td>';
        $output .= '<td width="33%" align="right"><select name="endperiod" onchange="setStartEndDates(\'startdate\', \'endperiod\', '.date("Ymd").'); document.entryform.startperiod.value = 0; setStartEndDates(\'enddate\', \'startperiod\', '.date("Ymd").');">
                        <option value="0" selected>---</option>
                        <option value="1">1 '.strtolower(atktext("week")).'</option>
                        <option value="2">2 '.strtolower(atktext("weeks")).'</option>
                        <option value="3">3 '.strtolower(atktext("weeks")).'</option>
                        <option value="4">4 '.strtolower(atktext("weeks")).'</option>
                        <option value="5">1 '.strtolower(atktext("month")).'</option>
                        <option value="6">2 '.strtolower(atktext("months")).'</option>
                        <option value="7">3 '.strtolower(atktext("months")).'</option>
                        <option value="8">6 '.strtolower(atktext("months")).'</option>
                       </select></td></tr></table>';

        $layout->td($output);
        $layout->output('</tr><tr>');
        $layout->td('<hr>','colspan="2"');
        $layout->output('</tr><tr>');

        $remarkattr = new atkAttribute("remark", 0, 80);
        $layout->td(atktext('remark').': ');
        $layout->td($remarkattr->edit($dummyrec));

        foreach ($externalHourSurveyColumns as $module => $attr)
        {
          $currentattr = &$hoursnode->getAttribute($attr);
          $layout->output('</tr><tr>');
          $layout->td('<hr>','colspan="2"');
          $layout->output('</tr><tr>');
          $layout->td(atktext($attr, $module).': ');

          if (get_class($currentattr) == "atkboolattribute")
          {
            $output = '<select name="'.$attr.'"><option value="" selected>---</option><option value="0">'.atktext("no","atk").'</option><option value="1">'.atktext("yes","atk").'</option></select>';
          }
          else
          {
            $output = $currentattr->edit($dummyrec);
          }
          $layout->td($output);
        }

        $layout->output('</tr><tr>');
        $layout->td('<hr>','colspan="2"');
        $layout->output('</tr>');

        // Get default selection items.
        $selitems = $hoursnode->getSelectionItems();

        if (is_array($selitems))
        {
          foreach ($selitems as $label=>$html)
          {
            $layout->output('<tr>');
            $layout->td(atktext($label, "", $hoursnode->m_type).":");
            $layout->td($html);
            $layout->output('</tr><tr>');
            $layout->td('<hr>','colspan="2"');
            $layout->output('</tr>');
          }
        }


        $layout->output('<tr>');

        $layout->td('<b>'.atktext("report_output_options").'</b>','colspan="2"');

        $layout->output('</tr><tr>');

        $layout->td(atktext("report_output_type").": ");
        $layout->td('<select name="outputType">
                         <option VALUE="0" selected>'.atktext("report_output_standard").'
                         <option value="1">'.atktext("report_output_printable").'
                         <option value="2">'.atktext("report_output_export").'
                         </select>');

        $layout->output('</tr><tr>');

        $layout->td(atktext("report_output_showcollumns"));

        $cell="";
        for($i=0,$_i=count($arr_col);$i<$_i;$i++)
        {
          $checked = (in_array($arr_col[$i],$col) ? "checked" : "");
          $translatewithmodule = !in_array($arr_col[$i], array_values($externalHourSurveyColumns)) ? "" : array_search($arr_col[$i], $externalHourSurveyColumns);
          $cell.='<input type="checkbox" name="col[]" value="'.$arr_col[$i].'" '.$checked.'>'.atktext($arr_col[$i], $translatewithmodule).'&nbsp;';
        }
        $layout->td($cell);

        $layout->output('</tr><tr>');

        $layout->td(atktext("orderby").': ', 'valign="top"');

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
          $orderstr.=($i+1).". ".$field->edit($dummyrec)." ".$direction->edit($dummyrec)." ".$total->edit($dummyrec)." ".atktext('subtotal')."<br>";
        }

        $layout->td($orderstr);

        $layout->output('</tr><tr>');

        $layout->td(atktext("aggregate").": ");

        $boolattrib = new atkListAttribute("aggregate", array("do_not_aggregate", "aggregate_by_order"), array(0,1), AF_OBLIGATORY);
        $layout->td($boolattrib->edit($dummyrec));

        $layout->output('</tr></table><input type="submit" value="'.atktext("refresh").'"></form><br>');

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

      foreach ($externalHourSurveyColumns as $module => $attr)
      {
        $var = "external$attr";
        $value = $$var;
        if (!empty($value)) $filter.=" AND hours.$attr = $value";
      }

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
          {
						//hack we need to add the tablename before the attribute. all attributes except for functionlevel has the table name hours
						$itsOrderby = ($itsOrderby == "functionlevel") ? "person.".$itsOrderby : "hours.".$itsOrderby;

            $orderbystatement[] = $itsOrderby . " " . $orderdirection[$key];
          }
        }
      }

      //The id field needs to be added too, or else the dispatch url doesn't know which record to edit.
      if(!in_array("id",$col))
        $col[] = "id";

      $data = $hoursnode->selectDb($filter,implode(",",$orderbystatement), "", "", $col);

      if (moduleExists("billing"))
      {
         // todo, add columns with rate and costs (are stored flat in the table, no reason to perform calculations)
      }

      if ($outputType=="0")
      {
        $layout->ui_bottom();
        $layout->ui_top(atktext("title_hoursurvey_hours")." ".atktext("report_intimespan")." ".$startdate." t/m ".$enddate);

        $layout->output('<br>'.(atkLevel()>0?atkButton('<< '.atktext("back"), "", SESSION_BACK)."<br>":"").'<br>');

        $actions = array();
        if (!$aggregate)
        {
          $actions = $hoursnode->defaultActions("list");
        }

        $rl = &atknew("atk.recordlist.atkrecordlist");
        $layout->output($rl->render($hoursnode, $data, $actions, RL_EXT_SORT, $suppress));
        $layout->output('<br>'.(atkLevel()>0?atkButton('<< '.atktext("back"), "", SESSION_BACK)."<br>":"").'<br>');

        $layout->ui_bottom();
      }

      if ($outputType=="1")
      {
        $rl = &atknew("atk.recordlist.atkcustomrecordlist");
        $layout->output($rl->render($hoursnode, $data,"<tr>","<td>","</td>","</tr>", "0", "", $suppress));
      }
      if ($outputType=="2")
      {
        // specail RecordList can export to file
        //$hoursnode->specialRecordList($data, "", '"', '";', "\r\n", "1", "", $suppress);
        $rl = &atknew("atk.recordlist.atkcustomrecordlist");
        $rl->render($hoursnode, $data, "", '"', '";', "\r\n", "1", "",$suppress);
      }
    }

    function get_activities($act_id)
    {
      $db = &atkGetDb();
      // Get the activities
      $sql = "SELECT id,name
              FROM activity
              ORDER BY name
             ";
      $records = $db->getrows($sql);
      if($act_id==-1) { $sel="SELECTED"; } else { $sel=""; }
      $activity_code='<OPTION VALUE="" '.$sel.'>'.atktext('allactivities').'</OPTION>';
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if($act_id==$records[$i]["id"]) { $sel="SELECTED"; } else { $sel=""; }
        $activity_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].'</OPTION>';
      }
      return $activity_code;
    }

    function get_departments($depname)
    {
      $db = &atkGetDb();

      $sql = "SELECT name
              FROM employee_department
              ORDER BY name";

      $records = $db->getrows($sql);
      $depcode = "<OPTION VALUE='all'>".atktext('alldepartments')."</OPTION>";
      foreach ($records as $onedep)
      {
        $sel = $onedep['name'] == $depname ? " SELECTED" : "";
        $depcode .= "<OPTION VALUE='".$onedep['name']."'$sel>".$onedep['name']."</OPTION>";
      }
      return $depcode;
    }

    function get_employees($user_id)
    {
      $db = &atkGetDb();

      $sql = "SELECT person.id,person.lastname,person.userid, person.firstname
              FROM person
              WHERE person.status='active' AND person.role='employee'
              ORDER BY person.lastname
             ";

      $records = $db->getrows($sql);
      if ($user_id == "new" || !$user_id || $user_id=="") $sel = " SELECTED";
      $employee_code='<OPTION VALUE="all"'.$sel.'>'.atktext("allusers");
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if($user_id==$records[$i]["id"]) { $sel="SELECTED"; } else { $sel=""; }
        $employee_code.='<OPTION VALUE="'.addslashes($records[$i]["id"]).'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      return $employee_code;
    }

    function get_roles($role)
    {
      $db = &atkGetDb();

      $sql = "SELECT role.name, role.id
              FROM role
              ORDER BY role.name";

      $records = $db->getrows($sql);
      $role_code='<OPTION VALUE="">'.atktext("allroles");
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if ($role == $records[$i]["id"]) $sel = "SELECTED"; else $sel = "";
        $role_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].':</OPTION>';
      }
      return $role_code;
    }

    function get_functionlevels($selected)
    {
      $db = &atkGetDb();
      $sql = "  SELECT DISTINCT functionlevel.id as id, functionlevel.name as name
                FROM functionlevel
                LEFT JOIN person ON (person.functionlevel = functionlevel.id)";
      $records = $db->getrows($sql);

      $sel = !$selected || $selected == "all" ? " selected" : "";
      $func_code = '<OPTION VALUE="all"'.$sel.'>'.atktext("allfunctionlevels").'</OPTION>';
      foreach($records as $record)
      {
        $sel = $selected == $record['id'] ? " selected" : "";
        $func_code .= '<OPTION VALUE="'.$record['id'].'"'.$sel.'>'.$record['name'].'</OPTION>';
      }
      return $func_code;
    }

    /**
     * adds a row with 'or' in it
     * @param layout object the layout object we have to hook ourselves in to
     * @return void
     */
    function _addOr(&$layout)
    {
      $layout->output('<tr>');
      $layout->td(atktext('or'),'colspan="2"');
      $layout->output('</tr>');
    }

    /**
     * adds all required hidden fields to the form
     * @param layout object the layout object we have to hook ourselves in to
     * @return void
     */
    function _addHiddenFields(&$layout)
    {
      $layout->output('<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">');
      $layout->output('<input type="hidden" name="atkaction" value="report">');
    }
  }

?>