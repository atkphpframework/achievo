<?php
  function employee_time_format($time)
  {
    return floor($time/60).":".sprintf("%02d",($time%60));
  }

  global $g_layout, $g_securityManager, $g_user, $g_db, $config_currency_symbol;
  
  $startdate = $_REQUEST["startdate"];
  $enddate = $_REQUEST["enddate"];
  $userid = $_REQUEST["userid"];
  

  useattrib("atkdateattribute");

  function get_employees($user_id)
  {
    global $g_db;

    $sql = "SELECT id,lastname,userid,firstname
            FROM person
            WHERE status='active' AND role='employee'
            ORDER BY lastname
           ";

    $records = $g_db->getrows($sql);
    $employee_code='<OPTION VALUE="all">'.text("allusers");
    for($i=0;$i<count($records);$i++)
    {
      if($user_id==$records[$i]["id"]) { $sel="SELECTED"; } else { $sel=""; }
      $employee_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
    }
    return $employee_code;

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

  $view_all = $g_securityManager->allowed('employee.employee','view_all');

  $g_layout->initGUI();
  $g_layout->ui_top(text("parameters", 'employee', 'employee'));

  $g_layout->output('<form action="dispatch.php" method="get">');
  $g_layout->output('<input type="hidden" name="atknodetype" value="employee.employee">');
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

  $g_layout->td(text("name").':</b> ');

  if($view_all)
  {
    $g_layout->td('<SELECT name="userid">'.get_employees($userid).'</SELECT>');
  }
  else
  {
    $g_layout->td($g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">');
  }
  $g_layout->output('</tr><tr>');

  $g_layout->td(text("timespan").': ');

  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));

  $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                  text("until").
                  '&nbsp; '.$enddateatt->edit($dummyrec));

  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');

  $g_layout->ui_bottom();
  
  if ($userid!='') // only display stats if we have a valid id.
  {

    $query = "SELECT
                hours.time as time,
                project.name as project,
                activity.name as activity
              FROM
                hours, activity, phase, project
              WHERE
                hours.activitydate >= '$startdate'
                AND hours.activitydate <= '$enddate'
                AND hours.activityid = activity.id
                AND hours.phaseid = phase.id
                AND phase.projectid = project.id";
    if ($userid!="all") $query.= " AND hours.userid = '$userid'";
  
    $records = $g_db->getrows($query);
  
    $hoursperact = Array();
    $hoursperproj = Array();
    $total = 0;
  
    for ($i=0;$i<count($records);$i++)
    {
      $hoursperact[$records[$i]["activity"]] += $records[$i]["time"];
      $hoursperproj[$records[$i]["project"]] += $records[$i]["time"];
      $total += $records[$i]["time"];
    }
  
    $g_layout->ui_top($g_layout->title('employee', 'employee', 'stats')." - ".$userid);
  
    $g_layout->output('<br><b>'.text("title_employeestats_perproject").'</b><br><br>');
  
    $g_layout->output($g_layout->data_top());
  
    $g_layout->output($g_layout->tr_top());
    $g_layout->td_datatitle(text("name"));
    $g_layout->td_datatitle(text("time"));
    $g_layout->td_datatitle(text("percentage"));
    $g_layout->output($g_layout->tr_bottom());
  
    while (list($projname, $time) = each ($hoursperproj))
    {
      $g_layout->output($g_layout->tr_top());
      $g_layout->td($projname);
      $g_layout->td(employee_time_format($time));
      $percentage = round(($time/$total)*100);
      $imgwidth = max(1,($percentage*3));
      $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
      $g_layout->output($g_layout->tr_bottom());
    }
  
    $g_layout->output($g_layout->tr_top());
    $g_layout->td_datatitle(text("total"));
    $g_layout->td_datatitle(employee_time_format($total));
    $g_layout->td_datatitle('&nbsp;');
    $g_layout->output($g_layout->tr_bottom());
  
    $g_layout->output($g_layout->data_bottom());
  
    $g_layout->output('<br><b>'.text("title_employeestats_peractivity").'</b><br><br>');
  
    $g_layout->output($g_layout->data_top());
  
    $g_layout->output($g_layout->tr_top());
    $g_layout->td_datatitle(text("name"));
    $g_layout->td_datatitle(text("time"));
    $g_layout->td_datatitle(text("percentage"));
    $g_layout->output($g_layout->tr_bottom());
  
    while (list($actname, $time) = each ($hoursperact))
    {
      $g_layout->output($g_layout->tr_top());
      $g_layout->td($actname);
      $g_layout->td(employee_time_format($time));
      $percentage = round(($time/$total)*100);
      $imgwidth = max(1,($percentage*3));
      $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
      $g_layout->output($g_layout->tr_bottom());
    }
  
    $g_layout->output($g_layout->tr_top());
    $g_layout->td_datatitle(text("total"));
    $g_layout->td_datatitle(employee_time_format($total));
    $g_layout->td_datatitle('&nbsp;');
    $g_layout->output($g_layout->tr_bottom());
  
    $g_layout->output($g_layout->data_bottom());
    
    $g_layout->output('<br><br><br>');
  
    $g_layout->ui_bottom();

  }
?>
