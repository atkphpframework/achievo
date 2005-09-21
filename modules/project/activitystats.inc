<?php

  include_once("achievotools.inc");
  useattrib("atkdateattribute");

  global $g_securityManager;
  $startdate = $_REQUEST["startdate"];
  $enddate = $_REQUEST["enddate"];
  $activityid = $_REQUEST["activityid"];  
  $activityname = $_REQUEST["activityname"];

  function get_activities($act_id, &$activityname)
  {
    $db = &atkGetDb();
    // Get the activities
    $sql = "SELECT id,name
            FROM activity
            ORDER BY name
           ";
    $records = $db->getrows($sql);
    
    for($i=0;$i<count($records);$i++)
    {
      // first activity is default if no activity is selected.
      //
      if($act_id==$records[$i]["id"])
      {
        $sel="SELECTED";
        // since we are getting activityrecords anyway, we also remember the name of the currently
        // selected activity.        
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
  
  atkimport("atk.layout");
  $layout = &layout::getInstance();

  $layout->initGUI();
  $layout->ui_top(text("title_hours_stats"));

  $layout->output('<form action="dispatch.php" method="get">');
  $layout->output('<input type="hidden" name="atknodetype" value="project.activity">');
  $layout->output('<input type="hidden" name="atkaction" value="'.$this->m_action.'">');
  $layout->table_simple();
  $layout->output('<tr>');
  $layout->td('<b>'.text('sethoursfilter').'</b>', 'colspan="2"');
  $layout->output('</tr><tr>');

    // we have to pass a 'dummy' record to the attributes to set their default value.
  $dummyrec = Array("startdate"=>array("year"=>substr($startdate,0,4),
                                       "month"=>substr($startdate,5,2),
                                       "day"=>substr($startdate,8,2)),
                    "enddate"=>array("year"=>substr($enddate,0,4),
                                     "month"=>substr($enddate,5,2),
                                     "day"=>substr($enddate,8,2)));

  $layout->td(text("activity").':</b> ');

  $layout->td('<SELECT name="activityid">'.get_activities($activityid, $activityname).'</SELECT>');

  $layout->output('</tr><tr>');

  $layout->td(text("timespan").': ');

  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));

  $layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                  text("until").
                  '&nbsp; '.$enddateatt->edit($dummyrec));

  $layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');

  $layout->ui_bottom();

  if ($activityid) // Show only stats if we have a valid activityid.
  {

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
                AND hours.userid = person.id
                AND hours.phaseid = phase.id
                AND phase.projectid = project.id
                AND hours.activityid = $activityid";
  
    $records = $db->getrows($query);
  
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
  
    $layout->ui_top(text("title_activitystats")." - ".$activityname);
  
    $layout->output('<br><b>'.text("title_activitystats_peremployee").'</b><br><br>');
  
    $layout->output($layout->data_top());
  
    $layout->output($layout->tr_top());
    $layout->td_datatitle(text("name"));
    $layout->td_datatitle(text("time"));
    $layout->td_datatitle(text("percentage"));
    $layout->output($layout->tr_bottom());
  
    while (list($empname, $time) = each ($hoursperemp))
    {
      $layout->output($layout->tr_top());
      $layout->td($empname);
      $layout->td(time_format($time));
      $percentage = round($time/$total*100);
      $imgwidth = max(1,($percentage*3));
  
      $layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
      $layout->output($layout->tr_bottom());
    }
  
    $layout->output($layout->tr_top());
    $layout->td_datatitle(text("total"));
    $layout->td_datatitle(time_format($total));
    $layout->td_datatitle('&nbsp;');
    $layout->output($layout->tr_bottom());
  
    $layout->output($layout->data_bottom());
  
    $layout->output('<br><b>'.text("title_activitystats_perproject").'</b><br><br>');
  
    $layout->output($layout->data_top());
  
    $layout->output($layout->tr_top());
    $layout->td_datatitle(text("name"));
    $layout->td_datatitle(text("time"));
    $layout->td_datatitle(text("percentage"));
    $layout->output($layout->tr_bottom());
  
    while (list($projname, $time) = each ($hoursperproj))
    {
      $layout->output($layout->tr_top());
      $layout->td($projname);
      $layout->td(time_format($time));
      $percentage = round($time/$total*100);
      $imgwidth = max(1,($percentage*3));
      $layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20">&nbsp; ('.$percentage.'%)');
      $layout->output($layout->tr_bottom());
    }
  
    $layout->output($layout->tr_top());
    $layout->td_datatitle(text("total"));
    $layout->td_datatitle(time_format($total));
    $layout->td_datatitle('&nbsp;');
    $layout->output($layout->tr_bottom());
  
    $layout->output($layout->data_bottom());
    $layout->output('<br><br><br>');
  
    $layout->ui_bottom();
  }
?>