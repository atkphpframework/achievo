<?php

include_once("achievotools.inc");

useattrib("atkdateattribute");

class timeDisplayAttribute extends atkAttribute
{
  function display($rec)
  {
    $time = $rec[$this->fieldName()];
    if ($time==0) return "";
    return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
  }
}

function status_cmp($a, $b) { return compare_by_field($a, $b, "status"); }
function week_cmp($a, $b) { return compare_by_field($a, $b, "week"); }
function userid_cmp($a, $b) { return compare_by_field($a, $b, "userid"); }

function compare_by_field($a, $b, $field)
{
  if ($a[$field]==$b[$field])
  {
    // equal items are always sorted by week
    if ($a["week"]==$b["week"]) return 0;
    return ($a["week"] < $b["week"]) ? -1 : 1;
  }
  return ($a[$field] < $b[$field]) ? -1 : 1;
}

class weekreport extends atkNode
{
  function weekreport()
  {
    $this->atkNode("weekreport",NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT); // node() constructor is *not* called automatically!
    $this->add(new atkAttribute("userid"));
    $this->add(new atkAttribute("week"));
    $this->add(new atkAttribute("startdate"));
    $this->add(new atkAttribute("enddate"));
    $this->add(new timeDisplayAttribute("mon"));
    $this->add(new timeDisplayAttribute("tue"));
    $this->add(new timeDisplayAttribute("wed"));
    $this->add(new timeDisplayAttribute("thu"));
    $this->add(new timeDisplayAttribute("fri"));
    $this->add(new timeDisplayAttribute("sat"));
    $this->add(new timeDisplayAttribute("sun"));
    $this->add(new timeDisplayAttribute("total",AF_TOTAL));
    $this->add(new timeDisplayAttribute("contract",AF_TOTAL));
    $this->add(new timeDisplayAttribute("overtime",AF_TOTAL));
    $this->add(new atkAttribute("status"));
  }

  function status_display($rec)
  {
    return text($rec["status"]);
  }

  function get_employee($user_id)
  {
    global $g_db;
    $sql = "SELECT * FROM person WHERE status='active' AND id='$user_id'";
    $record = $g_db->getrows($sql);
    return $record[0];
  }

  function get_employees($userid)
  {
    global $g_db;

    $sql = "SELECT id, lastname,firstname,userid
            FROM person
            WHERE status='active' AND role = 'employee'
            ORDER BY lastname
           ";

    $records = $g_db->getrows($sql);

    $employee_code.='<OPTION VALUE="all">'.text("all_users").'</OPTION>';
    for($i=0;$i<count($records);$i++)
    {
      if ($userid==$records[$i]["id"]) $selected="selected";
      else $selected="";
      $employee_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$selected.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
    }
    return $employee_code;
  }


  function action_report()
	{
    global $g_layout, $g_securityManager, $g_user, $g_db;
    $atkorderby = $_REQUEST["atkorderby"];    
    $userid = $_REQUEST["userid"];
    $nameswitch = $_REQUEST["nameswitch"];
    $showdetails = $_REQUEST["showdetails"];
    $showstatus = $_REQUEST["showstatus"];

    /** $outputType gives control over the kind of output:
    *        0 standard(onscreen in compliance with theme, default)
    *        1 printable (onscreen in a format that easily can be printed on paper))
    *             [was $printable]
    *        2 export ( to a file on local computer ex. achievo.csv)
    */
    $outputType = $_REQUEST["outputType"];
    if (!isset($outputType)||$outputType=="") $outputType=0;

    if (!isset($atkorderby)||$atkorderby=="") $atkorderby = "week";

    $view_all = $g_securityManager->allowed('reports.weekreport','view_all');

    $startdate = $this->m_postvars["startdate"];
    $enddate = $this->m_postvars["enddate"];

    $col = $this->m_postvars["col"];

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

    if (!isset($showstatus)||$showstatus=="") $showstatus = "both";

    if ($outputType==0)
    {
      $g_layout->initGUI();
      $g_layout->ui_top(text("title_employee_weekreport"));

      $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');

      $g_layout->output('<input type="hidden" name="atknodetype" value="reports.weekreport">');
      $g_layout->output('<input type="hidden" name="atkaction" value="'.$this->m_action.'">');
      $g_layout->table_simple();
      $g_layout->output('<tr>');
      $g_layout->td('<b>'.text('sethoursfilter').'</b>', 'colspan="2"');
      $g_layout->output('</tr><tr>');

      // we have to pass a 'dummy' record to the attributes to set their default value.
      $dummyrec = Array("startdate"=>array("year"=>substr($startdate,0,4),
                                           "month"=>substr($startdate,5,2),
                                           "day"=>substr($startdate,8,2)),
                        "enddate"=>array("year"=>substr($enddate,0,4),
                                         "month"=>substr($enddate,5,2),
                                        "day"=>substr($enddate,8,2)));

      //$g_layout->td(text("name").':</b> ');
      $g_layout->td('<SELECT name="nameswitch"><OPTION VALUE="name">'.text("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.text("supervisor").': </SELECT>');

      if($view_all)
      {
        $g_layout->td('<SELECT name="userid">'.$this->get_employees($userid).'</SELECT>');
      }
      else
      {
        $userid = $g_user["id"];
        $g_layout->td($g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">');
      }

      /*// TODO: Even if you don't have view_all right, you should still be able to view employees
      // that you supervise.
      if($view_all)
      {
        $g_layout->output('</tr><tr>');
        $g_layout->td(text("supervisor").':</b> ');
        $g_layout->td('<SELECT name="supervisor">'.$this->get_supervisors($supervisor).'</SELECT>');
      }    */

      $g_layout->output('</tr><tr>');

      $g_layout->td(text("timespan").': ');

      $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
      $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));

      $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                      text("until").
                      '&nbsp; '.$enddateatt->edit($dummyrec));

      $g_layout->output('</tr><tr>');

      $g_layout->td(text("status"));
      $g_layout->td('<select name="showstatus">
                     <option VALUE="both" '.($showstatus=="both"?"selected":"").'>'.text("both").'
                     <option VALUE="locked" '.($showstatus=="locked"?"selected":"").'>'.text("locked").'
                     <option VALUE="unlocked" '.($showstatus=="unlocked"?"selected":"").'>'.text("unlocked").'
                     </select>');
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
      $g_layout->td(text("report_show_details"));
      if ($showdetails==1) $checked = "checked"; else $checked = "";
      $g_layout->td('<input type="checkbox" name="showdetails" value="1" '.$checked.'>');

      $g_layout->output('</tr><tr>');

      $g_layout->td(text("orderby").': ');

      $g_layout->td('<select name="atkorderby">
                     <option VALUE="week" '.($atkorderby=="week"?"selected":"").'>'.text("week").'
                     <option VALUE="status" '.($atkorderby=="status"?"selected":"").'>'.text("status").'
                     <option VALUE="userid" '.($atkorderby=="userid"?"selected":"").'>'.text("userid").'
                     </select>');



      $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');

    }

    $weekdata = array();

    // first fill the weeks and start/end dates..
    $startstamp = mktime(0,0,0,substr($startdate,5,2),
                               substr($startdate,8,2),
                               substr($startdate,0,4));
    $endstamp = mktime(0,0,0,substr($enddate,5,2),
                             substr($enddate,8,2),
                             substr($enddate,0,4));

    // overzicht moet beginnen op de eerste dag van de week van de gekozen datum.
    $realweekstart = startOfWeek($startdate);
    $realstartstamp = mktime(0,0,0,substr($realweekstart,5,2),
                                   substr($realweekstart,8,2),
                                   substr($realweekstart,0,4));

    $nrofdays = ceil(($endstamp-$realstartstamp)/(24*60*60));

    for ($i=0;$i<=$nrofdays;$i++)
    {
      $curstamp = $realstartstamp + ((24*60*60)*$i);
      if (strftime("%w",$curstamp)==1)
      {
        $key = weekstamp($curstamp);
        $weekdata[$key]["week"] = $key;
        $weekdata[$key]["startdate"] = strftime("%d-%m-%Y",$curstamp);
        $weekdata[$key]["enddate"] = strftime("%d-%m-%Y",$curstamp+(6*24*60*60));
        $i+=6;
      }
    }

    $report_start_week = weekstamp($startstamp);
    $report_end_week = weekstamp($endstamp);

    // determine contracthours per week..
    $query = "SELECT
                uc_hours, startdate, enddate, person.userid, lastname, firstname
              FROM
                 usercontract, person
              WHERE ";
    if ($nameswitch=="supervisor")
    {
      if ($userid!="all") $query.= " person.supervisor = ".$userid." AND ";
    }
    else
    {
      if ($userid!="all") $query.= " usercontract.userid =".$userid." AND ";
    }
    $query.= "  startdate <= '$enddate'
                AND enddate >= '$startdate'
                AND usercontract.userid = person.id
              ORDER BY
                startdate";
    $contracts = $g_db->getrows($query);

    for ($i=0;$i<count($contracts);$i++)
    {
      $start_week = weekstamp(mktime(0,0,0,substr($contracts[$i]["startdate"],5,2),
                                             substr($contracts[$i]["startdate"],8,2),
                                             substr($contracts[$i]["startdate"],0,4)));
      $end_week = weekstamp(mktime(0,0,0,substr($contracts[$i]["enddate"],5,2),
                                           substr($contracts[$i]["enddate"],8,2),
                                           substr($contracts[$i]["enddate"],0,4)));
      for ($j=$start_week;$j<=$end_week;$j++)
      {
        if ($j>=$report_start_week && $j<=$report_end_week && substr($j,4)<=52)
        {
          $weekdata[$j]["week"] = $j;
          $weekdata[$j]["userdata"][$contracts[$i]["userid"]]["contract"] = $contracts[$i]["uc_hours"]*60;
          $weekdata[$j]["userdata"][$contracts[$i]["userid"]]["name"] = $contracts[$i]["lastname"].", ".$contracts[$i]["firstname"];
        }
      }
    }

    // collect data..
    $query = "SELECT
                sum(time) as time, activitydate, hours.userid, person.lastname, person.firstname
              FROM
                hours, person
              WHERE ";
    if ($nameswitch=="supervisor")
    {
      if ($userid!="all") $query.= " person.supervisor = '$userid' AND ";
    }
    else
    {
      if ($userid!="all") $query.= " hours.userid ='$userid' AND ";
    }
    $query.= "  activitydate >= '$startdate'
                AND activitydate <= '$enddate'
                AND hours.userid = person.id
              GROUP BY
                hours.userid, activitydate";

    $raw = $g_db->getrows($query);

    $strdow = array("sun","mon","tue","wed","thu","fri","sat");

    for ($i=0;$i<count($raw);$i++)
    {
      $stamp = mktime(0,0,0,substr($raw[$i]["activitydate"],5,2),
                            substr($raw[$i]["activitydate"],8,2),
                            substr($raw[$i]["activitydate"],0,4));
      $weeknr = strftime("%V",$stamp);
      $dow = strftime("%w",$stamp);
      $key = weekstamp($stamp);
      $weekdata[$key]["week"] = $key;
      $uid = $raw[$i]["userid"];
      $weekdata[$key]["userdata"][$uid][$strdow[$dow]] = $raw[$i]["time"];
      $weekdata[$key]["userdata"][$uid]["total"] = $weekdata[$key]["userdata"][$uid]["total"]+$raw[$i]["time"];
      $weekdata[$key]["userdata"][$uid]["name"] = $raw[$i]["lastname"].", ".$raw[$i]["firstname"];

      // We need to display the first date and last date of each week.
      // Since we are looping days now, it is easiest if we calculate the
      // beginning of a week here.
    }

    if ($nameswitch=="supervisor" && $userid!='all')
    {
      $query = "SELECT
                  week, hours_lock.userid
                FROM
                  hours_lock, person
                WHERE
                  ((hours_lock.userid = person.id AND person.supervisor='$userid')
                  OR hours_lock.userid IS NULL OR hours_lock.userid='')
                  AND
                    week between '$report_start_week' and '$report_end_week' ";
    }
    else
    {
      // Collect locks..
      $query = "SELECT
                  week, userid
                FROM
                  hours_lock
                WHERE
                  week between '$report_start_week' and '$report_end_week' ";
      if ($nameswitch=="name"&&$userid!="all")
      {
        $query.="AND (userid = '$userid' OR userid IS NULL OR userid = '')";
      }
    }
    $locks = $g_db->getrows($query);
    for ($i=0;$i<count($locks);$i++)
    {
      if ($locks[$i]["userid"]=="")
      {
        $weekdata[$locks[$i]["week"]]["status"]="locked";
      }
      else
      {
        $weekdata[$locks[$i]["week"]]["userdata"][$locks[$i]["userid"]]["status"]="locked";
      }
    }

    // weekdata is an associative array, indexed by weeknumber.
    // it must be converted to a normal array in order to be usable
    // by recordlist functions.
    // we also need to calculate some numbers.
    $rldata = array();
    foreach ($weekdata as $weeknr => $data)
    {
      $record = array();
      $record["startdate"] = $data["startdate"];
      $record["enddate"] = $data["enddate"];
      $record["week"] = $data["week"];
      $record["status"] = $data["status"];
      if (is_array($data["userdata"]))
      {
        foreach($data["userdata"] as $user => $userdata)
        {
          $record["userid"] = $userdata["name"];
          $record["total"] = $userdata["total"];
          $record["contract"] = $userdata["contract"];
          $record["overtime"] = max(0,($userdata["total"]-$userdata["contract"]));
          if ($userdata["status"]!="")
          {
            $record["status"] = $userdata["status"];
          }
          else
          {
            $record["status"] = $data["status"];
          }
          $cmpstatus = (($record["status"]=="locked")?"locked":"unlocked");
          if ($showstatus=="both" || $cmpstatus==$showstatus)
          {
            for ($i=0;$i<count($strdow);$i++)
            {
              $record[$strdow[$i]] = $userdata[$strdow[$i]];
            }
            $rldata[] = $record;
          }
        }
      }
    }

    // sorting..
    if ($atkorderby=="") $atkorderby = "week";
    usort($rldata,$atkorderby."_cmp");

    if ($showdetails==1)
    {
      $suppress = array(); // show all columns
    }
    else
    {
      $suppress = $strdow; // suppress day details
      $suppress[] = "startdate";
      $suppress[] = "enddate";
    }
    if ($nameswitch=="name" && $userid!="all") $suppress[] = "userid";

    if ($outputType=="0")
    {
      $g_layout->ui_bottom();
      $g_layout->ui_top(text("title_weekreport")." ".text("report_intimespan")." ".$startdate." t/m ".$enddate);

      if ($userid!="all")
      {
        $emprec = $this->get_employee($userid);      ;

        $employeedata = '<br>'.$g_layout->ret_table_simple().'<tr>';

        if ($nameswitch=="supervisor")
        {
          $label = text("supervisor");
        }
        else
        {
          $label = text("name");
        }
        $employeedata.= $g_layout->ret_td($label.": ");
        $employeedata.= $g_layout->ret_td($emprec["lastname"].", ".$emprec["firstname"]);
        $employeedata.= '</tr><tr>';
        $employeedata.= $g_layout->ret_td(text("email").": ");
        $employeedata.= $g_layout->ret_td($emprec["email"]);
        $employeedata.= '</tr></table>';

        $g_layout->output($employeedata);
        $g_layout->output('<br>');
      }


      $g_layout->output('<br>'.$this->recordList($rldata, array(),false,$suppress));
      $g_layout->output('<br><br>');
      $g_layout->ui_bottom();
    }
    else if ($outputType=="1")
    {
      $g_layout->output($this->specialRecordList($rldata,"<tr>","<td>","</td>","</tr>", "0","",$suppress));
    }
    else if ($outputType=="2")
    {
      // specail RecordList can export to file
      $this->specialRecordList($rldata, "", '"', '";', "\r\n", "1", "",$suppress);
    }
  }
}

?>
