<?php

  userelation("atkmanytoonerelation");
  
  class hoursurvey extends atkNode
  {
    function hoursurvey()
    {
      $this->atkNode("hoursurvey");
    }

    function action_report()
    {
      global $g_layout, $g_user;
      $atkorderby = $this->m_postvars["atkorderby"];
      $activity_id = $this->m_postvars["activity_id"];
      $projectid = $this->m_postvars["projectid"];
      $selectedprojectid = $this->m_postvars["selectedprojectid"];
      $phaseid = $this->m_postvars["phaseid"];
      $userid = $this->m_postvars["userid"];
      $nameswitch = $this->m_postvars["nameswitch"];
      $roleid = $this->m_postvars["roleid"];

      /** $outputType gives control over the kind of output:
       * 0 standard (onscreen in compliance with theme, default)
       * 1 printable (onscreen in a format that easily can be printed on paper))
       * 2 export ( to a file on local computer ex. achievo.csv)
       */
      $outputType = $this->m_postvars["outputType"];

      if (empty($outputType))
      {
        $outputType=0;   //default=standard
      }

      $startdate = $this->m_postvars["startdate"];
      $enddate = $this->m_postvars["enddate"];
      $col = $this->m_postvars["col"];

      // the selected customer_id is found as "customer.id='3'" in the postvars
      $organizationarr = decodeKeyValuePair($this->m_postvars["organizationid"]);
      $organizationid = $organizationarr["organization.id"];

      $hoursnode = &getNode("timereg.hours");


      // Determine collumn suppresslist:
      $arr_col = array("activitydate","userid","phaseid","activityid","remark","time");

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
        $g_layout->ui_top(text("title_hours_stats"));

        $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');
        $g_layout->output(session_form());

        $g_layout->output('<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">');
        $g_layout->output('<input type="hidden" name="atkaction" value="'.$this->m_action.'">');
        $g_layout->table_simple();
        $g_layout->output('<tr>');
        $g_layout->td('<b>'.text('sethoursfilter').'</b>', 'colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td('<hr>'.text('searchbyprojectorphase').':','colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('project').':');
        $projectatt = new atkProjectAttribute('projectid');

        // we have to pass a 'dummy' record to the attributes to set their default value.
        $dummyrec = Array("projectid"=>$projectid,
                          "organizationid"=>array("id"=>$organizationid), // manytoonerelation needs subarray
                          "phaseid"=>Array("phaseid"=>$phaseid), // is implemented as an array in projphaseattrib.
                          "startdate"=>array("year"=>substr($startdate,0,4),
                                             "month"=>substr($startdate,5,2),
                                             "day"=>substr($startdate,8,2)),
                          "enddate"=>array("year"=>substr($enddate,0,4),
                                           "month"=>substr($enddate,5,2),
                                          "day"=>substr($enddate,8,2)),
                          "atkorderby"=>$atkorderby);

        $g_layout->td($projectatt->edit($dummyrec));
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('or'),'colspan="2"');
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('phase').':');
        $phaseatt = $hoursnode->m_attribList["phaseid"];
        $phaseatt->m_flags = $phaseatt->m_flags&~AF_JS_UPDATEACTIVITY;
        $g_layout->td($phaseatt->edit($dummyrec));
        $g_layout->output('</tr><tr>');
        $g_layout->td(text('or'),'colspan="2"');
        $g_layout->output('</tr><tr>');
        //$g_layout->td(text('customer').':');
        $g_layout->td(text("organization").':');
        $organizationatt = new atkManyToOneRelation("organizationid", "organization.organization");
        $g_layout->td('<SELECT name="roleid">'.$this->get_roles($roleid).'</SELECT> '.
                      $organizationatt->edit($dummyrec));
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
          $g_layout->td($g_user["id"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">');
        }
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

        $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                        text("until").
                        '&nbsp; '.$enddateatt->edit($dummyrec));

        $g_layout->output('</tr><tr>');
        $g_layout->td('<hr>','colspan="2"');
        $g_layout->output('</tr><tr>');

        $g_layout->td('<b>'.text("report_output_options").'</b>','colspan="2"');

        $g_layout->output('</tr><tr>');

        $g_layout->td(text("report_output_type"));
        $g_layout->td('<select name="outputType">
                         <option VALUE="0" selected>'.text("report_output_standard").'
                         <option value="1">'.text("report_output_printable").'
                         <option value="2">'.text("report_output_export").'
                         </select>');

        $g_layout->output('</tr><tr>');

        //$g_layout->td(text("PDF"));
        //$g_layout->td('<input type="checkbox" name="pdf" value="1">');
        //$g_layout->output('</tr><tr>');

        $g_layout->td(text("report_output_showcollumns"));

        $cell="";
        for($i=0,$_i=count($arr_col);$i<$_i;$i++)
        {
          $checked = (in_array($arr_col[$i],$col) ? "checked" : "");
          $cell.='<input type="checkbox" name="col[]" value="'.$arr_col[$i].'" '.$checked.'>'.text($arr_col[$i]).'&nbsp;';
        }
        $g_layout->td($cell);

        $g_layout->output('</tr><tr>');

        $g_layout->td(text("orderby").': ');

        $listcol = array();
        for($i=0,$_i=count($arr_col);$i<$_i;$i++) $listcol[] = "hours.".$arr_col[$i];

        $orderatt = new atkListAttribute("atkorderby",$arr_col, $listcol);

        $g_layout->td($orderatt->edit($dummyrec));

        $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');

      }

      // Create Filter
      $filter="activitydate>='".$startdate."' AND activitydate<='".$enddate."'";
      if(!empty($activity_id)&&$activity_id!="") $filter.=" AND hours.activityid=".$activity_id;
      if(!empty($phaseid)&&$phaseid!="") $filter.=" AND hours.phaseid=".$phaseid;
      if(!empty($projectid)&&$projectid!="") $filter.=" AND A.projectid=".$projectid;
      //if(!empty($customerid)&&$customerid!="") $filter.= " AND B.customer=".$customerid;

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

      $data = $hoursnode->selectDb($filter,$atkorderby);

      if ($outputType=="0")
      {
        $g_layout->ui_bottom();
        $g_layout->ui_top(text("title_hoursurvey_hours")." ".text("report_intimespan")." ".$startdate." t/m ".$enddate);
        $g_layout->output('<br><br>');
        $g_layout->output($hoursnode->recordList($data, array(),false,$suppress));
        $g_layout->output('<br><br>');
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

    function get_employees($user_id)
    {
      global $g_db;

      $sql = "SELECT id,lastname,userid, firstname
              FROM person
              WHERE status='active' AND role='employee'
              ORDER BY lastname
             ";

      $records = $g_db->getrows($sql);
      $employee_code='<OPTION VALUE="all">'.text("allusers");
      for($i=0,$_i=count($records);$i<$_i;$i++)
      {
        if($user_id==$records[$i]["id"]) { $sel="SELECTED"; } else { $sel=""; }
        $employee_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
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
  }

?>