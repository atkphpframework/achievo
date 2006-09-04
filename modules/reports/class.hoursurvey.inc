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

  class functionLevelAttribute extends atkAttribute
  {
    function storageType()
    {
      return NOSTORE;
    }

    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      $query->addJoin("functionlevel", "functionlevelid", "userid.functionlevel = functionlevelid.id", true);
      $query->addField("name","","functionlevelid","functionlevel_",!$this->hasFlag(AF_NO_QUOTES),true);
    }
    function db2value($record)
    {
      return atkArrayNvl($record, "functionlevel_name");
    }

  }

  class billingTypeViewAttribute extends atkAttribute {

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function billingTypeViewAttribute($name, $flags=0)
    {
      $this->atkAttribute($name,$flags|AF_READONLY|AF_HIDE_SEARCH|AF_NO_SORT); // base class constructor
    }

    /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
    function storageType() { return NOSTORE; }

    /**
     * Make sure the value is loaded *after* the main record is loaded.
     * @access private
     * @return int
     */
    function loadType() { return POSTLOAD; }

    function label()
    {
      return atkText("billing_type", "billing");
    }

    /**
     * The load method performs the calculation.
     *
     * @access private
     * @param atkDb $db
     * @param array $record
     * @return String result of the calculation
     */
    function load(&$db, $record)
    {
      $phaseid = $record["phaseid"]["phaseid"];
      $db = &atkGetDb();
      $query = "SELECT C.description FROM phase A LEFT JOIN project B ON A.projectid=B.id LEFT JOIN billing_projecttype C ON B.billing_type = C.id WHERE A.id='$phaseid'";
      $recs = $db->getRows($query);
      return $recs[0]["description"];
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

      $ui = &$this->getUi();
      $page = &$this->getPage();
      $this->addStyle("style.css");
      $page->register_style(moduleDir("reports")."style/printablereport.css", "print");

      // Manual sort config
      $orderby =            atkArrayNvl($this->m_postvars, "orderby", "");
      $orderdirection =     atkArrayNvl($this->m_postvars, "orderdirection", "");
      $subtotal =           atkArrayNvl($this->m_postvars, "subtotal", "");

      $activity_id =        $g_sessionManager->pageVar("activity_id");
      $projectid =          $g_sessionManager->pageVar("projectid");
      $selectedprojectid =  $g_sessionManager->pageVar("selectedprojectid");
      if (moduleExists("billing"))
      {
        $billing_projecttypepair = $g_sessionManager->pageVar("billing_projecttypeid");
        $billing_projecttypeid = (!empty($billing_projecttypepair)) ? atkArrayNvl(decodeKeyValuePair($billing_projecttypepair), "billing_projecttype.id", "") : "";
      }
      $phaseid =            $g_sessionManager->pageVar("phaseid");
      $userid =             $g_sessionManager->pageVar("userid");
      $nameswitch =         $g_sessionManager->pageVar("nameswitch");
      $roleid =             $g_sessionManager->pageVar("roleid");
      $remark =             $g_sessionManager->pageVar("remark");
      $aggregate =          $g_sessionManager->pageVar("aggregate");
      $startdate =          $g_sessionManager->pageVar("startdate");
      $enddate =            $g_sessionManager->pageVar("enddate");
      $col =                $g_sessionManager->pageVar("col");
      // the selected customer_id is found as key=value pair ("customer.id='3'") in the postvars
      $organizationpair =   atkArrayNvl($this->m_postvars, "organizationid", "");
      $organizationid =     (!empty($organizationpair)) ? atkArrayNvl(decodeKeyValuePair($organizationpair), "organization.id", "") : "";
      $department_name =    $g_sessionManager->pageVar("departmentid");
      $functionlevelswitch =$g_sessionManager->pageVar("functionlevelswitch");
      $outputType =         atkArrayNvl($this->m_postvars, "outputType", "");

      $externalHourSurveyColumns = atkHarvestModules("getHourSurveyColumns", "", true);

      //The orderby postvar is only filled when clicking on the refresh button.
      //Atk uses the postvar atkorderby when a user clicks on a column to sort on it. Powers combined
      if(!isset($orderby) && isset($this->m_postvars["atkorderby"])) $orderby[] = $this->m_postvars["atkorderby"];

      foreach(array_keys($externalHourSurveyColumns) as $module)
      {
        $attrlist = $externalHourSurveyColumns[$module];
        if (is_string($attrlist))
          $attrlist = array($attrlist);
        foreach($attrlist as $attr)
        {
          eval("\$external$attr = '".$g_sessionManager->pageVar($attr)."';");
        }
      }

      if (empty($outputType))
      {
        $outputType=0;   //default=standard
      }

      $hoursnode = &atkGetNode("timereg.hours");
      $hoursnode->m_postvars = $this->m_postvars; // Share postvars.

      // Hack: make sure the activities, which are populated by the project/phase column, are properly loaded.
      $hoursnode->add(new sortableActivity());
      $hoursnode->add(new functionLevelAttribute("functionlevel"));
      $hoursnode->add(new billingTypeViewAttribute("billing_projecttypeid"));


      $columnConfig = &$hoursnode->getColumnConfig();


      if (is_array($orderby))
      {
        $columnConfig->clearOrder();
        for ($i=0, $_i=count($orderby); $i<$_i; $i++)
        {
          if ($orderby[$i]!="-" && $orderby[$i]!="")
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

      $groupfields = $columnConfig->getOrderFields();
      if ($aggregate==1 && count($groupfields)>0)
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
        $title = atktext("parameters", $this->m_module, $this->m_type);

        $content='<form action="dispatch.php" method="get" name="entryform">';
        $content.=session_form();

        $this->_addHiddenFields($content);

        $tbl = &atknew("atk.utils.atktablerenderer");
        $data[] = array("<b>".atkText("sethoursfilter","project")."</b>");
        $data[] = array('<hr>'.atktext('searchbyprojectorphase','project').':');

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
        if (moduleExists("billing"))
          $dummyrec["billing_projecttypeid"] = array("id"=>$billing_projecttypeid);

        foreach ($externalHourSurveyColumns as $module => $attrlist)
        {
          if (is_array($attrlist))
          {
            foreach($attrlist as $attr)
            {
              $var = "external$attr";
              $dummyrec[$attr] = $$var;
            }
          }
        }

        $projectatt = new atkProjectAttribute('projectid');

        $sortfieldcount = max(3, $columnConfig->countSortAttribs());

        for ($i=0; $i<$sortfieldcount; $i++)
        {
          $dummyrec["orderby[$i]"]=$columnConfig->getAttributeByOrder($i+1);
          $dummyrec["orderdirection[$i]"]=$columnConfig->getDirectionByOrder($i+1);
          $dummyrec["subtotal[$i]"]=($columnConfig->hasSubTotalByOrder($i+1)?1:0);
        }
        $data[] = array(atktext('project','project').':',$projectatt->edit($dummyrec));


        $this->_addOr($data);

        if (moduleExists("billing"))
        {
          $projecttypeatt = new atkManyToOneRelation('billing_projecttypeid', "billing.projecttype");
          $data[] = array(atktext('billing_type','billing').':',$projecttypeatt->edit($dummyrec));
          $this->_addOr($data);
        }

        $phaseatt = $hoursnode->m_attribList["phaseid"];
        $phaseatt->m_flags = $phaseatt->m_flags&~AF_JS_UPDATEACTIVITY;
        $phaseatt->m_nested = false;
        $g_sessionManager->stackVar("reportnodetype","hoursurvey");

        $data[] = array(atktext('phase','project').':',$phaseatt->edit($dummyrec));
        $this->_addOr($data);

        $organizationatt = new atkManyToOneRelation("organizationid", "organization.organization");

        $data[] = array(atktext("organization","project").':','<SELECT name="roleid">'.$this->get_roles($roleid).'</SELECT> '.$organizationatt->edit($dummyrec));
        $this->_addOr($data);

        $data[] = array(atkText("department","project").':','<SELECT name="departmentid">'.$this->get_departments($department_name).'</SELECT>');

        $data[] = array("<hr>");


        if($view_all)
        {
          $tmp = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>'.
                 ' <SELECT name="userid">'.$this->get_employees($userid).'</SELECT>';
        }
        else
        {
          $userid = $g_user["id"];
          $tmp = $g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">';
        }
        $data[] = array(atktext('employee','project').':',$tmp);
        $this->_addOr($data);


        // selecting functionlevel:
        $data[] = array(atktext('functionlevel','project').':','<SELECT name="functionlevelswitch">' . $this->get_functionlevels($functionlevelswitch).'</SELECT>');
        $data[] = array('<hr>');
        $data[] = array(atktext('activity').': ','<SELECT name="activity_id">'.$this->get_activities($activity_id).'</SELECT>');
        $data[] = array('<hr>');


        $startdateatt = new atkDateAttribute("startdate","","", 0, date("Ymd"),AF_OBLIGATORY);
        $enddateatt = new atkDateAttribute("enddate","","", 0, date("Ymd"),AF_OBLIGATORY);

        $timespan='<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td>'.$startdateatt->edit($dummyrec).'</td><td class="table"> &nbsp;'.
                  atktext("until").'&nbsp; </td><td align="right">'.$enddateatt->edit($dummyrec).'</td></tr></table>';
        $data[] = array(atktext("timespan","project"),$timespan);

        $page->register_script(atkconfig("atkroot")."javascript/timereg.js");

        $lookbackperiod = '<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td width="33%">';
        $lookbackperiod .= '<select name="startperiod" onchange="setStartEndDates(\'enddate\', \'startperiod\', '.date("Ymd").'); document.entryform.endperiod.value = 0; setStartEndDates(\'startdate\', \'endperiod\', '.date("Ymd").');">
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

        $lookbackperiod .= '<td width="34%" align="center">&nbsp;</td>';
        $lookbackperiod .= '<td width="33%" align="right"><select name="endperiod" onchange="setStartEndDates(\'startdate\', \'endperiod\', '.date("Ymd").'); document.entryform.startperiod.value = 0; setStartEndDates(\'enddate\', \'startperiod\', '.date("Ymd").');">
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
        $data[] = array(atktext("lookbackperiod","project"),$lookbackperiod);
        $data[] = array("<hr>");

        $remarkattr = new atkAttribute("remark", 0, 80);
        $data[] = array(atktext("remark","project"),$remarkattr->edit($dummyrec));

        foreach ($externalHourSurveyColumns as $module => $attrlist)
        {
          $data[] = array("<hr>");
          if (is_array($attrlist))
          {
            foreach($attrlist as $attr)
            {
              $currentattr = &$hoursnode->getAttribute($attr);
              if (is_object($currentattr) && !$currentattr->hasFlag(AF_HIDE_SEARCH))
              {
                if (get_class($currentattr) == "atkboolattribute")
                {
                  $output = '<select name="'.$attr.'">';
                  $output.= '<option value=""'.($dummyrec[$attr]===""?' selected':'').'>---</option>';
                  $output.= '<option value="0"'.($dummyrec[$attr]==="0"?' selected':'').'>'.atktext("no","atk").'</option>';
                  $output.= '<option value="1"'.($dummyrec[$attr]==="1"?' selected':'').'>'.atktext("yes","atk").'</option>';
                  $output.= '</select>';
                }
                else
                {
                  $output = $currentattr->edit($dummyrec);
                }
                $data[]=array($currentattr->label().": ",$output);
              }
            }
          }
        }

        $data[] = array('<hr>');
        // Get default selection items.
        $selitems = $hoursnode->getSelectionItems();

        if (is_array($selitems))
        {
          foreach ($selitems as $label=>$html)
          {
            $data[] = array(atktext($label, "", $hoursnode->m_type).":",$html);
            $data[] = array('<hr>');
          }
        }

        $data[] = array('<b>'.atktext("report_output_options","project").'</b>');
        $data[] = array(atktext("report_output_type","project").":",'<select name="outputType">
                        <option VALUE="0" selected>'.atktext("report_output_standard","project").'
                        <option value="2">'.atktext("report_output_export","project").'
                        </select>');


        $arr_collabel = array();
        for($i=0,$_i=count($arr_col);$i<$_i;$i++)
        {
          $translatewithmodule = "";
          foreach($externalHourSurveyColumns as $module => $attrlist)
          {
            if (is_array($attrlist) && in_array($arr_col[$i], array_values($attrlist)))
              $translatewithmodule = $module;
          }
          $label = atkText($arr_col[$i], $translatewithmodule);
          if ($arr_col[$i]=="billing_projecttypeid") $label = atkText("billing_type", "billing");
          $arr_collabel[] = $label;
        }

        $cell="";
        for($i=0,$_i=count($arr_col);$i<$_i;$i++)
        {
          $checked = (in_array($arr_col[$i],$col) ? "checked" : "");
          $cell.='<input type="checkbox" name="col[]" value="'.$arr_col[$i].'" '.$checked.'>'.$arr_collabel[$i].'&nbsp;';
        }
        $data[] = array(atktext('report_output_showcollumns','project'),$cell);

        $orderattrs = array();
        $orderattrlabels = array();
        $orderstr = "";
        for ($i=0,$_i=count($arr_col); $i<$_i; $i++)
        {
          $orderattr = &$hoursnode->getAttribute($arr_col[$i]);
          if (($arr_col[$i] == "functionlevel") || (is_object($orderattr) && !$orderattr->hasFlag(AF_NO_SORT)))
          {
            $orderattrs[] = $arr_col[$i];
            $orderattrlabels[] = $arr_collabel[$i];
          }
        }

        for ($i=0; $i<$sortfieldcount; $i++)
        {
          $orderbylistflags = ($i == 0) ? AF_LIST_NO_NULL_ITEM : 0;
          $field = new atkListAttribute("orderby[$i]", $orderattrlabels, $orderattrs, $orderbylistflags);
          $direction =new atkListAttribute("orderdirection[$i]",array('column_asc', 'column_desc'), array('asc', 'desc'));
          $total = new atkBoolAttribute("subtotal[$i]",0);
          $orderstr.=($i+1).". ".$field->edit($dummyrec)." ".$direction->edit($dummyrec)." ".$total->edit($dummyrec)." ".atktext('subtotal')."<br>";
        }
        $data[] = array(atktext('orderby','project').':',$orderstr);

        $boolattrib = new atkListAttribute("aggregate", array("do_not_aggregate", "aggregate_by_order"), array(0,1), AF_OBLIGATORY);
        $data[] = array(atktext('aggregate','project').':',$boolattrib->edit($dummyrec));

        $buttons = array();
        $buttons[] = '<input type="submit" value="'.atktext("refresh").'">';
        $buttons[] = '<input type="button" name="printbutton" value="'.atkText("print").'" onClick=\'if (window.print) {var frame = parent.frames.main;if (frame) {frame.focus();frame.print();}}return false;\'>';
        $data[] = array('', implode(' ', $buttons));

        for($i=0,$_i=count($data); $i<$_i; $i++)
          if (count($data[$i])==1)
            $tbl->setColSpan($i,0,2);

        $content.=$tbl->render($data);
        $content.='</form><br>';
        $page->addContent('<div class="contentblock">'.$ui->renderBox(array("title"=>$title, "content"=>'<div class="content">'.$content.'</div>')).'</div>');
      }

      // Create Filter
      $filter="activitydate>='".$startdate."' AND activitydate<='".$enddate."'";
      if(!empty($activity_id)&&$activity_id!="") $filter.=" AND hours.activityid=".$activity_id;
      if(!empty($phaseid)&&$phaseid!="") $filter.=" AND hours.phaseid=".$phaseid;
      if(!empty($projectid)&&$projectid!="") $filter.=" AND A.projectid=".$projectid;
      if(moduleExists("billing") && !empty($billing_projecttypeid) && ($billing_projecttypeid!="")) $filter.=" AND B.billing_type=$billing_projecttypeid";
      if(!empty($remark)&&$remark!="") $filter.=" AND hours.remark LIKE '%$remark%'";
      //if(!empty($customerid)&&$customerid!="") $filter.= " AND B.customer=".$customerid;
      if($department_name && $department_name != "all") $filter.=" AND userid.department='".escapeSQL($department_name)."'";
      if($functionlevelswitch && $functionlevelswitch != "all") $filter.=" AND userid.functionlevel='$functionlevelswitch'";

      foreach ($externalHourSurveyColumns as $module => $attrlist)
      {
        if (is_array($attrlist)) 
        { 
          foreach ($attrlist as $attr)
          {
            $var = "external$attr";
            $value = $$var;
            if (!empty($value)) $filter.=" AND hours.$attr = '$value'";
          }
        }
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
            $itsOrderby = ($itsOrderby == "functionlevel") ? "functionlevelid.name" : "hours.".$itsOrderby;

            $orderbystatement[] = $itsOrderby . " " . $orderdirection[$key];
          }
        }
      }

      //The id field needs to be added too, or else the dispatch url doesn't know which record to edit.
      if(!in_array("id",$col))
        $col[] = "id";

      $data = $hoursnode->selectDb($filter,implode(",",$orderbystatement), "", "", $col);
      if ($outputType=="0")
      {
        $title = atktext("title_hoursurvey_hours")." ".atktext("report_intimespan")." ".$startdate." t/m ".$enddate;
        $content = '<br><b>'.$title.'</b><br>';
        $content.= '<br>'.(atkLevel()>0?atkButton('<< '.atktext("back"), "", SESSION_BACK)."<br>":"").'<br>';

        $actions = array();
        if (!$aggregate)
        {
          $actions = $hoursnode->defaultActions("list");
        }

        $rl = &atknew("atk.recordlist.atkrecordlist");
        $content.=$rl->render($hoursnode, $data, $actions, RL_EXT_SORT, $suppress);
        $content.='<br>'.(atkLevel()>0?atkButton('<< '.atktext("back"), "", SESSION_BACK)."<br>":"").'<br>';
        $page->addContent('<div class="contentblock">'.$ui->renderBox(array("title"=>$title, "content"=>'<div class="content">'.$content.'</div>')).'</div>');
      }

      if ($outputType=="2")
      {
        // Custom recordList can export to file
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

    function get_departments($depid)
    {
      $db = &atkGetDb();

      $sql = "SELECT id,name
              FROM employee_department
              ORDER BY name";

      $records = $db->getrows($sql);
      $depcode = "<OPTION VALUE='all'>".atktext('alldepartments')."</OPTION>";
      foreach ($records as $onedep)
      {
        $sel = $onedep['id'] == $depid ? " SELECTED" : "";
        $depcode .= "<OPTION VALUE='".$onedep['id']."'$sel>".$onedep['name']."</OPTION>";
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
      if ($user_id == "new" || !$user_id || $user_id=="") { $sel = " SELECTED"; } else { $sel = ""; }
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
    function _addOr(&$data)
    {
      $data[] = array(atkText("or","project"));
    }

    /**
     * adds all required hidden fields to the form
     * @param layout object the layout object we have to hook ourselves in to
     * @return void
     */
    function _addHiddenFields(&$content)
    {
      $content.='<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">';
      $content.='<input type="hidden" name="atkaction" value="report">';
    }
  }

?>
