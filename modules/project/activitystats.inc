<?php

  include_once("achievotools.inc");
  useattrib("atkdateattribute");

  global $g_layout;
  global $g_securityManager;
  global $startdate;
  global $enddate;
  global $activityid;
  global $g_db;
  global $activityname;

  function get_activities($act_id)
  {
    global $g_db, $activityname, $activityid;
    // Get the activities
    $sql = "SELECT id,name
            FROM activity
            ORDER BY name
           ";
    $records = $g_db->getrows($sql);
    if($act_id==-1) { $sel="SELECTED"; } else { $sel=""; }
    for($i=0;$i<count($records);$i++)
    {
      // first activity is default if no activity is selected.
      //
      if($act_id==$records[$i]["id"] || ($act_id=="" && $i==0))
      {
        $sel="SELECTED";
        // since we are getting activityrecords anyway, we also remember the name of the currently
        // selected activity.
        $activityid = $records[$i]["id"];
        $activityname = $records[$i]["name"];
      }
      else
      {
        $sel="";
      }
      $activity_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$sel.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $activity_code;

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

  $g_layout->ui_top(text("title_hours_stats"));

  $g_layout->output('<form action="dispatch.php" method="get">');
  $g_layout->output('<input type="hidden" name="atknodetype" value="project.activity">');
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

  $g_layout->td(text("activity").':</b> ');

  $g_layout->td('<SELECT name="activityid">'.get_activities($activityid).'</SELECT>');

  $g_layout->output('</tr><tr>');

  $g_layout->td(text("timespan").': ');

  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));

  $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                  text("until").
                  '&nbsp; '.$enddateatt->edit($dummyrec));

  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');

  $g_layout->ui_bottom();


  $query = "SELECT
              hours.time as time,
              project.name as project,
              person.lastname as employee_lastname,
              person.firstname as employee_firstname
            FROM
              hours, person, phase, project
            WHERE
              hours.activitydate >= '$startdate'
              AND hours.activitydate <= '$enddate'
              AND hours.userid = person.userid
              AND hours.phaseid = phase.id
              AND phase.projectid = project.id
              AND hours.activityid = $activityid";

  $records = $g_db->getrows($query);

  $hoursperemp = Array();
  $hoursperproj = Array();
  $total = 0;

  for ($i=0;$i<count($records);$i++)
  {
    $employee = $records[$i]["employee_lastname"].", ".$records[$i]["employee_firstname"];
    $hoursperemp[$employee] += $records[$i]["time"];
    $hoursperproj[$records[$i]["project"]] += $records[$i]["time"];
    $total += $records[$i]["time"];
  }

  $g_layout->ui_top(text("title_activitystats")." - ".$activityname);

  $g_layout->output('<br><b>'.text("title_activitystats_peremployee").'</b><br><br>');

  $g_layout->output($g_layout->data_top());

  $g_layout->output($g_layout->tr_top());
  $g_layout->td_datatitle(text("name"));
  $g_layout->td_datatitle(text("time"));
  $g_layout->td_datatitle(text("percentage"));
  $g_layout->output($g_layout->tr_bottom());

  while (list($empname, $time) = each ($hoursperemp))
  {
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($empname);
    $g_layout->td(time_format($time));
    $percentage = round($time/$total*100);
    $imgwidth = max(1,($percentage*3));

    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }

  $g_layout->output($g_layout->tr_top());
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(time_format($total));
  $g_layout->td_datatitle('&nbsp;');
  $g_layout->output($g_layout->tr_bottom());

  $g_layout->output($g_layout->data_bottom());

  $g_layout->output('<br><b>'.text("title_activitystats_perproject").'</b><br><br>');

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
    $g_layout->td(time_format($time));
    $percentage = round($time/$total*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20">&nbsp; ('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }

  $g_layout->output($g_layout->tr_top());
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(time_format($total));
  $g_layout->td_datatitle('&nbsp;');
  $g_layout->output($g_layout->tr_bottom());

  $g_layout->output($g_layout->data_bottom());
  $g_layout->output('<br><br><br>');

  $g_layout->ui_bottom();
?>
