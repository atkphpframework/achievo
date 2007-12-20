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
    // Both grouping and summarising one column is useless and would give an error, so just don't do it.
    if ((count($this->m_groupbyfields) == 1) &&($this->fieldName() == $this->m_groupbyfields[0]))
    return;

    $query->addField("SUM(".$tablename.".".$this->fieldName().")","","",$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));

    foreach($this->m_groupbyfields as $field)
    {
      if ($field!=$this->fieldName()) // can't aggregate on ourselves
      {
        $groupby = $tablename.".".$field;
        if ($groupby == "hours.projectid") { $groupby = "phaseid.projectid"; }
        if ($groupby == "hours.functionlevel") { $groupby = "userid.functionlevel"; }
        if ($groupby == "hours.creditor_id") { $groupby = "userid.creditor_id"; }
        $query->addGroupBy($groupby);
      }
    }
  }

  function db2value($rec)
  {
    $rec["time"] = $rec['SUM(hours.time)'];
    return parent::db2value($rec);
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
    $phaseid = $record["phaseid"]["id"];
    $db = &atkGetDb();
    $query = "SELECT C.description FROM phase A LEFT JOIN project B ON A.projectid=B.id LEFT JOIN billing_projecttype C ON B.billing_type = C.id WHERE A.id='$phaseid'";
    $recs = $db->getRows($query);
    return $recs[0]["description"];
  }
}

class creditorViewAttribute extends atkAttribute {

  /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
  function storageType() { return NOSTORE; }

  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
  {
    $query->addJoin("organization", "creditor_id", "userid.creditor_id = creditor_id.id", true);
    $query->addField("name","","creditor_id","creditor_",!$this->hasFlag(AF_NO_QUOTES),true);
  }
  function db2value($record)
  {
    return atkArrayNvl($record, "creditor_name");
  }
}

class hoursurvey extends atkNode
{
  function hoursurvey()
  {
    $this->atkNode("hoursurvey");
  }

  function getOrderByStatement($orderby, $orderdirection)
  {
    $orderbystatement = array();
    if ($orderby)
    {
      foreach($orderby as $key => $itsOrderby)
      {
        if ($itsOrderby && $itsOrderby != "-")
        {
          //We need to add the tablename before the attribute.
          switch($itsOrderby)
          {
            //functionlevel has another table- AND fieldname
            case "functionlevel":
              $itsOrderby = "functionlevelid.name";
              break;
              //creditor_id has another table- AND fieldname
            case "creditor_id":
              $itsOrderby = "creditor_id.name";
              break;
              //projectid has the table phaseid
            case "projectid":
              $itsOrderby = "phaseid.".$itsOrderby;
              break;
              //Other attributes have the table hours
            default:
              $itsOrderby = "hours.".$itsOrderby;
              break;
          }

          $orderbystatement[] = $itsOrderby . " " . $orderdirection[$key];
        }
      }
    }
    return $orderbystatement;
  }

  function action_report(&$handler)
  {
    global $g_user, $g_sessionManager;

    require_once(moduleDir('report').'utils/class.reportutils.inc');

    $repUtils = &atknew('report.utils.reportutils');

    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");
    $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
    $page->register_style(moduleDir("reports")."style/printablereport.css", "print");

    $contentblocks = array();

    // Manual sort config
    $orderby =            atkArrayNvl($this->m_postvars, "orderby", "");
    $orderdirection =     atkArrayNvl($this->m_postvars, "orderdirection", "");
    $subtotal =           atkArrayNvl($this->m_postvars, "subtotal", "");

    $activitypair =       explode("=",$g_sessionManager->pageVar("activityid"));
    $activity_id =        stripQuotes($activitypair[1]);
    $projectpair =        explode("=",$g_sessionManager->pageVar("projectid"));
    $projectid =          stripQuotes($projectpair[1]);
    $categorypair =       explode("=",$g_sessionManager->pageVar("project_category"));
    $category =           stripQuotes($categorypair[1]);
    if(!isset($category)) $category = "";
    
    if (moduleExists("billing"))
    {
      $billing_projecttypepair = $g_sessionManager->pageVar("billing_projecttypeid");
      $billing_projecttypeid = (!empty($billing_projecttypepair)) ? atkArrayNvl(decodeKeyValuePair($billing_projecttypepair), "billing_projecttype.id", "") : "";
    }

    if (moduleExists('advancedsecurity'))
      $lowerlevels = atkArrayNvl($this->m_postvars, "lowerlevels", 'off');

    $phasepair =          explode("=",$g_sessionManager->pageVar("phaseid"));
    $phaseid =            stripQuotes($phasepair[1]);

    //if we only have a phaseid, retrieve the projectid.
    if(!empty($phaseid) && empty($projectid))
    {
      $node = &atkGetNode("project.phase");
      /* @var $node atkNode */
      $records = $node->selectDb(sprintf("phase.id='%d'",$phaseid),"","","",array("id","projectid"));

      if(count($records))
      $projectid= $records[0]["projectid"]["id"];
      else //We have an incorrect phaseid, we clear the phaseid
      {
        $g_sessionManager->pageVar("phaseid","");
        $phaseid = "";
      }

    }

    $userid =             $g_sessionManager->pageVar("userid");
    $nameswitch =         $g_sessionManager->pageVar("nameswitch");
    $roleid =             $g_sessionManager->pageVar("roleid");
    $remark =             $g_sessionManager->pageVar("remark");
    $aggregate =          $g_sessionManager->pageVar("aggregate");
    $startdate =          $g_sessionManager->pageVar("startdate");
    $enddate =            $g_sessionManager->pageVar("enddate");
    $col =                $g_sessionManager->pageVar("col");
    $organizationpair =   explode("=",$g_sessionManager->pageVar("organizationid"));
    $organizationid =     stripQuotes($organizationpair[1]);
    if(!isset($organizationid)) $organizationid = "";

    if(atkconfig::get("timereg","timereg_contact_link",false))
    {
      $contact_pair =       explode("=",$g_sessionManager->pageVar("contact_id"));
      $contact_id =         stripQuotes($contact_pair[1]);
      if(!isset($contact_id)) $contact_id = "";
    }

    $department_name =    $g_sessionManager->pageVar("departmentid");
    $functionlevelswitch =$g_sessionManager->pageVar("functionlevelswitch");
    $creditor_id =        $g_sessionManager->pageVar("creditor_id");
    $outputType =         atkArrayNvl($this->m_postvars, "outputType", "");
    $subProject =         atkArrayNvl($this->m_postvars, "subproject", 0);

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

    $hoursnode = &atkGetNode($this->getHoursNodeName());

    $hoursnode->m_postvars = $this->m_postvars; // Share postvars.

    $hoursnode->add(new functionLevelAttribute("functionlevel",AF_FORCE_LOAD));
    if(moduleExists("billing"))
    {
      $attbilling_projecttypeid = &$hoursnode->add(new billingTypeViewAttribute("billing_projecttypeid"));
      $attbilling_projecttypeid->m_ownerInstance = &$hoursnode;
    }
    $hoursnode->add(new creditorViewAttribute("creditor_id", AF_FORCE_LOAD));

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
    else
    {
      $orderby = array();
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

    $view_all = $this->allowed('view_all');
    $view_managed = $this->allowed('view_managed');

    if (moduleExists('advancedsecurity'))
      $view_level = $this->allowed('view_level');
    else
      $view_level = false;

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
    $hoursnode->m_attribList["userid"]->m_flags = ($hoursnode->m_attribList["userid"]->m_flags&(~AF_HIDE_LIST));


    if ($outputType==0)
    {
      $title = atktext("parameters", $this->m_module, $this->m_type);

      $content='<form action="'.getDispatchFile().'" method="get" name="entryform" id="entryform">';
      $content.=session_form();

      $this->_addHiddenFields($content);

      $current = unserialize(atkArrayNvl($this->m_postvars,'projects'));
      
      if(isset($this->m_postvars['add_project']) || ($current && !empty($projectid)) )
      {
        //add_project button has been pressed
        $list = $this->_addHiddenProjects($content,array($projectid, $phaseid),null,$current);
        //clear current project/phase selection and remove it from stack
        $projectid = $phaseid = "";
        $g_sessionManager->pageVar("projectid"," ");
        $g_sessionManager->pageVar("phaseid"," ");
      }
      else 
      {
        $list = $this->_addHiddenProjects($content,array(),atkArrayNvl($this->m_postvars,"delete_project"),$current);
      }
      
      $tbl = &atknew("atk.utils.atktablerenderer");
      $data[] = array("<b>".atkText("sethoursfilter","project")."</b>");
      $data[] = array('<hr>'.atktext('searchbyprojectorphase','project').':');
      
      // we have to pass a 'dummy' record to the attributes to set their default value.
      $dummyrec = array(
      "projectid"     =>array("id"=>$projectid),
      "project_category" => array("id"=>$category),
      "subproject"   =>$subProject,
      "organizationid"=>array("id"=>$organizationid), // manytoonerelation needs subarray
      "phaseid"       =>array("id"=>$phaseid),
      "activityid"    =>array("id"=>$activity_id),
      "startdate"     =>array("year"=>substr($startdate,0,4),
      "month"         =>substr($startdate,5,2),
      "day"           =>substr($startdate,8,2)),
      "enddate"       =>array("year"=>substr($enddate,0,4),
      "month"         =>substr($enddate,5,2),
      "day"           =>substr($enddate,8,2)),
      "aggregate"     =>$aggregate,
      "remark"  =>$remark
      );

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $dummyrec["contact_id"] = array("id"=>$contact_id);
      }

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

      $projectatt = &$hoursnode->m_attribList["projectid"];
      $projectatt->addDependee("phaseid");

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $projectatt->addDependee("contact_id");
      }

      $projectatt->m_ownerInstance = &$hoursnode;
      $projectatt->_registerDependees();

      $sortfieldcount = max(3, $columnConfig->countSortAttribs());

      for ($i=0; $i<$sortfieldcount; $i++)
      {
        $dummyrec["orderby[$i]"]=$columnConfig->getAttributeByOrder($i+1);
        $dummyrec["orderdirection[$i]"]=$columnConfig->getDirectionByOrder($i+1);
        $dummyrec["subtotal[$i]"]=($columnConfig->hasSubTotalByOrder($i+1)?1:0);
      }

      if(!empty($list))
      {
        $data[] = array(atktext('projectlist','project').':',$this->_drowProjectsList($list));
        $data[] = array('<hr>');
      }
      
      $text = atktext("add_project");
      $link = atkHref(dispatch_url("reports.hoursurvey", "report",array("add_project"=>1)), $text,SESSION_DEFAULT,true);

      $data[] = array(atktext('project','project').':',$projectatt->edit($dummyrec)." ".$link);

      
      $p_attr = new atkBoolAttribute("subproject");
      $p_attr->addDependee("phaseid");
      $p_attr->m_ownerInstance = &$hoursnode;
      $p_attr->_registerDependees();

      $data[] = array(atktext('subproject','project').':',$p_attr->edit($dummyrec));

      $phaseatt = &$hoursnode->m_attribList["phaseid"];
      $phaseatt->setDestinationFilter("phase.projectid='[projectid.id]'");
      $phaseatt->m_ownerInstance = &$hoursnode;

      $g_sessionManager->stackVar("reportnodetype","hoursurvey");

      //We add an array as a row to the data with the value and id fields,
      //to make the atkTableRenderer render the id in the TD of this attribute.
      $data[] = array(atktext('phase','project').':',
      array('value'=>$phaseatt->edit($dummyrec),
      'id'=>str_replace(".","_",$this->getHoursNodeName())."_".$phaseatt->m_name)
      );

      $this->_addOr($data);
      
      $p_attr = new atkManyToOneRelation("project_category","project.project_category");
      $p_attr->m_ownerInstance = &$hoursnode;

      $data[] = array(atktext('project_category','project').':',$p_attr->edit($dummyrec));

      $this->_addOr($data);

      if (moduleExists("billing"))
      {
        $billingRateNode = &atkGetNode("billing.billing_rate");
        $projecttypeatt = &new atkManyToOneRelation('billing_projecttypeid', "billing.projecttype");
        $projecttypeatt->m_ownerInstance = &$billingRateNode;
        $projecttypeatt->removeFlag(AF_RELATION_AUTOCOMPLETE);
        $data[] = array(atktext('billing_type','billing').':',$projecttypeatt->edit($dummyrec));
        $this->_addOr($data);
      }

      $organizationatt = &$hoursnode->getAttribute("organizationid");

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $organizationatt->addDependee("contact_id");
        $organizationatt->m_ownerInstance = &$hoursnode;
        $organizationatt->_registerDependees();
      }

      $data[] = array(atktext("organization","project").':','<SELECT name="roleid">'.$this->get_roles($roleid).'</SELECT> '.$organizationatt->edit($dummyrec));
      $this->_addOr($data);

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $contact_id_att = &$hoursnode->getAttribute("contact_id");
        // addDependee don't work without id
        $data[] = array(atktext("contact_id").':',
        array('value'=>$contact_id_att->getEdit("edit",$dummyrec,''),
        'id'=>str_replace(".","_",$this->getHoursNodeName())."_".$contact_id_att->m_name)
        );
        $this->_addOr($data);
      }

      $data[] = array(atkText("department","project").':','<SELECT name="departmentid">'.$this->get_departments($department_name).'</SELECT>');
      $this->_addOr($data);

      $data[] = array(atkText("creditor_id","employee").':','<SELECT name="creditor_id">'.$this->get_creditors($creditor_id).'</SELECT>');

      $data[] = array("<hr>");


      if($view_all)
      {
        $tmp = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>'.
        ' <SELECT name="userid">'.$repUtils->get_employees($userid, true).'</SELECT>';
      }
      elseif($view_managed && !$view_level)
      {
        $tmp = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>'.
        ' <SELECT name="userid">'.$repUtils->get_employees($userid).'</SELECT>';
      }
      elseif ($view_level)
      {
        $tmp = '<SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT>'.
        ' <SELECT name="userid">'.$repUtils->getLevelEmployees($userid).'</SELECT>';
      }
      else
      {
        $userid = $g_user["id"];
        $tmp = $g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">';
      }
      $data[] = array(atktext('employee','project').':',$tmp);
      $this->_addOr($data);


      // selecting functionlevel:
      $data[] = array(atktext('functionlevel','project').':',$repUtils->get_functionlevels($functionlevelswitch, $lowerlevels));
      $data[] = array('<hr>');
      $activity = &$hoursnode->m_attribList["activityid"];
      $activity->m_flags = 0;
      $data[] = array(atktext('activity').': ',$activity->edit($dummyrec));
      $data[] = array('<hr>');

      $startdateatt = new atkDateAttribute("startdate","","", 0, date("Ymd"),AF_OBLIGATORY);
      $enddateatt = new atkDateAttribute("enddate","","", 0, atkConfig("timereg_allowfuture", false) ? 0 : date("Ymd"), AF_OBLIGATORY);

      $timespan='<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td>'.$startdateatt->edit($dummyrec).'</td><td class="table"> &nbsp;'.
      atktext("until").'&nbsp; </td><td align="right">'.$enddateatt->edit($dummyrec).'</td></tr></table>';
      $data[] = array(atktext("timespan","project"),$timespan);

      $page->register_script(atkconfig("atkroot")."javascript/timereg.js");

      $lookbackperiod = '<table width="100%" cellpadding="0" cellspacing="0" border="0"><tr><td width="33%">';
      $lookbackperiod .= '<select name="startperiod" onchange="Achievo.Timereg.setStartEndDates(\'startdate\', \'enddate\', document.entryform.startperiod.value, -1); document.entryform.startperiod.value = 0;">
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
      $lookbackperiod .= '<td width="33%" align="right"><select name="endperiod" onchange="Achievo.Timereg.setStartEndDates(\'startdate\', \'enddate\', document.entryform.endperiod.value, 1); document.entryform.endperiod.value = 0;">
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
              if (strtolower(get_class($currentattr)) == "atkboolattribute")
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

      $cell="";
      $showcolumnstbl = &atknew("atk.utils.atktablerenderer");
      $showcolumnsdata = array(array());
      for($i=0,$_i=count($arr_col);$i<$_i;$i++)
      {
        $checked = (in_array($arr_col[$i],$col) ? "checked" : "");
        $cell ='<input type="checkbox" name="col[]" class="atkcheckbox" value="'.$arr_col[$i].'" '.$checked.'>'.$arr_collabel[$i].'&nbsp;';
        if (count($showcolumnsdata[count($showcolumnsdata)-1]) == 4)
        $showcolumnsdata[] = array();
        $showcolumnsdata[count($showcolumnsdata)-1][] = $cell;
      }
      $data[] = array(atktext('report_output_showcollumns','project'),$showcolumnstbl->render($showcolumnsdata));

      for ($i=0; $i<$sortfieldcount; $i++)
      {
        $orderbylistflags = ($i == 0) ? AF_LIST_NO_NULL_ITEM : 0;
        $field = new atkListAttribute("orderby[$i]", $orderattrlabels, $orderattrs, $orderbylistflags);
        $direction =new atkListAttribute("orderdirection[$i]",array('column_asc', 'column_desc'), array('asc', 'desc'));
        $total = new atkBoolAttribute("subtotal[$i]",0);
        $orderstr.=($i+1).". ".$field->edit($dummyrec)." ".$direction->edit($dummyrec)." ".$total->edit($dummyrec)." ".atktext('subtotal')."<br>";
      }
      $data[] = array(atktext('orderby','project').':',$orderstr);

      $boolattrib = new atkListAttribute("aggregate", array("do_not_aggregate", "aggregate_by_order"), array(0,1), AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM);
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
      if (atkConfig("report_hide_parameters", false))
        $contentblocks[] = '<div class="displaynone">'.$ui->renderBox(array("title"=>$title, "content"=>$content)).'</div>';
      else
        $contentblocks[] = '<div class="contentblock">'.$ui->renderBox(array("title"=>$title, "content"=>'<div class="content">'.$content.'</div>')).'</div>';
    }

    /** Create filter array **/
    $filterarray = array();
    $filterarray[] = $this->_addFilterArrayItem("activitydate", ">='$startdate'", "hours");
    $filterarray[] = $this->_addFilterArrayItem("activitydate", "<='$enddate'", "hours");

    if(!empty($activity_id)&&$activity_id!="")
      $filterarray[] = $this->_addFilterArrayItem("activityid","='$activity_id'","hours");

    //If we use multi project, we don't take into account project/phase,
    //selected in other controls
    if(!empty($list))
    {
      $tmp = array();
      foreach ($list as $l)
      {
        $line = "(phaseid.projectid='".$l[0]."'";
        if(!empty($l[1])&&$l[1]!="") $line .= " AND hours.phaseid='".$l[1]."'";
        $line .= ")";
        $tmp[] = $line;
      }
      $all = "(".implode(" OR ",$tmp).")";
      $filterarray[] = $this->_addFilterArrayItem($all,'');
    }
    else 
    {
      if(!empty($phaseid)&&$phaseid!="")
        $filterarray[] = $this->_addFilterArrayItem("phaseid","='$phaseid'","hours");
  
      if(!empty($projectid)&&$projectid!="")
      {
        if($subProject)
        {
          $id = $hoursnode->_getProjectAndSubProjectId($projectid);
          $condition = "IN(".implode(",",$id).")";
          $filterarray[] = $this->_addFilterArrayItem("projectid",$condition,"phaseid");
        }
        else
          $filterarray[] = $this->_addFilterArrayItem("projectid","='$projectid'","phaseid");
      }
    }
        
    if($category)
      $filterarray[] = $this->_addFilterArrayItem("project_category","='$category'","projectid");
        
    if(moduleExists("billing") && !empty($billing_projecttypeid) && ($billing_projecttypeid!=""))
      $filterarray[] = $this->_addFilterArrayItem("billing_type","='$billing_projecttypeid'","projectid");

    if(!empty($remark)&&$remark!="")
      $filterarray[] = $this->_addFilterArrayItem("remark","LIKE lower('%".escapeSQL($remark)."%')","hours");

    if($department_name && $department_name != "all")
      $filterarray[] = $this->_addFilterArrayItem("department","='".escapeSQL($department_name)."'","userid");

    if($functionlevelswitch && $functionlevelswitch != "all")
    {
      if (moduleExists('advancedsecurity') && $lowerlevels == 'on')
      {
        require_once(moduleDir('advancedsecurity').'utils/class.functionlevelsutils.inc');

        $flutils = &atknew('advancedsecurity.utils.functionlevelutils');

        $functionlevels = $flutils->getFunctionLevelBelow($functionlevelswitch, true);

        if(is_array($functionlevels) && count($functionlevels) == 1)
          $filterarray[] = $this->_addFilterArrayItem("functionlevel"," = '".$level['id']."'","userid");
        elseif (is_array($functionlevels) && count($functionlevels) > 1)
        {
          foreach ($functionlevels as $level)
            $levelStr .= $level['id'].',';

          $levelStr = substr($levelStr,0,-1);

          $filterarray[] = $this->_addFilterArrayItem("functionlevel"," IN ($levelStr)","userid");
        }
      }
      else
        $filterarray[] = $this->_addFilterArrayItem("functionlevel"," = '".$functionlevelswitch."'","userid");
    }

    if(!empty($creditor_id) && $creditor_id != "all")
    $filterarray[] = $this->_addFilterArrayItem("creditor_id","='$creditor_id'","userid");

    //Add external columns to the filterarray
    foreach ($externalHourSurveyColumns as $module => $attrlist)
    {
      if (is_array($attrlist))
      {
        foreach ($attrlist as $attr)
        {
          $var = "external$attr";
          $value = $$var;
          if (!empty($value))
            $filterarray[] = $this->_addFilterArrayItem($attr,"='$value'","hours");

        }
      }
    }

    // employee table is not yet added to the query. Force that by adding a manytoonerel,
    // so we display a users name.
    $hoursnode->add(new atkManyToOneRelation("userid", "employee.employee",AF_FORCE_LOAD));

    // organization and person_organization tables are not yet added to the query.
    // Since this is a many:many thing between projects and organizations, with the
    // contactpersons als intermediairy, we have to add a special attribute which
    // nests itself nicely in the query.
    if ($organizationid!="")
    {
      useattrib("project.filterprojectbyorganizationattrib");
      $hoursnode->add(new filterProjectByOrganizationAttrib($organizationid, $roleid, "projectid")); // the 'projectid' is an alias for the project table.

      //the organization filter needs the projectid and the phaseid to be added to the cols
      if(!in_array("projectid",$col))
      $col[] = "projectid";

      if(!in_array("phaseid",$col))
      $col[] = "phaseid";
    }
    if(atkconfig::get("timereg","timereg_contact_link",false))
    {
      if(!in_array("contact_id",$col))
      $col[] = "contact_id";
      if ($contact_id!="")
      {
        $filterarray[] = $this->_addFilterArrayItem("contact_id","='$contact_id'","hours");
      }
    }

    $orderbystatement = $this->getOrderByStatement($orderby, $orderdirection);

    //Check if we need to add additional columns because of the selected filter.
    foreach($filterarray as $item)
    {
      switch($item["table"].".".$item["columnname"])
      {
        case "phaseid.projectid":
          if(!in_array("phaseid",$col))
            $col[] = "phaseid";
          break;
        case "projectid.billing_type":
          if(!in_array("projectid",$col))
            $col[] = "projectid";
          break;
        default:
          //do nothing
          break;
      }
    }

    //Check if we need to add additional columns because of the selected order-by.
    foreach($orderby as $key =>$itsOrderby)
    {
      if ($itsOrderby && $itsOrderby != "-")
      {
        switch($itsOrderby)
        {
          case "phaseid":
            if(!in_array("phaseid",$col))
              $col[] = "phaseid";
          default:
            //do nothing
            break;
        }
      }
    }

    //The id field needs to be added too, or else the dispatch url doesn't know which record to edit.
    if(!in_array("id",$col))
    $col[] = "id";

    //render filter
    $filter = "";

    for($i=0,$_i=count($filterarray);$i<$_i;$i++)
    {
      if($i!=0)
        $filter.= " AND ";

      $filter.= ($filterarray[$i]["table"]!="") ? $filterarray[$i]["table"]."." : "";
      $filter.= $filterarray[$i]["columnname"]." ".$filterarray[$i]["condition"];
    }

    $data = $this->getHourRecords($nameswitch, $userid, $filter, $orderbystatement, $col);
    
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
      // make the recordlist think its operating on reports node.
      $rl->setMasterNode($this);
      $content.=$rl->render($hoursnode, $data, $actions, RL_EXT_SORT|RL_MRA|RL_NO_SORT, $suppress);
      $content.='<br>'.(atkLevel()>0?atkButton('<< '.atktext("back"), "", SESSION_BACK)."<br>":"").'<br>';
      $contentblocks[] = '<div class="contentblock">'.$ui->renderBox(array("title"=>$title, "content"=>'<div class="content">'.$content.'</div>')).'</div>';
      $actionpage = $this->renderActionPage("admin", $contentblocks);
      $page->addContent($actionpage);;
      $page->register_scriptcode("var atkErrorFields = new Array();");
    }

    if ($outputType=="2")
    {
      // Custom recordList can export to file
      $rl = &atknew("atk.recordlist.atkcustomrecordlist");
      $rl->render($hoursnode, $data, "", '"', '";', "\r\n", "1", "",$suppress);
    }
  }

  function getHoursNodeName()
  {
    return "reports.surveyhours";
  }

  function getHourRecords($nameswitch, $user, $filter, $orderby, $col)
  {
    require_once(moduleDir('report').'utils/class.reportutils.inc');

    $repUtils = &atknew('report.utils.reportutils');

    if ($nameswitch == 'supervisor')
    {
      $employees = $repUtils->getEmployeesArray($user);
      $employees[]['id'] = $user;

      $hourrecords = array();

      foreach ($employees as $employee)
      {
        $filterExtended = $filter;

        if ($user != 'all')
          $and = " AND userid.supervisor = '".$employee['id']."'";

        $filterExtended .= $and;

        $records = $this->getHourRecordsFromDB($filterExtended, $orderby, $col);

        if (is_array($records) && count($records) > 0)
          $hourrecords = array_merge($hourrecords, $records);
      }

      return $hourrecords;
    }
    else
    {
      if ($user != "all")
        $filter.= " AND hours.userid ='$user' ";

      return $this->getHourRecordsFromDB($filter, $orderby, $col);
    }
  }

  function getHourRecordsFromDB($filter, $orderby, $col)
  {
    $hoursnode = &atkGetNode("timereg.hours");

    $records = $hoursnode->selectDb($filter,implode(",",$orderby), "", "", $col);

    if (is_array($records) && count($records) > 0)
      return $records;

    return null;
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

    if($act_id==-1)
      $sel="SELECTED";
    else
      $sel="";

    $activity_code='<OPTION VALUE="" '.$sel.'>'.atktext('allactivities').'</OPTION>';
    for($i=0,$_i=count($records);$i<$_i;$i++)
    {
      if($act_id==$records[$i]["id"])
        $sel="SELECTED";
      else
        $sel="";

      $activity_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $activity_code;
  }

  function _addFilterArrayItem($columnname, $condition, $tablename="")
  {
    return array(
    "table"       => $tablename,
    "columnname"  => $columnname,
    "condition"   => $condition
    );
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

  function get_creditors($creditor_id)
  {
    $db = &atkGetDB();

    $sql = "SELECT organization.id,organization.name,count(organization.id) AS cnt
              FROM person
              INNER JOIN organization ON person.creditor_id = organization.id
              WHERE person.role='employee'
              GROUP BY organization.id";

    $records = $db->getrows($sql);
    $creditorscode = "<OPTION VALUE='all'>".atktext('allcreditors')."</OPTION>";
    foreach ($records as $creditor)
    {
      $sel = $creditor['id'] == $creditor_id ? " SELECTED" : "";
      $creditorscode .= "<OPTION VALUE='".$creditor['id']."'$sel>".$creditor['name']."</OPTION>";
    }
    return $creditorscode;
  }

  function get_employees($user_id)
  {
    $user = getUser();

    if (is_array($user))
    {
      $records = $this->getEmployeesArray($user['id']);

      if ($user_id == "new" || !$user_id || $user_id=="")
        $sel = " SELECTED";
      else
        $sel = "";

      $employee_code='<OPTION VALUE="all"'.$sel.'>'.atktext("allusers");
      $employee_code .= '<OPTION VALUE="'.addslashes($user["id"]).'" '.$sel.'>'.$user["lastname"].', '.$user["firstname"].'</OPTION>';
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if($user_id==$records[$i]["id"])
          $sel="SELECTED";
        else
          $sel="";

        $employee_code.='<OPTION VALUE="'.addslashes($records[$i]["id"]).'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      return $employee_code;
    }
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
      if ($role == $records[$i]["id"])
        $sel = "SELECTED";
      else
        $sel = "";

      $role_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].':</OPTION>';
    }
    return $role_code;
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
  
  function _addHiddenProjects(&$content,$item=array(),$id=null,$current)
  {
    if(isset($id))
    {
      unset($current[$id]);
      $this->_hiddenProjects($content, $current);
    }
    elseif( !empty($current) )
    {
      if(!empty($item))
      {
        $current[] = $item;
        $this->_hiddenProjects($content,$current);
      }
      else
      {
        $this->_hiddenProjects($content,$current);
      }
      
    }
    elseif(!empty($item)) 
    {
      $current = array($item);
      $this->_hiddenProjects($content,$current);
    }
    return $current;
  }

  function _hiddenProjects(&$content,$value='')
  {
    if(!empty($value))
    {
      $content .= '<input type="hidden" name="projects" '.'value="'.htmlspecialchars(serialize($value), ENT_QUOTES).'">';
    }
  }
  
  function _drowProjectsList($res)
  {
    $content = '';
    if(atk_value_in_array($res))
    {
      $data = array();
      $data[] = array("",atktext("project"),atktext("phase"));
      $prj = &atkGetNode("project.project");
      $phs = &atkGetNode("project.phase");
      foreach ($res as $key=>$value)
      {
        $link = atkHref(dispatch_url("reports.hoursurvey", "report",array("delete_project"=>$key)), "X",SESSION_DEFAULT,true);
        $data[] = array($link,$this->_getDescriptor($prj,"project.id='".$value[0]."'"), $this->_getDescriptor($phs,"phase.id='".$value[1]."'"));
      }
      
      $tr = atknew("atk.utils.atktablerenderer");
      $content = $tr->render($data, TBL_HEADER|TBL_ALTERNATE, "recordlist");    
    }
    return $content;
  }

  function _getDescriptor(&$node,$selector)
  {
    list($record) = $node->selectDb($selector,"","","",$node->descriptorFields());
    return $node->descriptor($record);
  }
}
?>
