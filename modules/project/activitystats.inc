<?php
  // This file is included by class.activity.inc

  include_once("achievotools.inc");
  useattrib("atkdateattribute");

  global $g_securityManager;

  $ui = &$this->getUi();
  $page = &$this->getPage();
  $this->addStyle("style.css");
  $activityid = $_REQUEST["activityid"];
  $startdate = $_REQUEST["startdate"];
  $enddate = $_REQUEST["enddate"];
  $activityname = "";

  // Formar start and end date.
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

  /**
   * Get Activities
   *
   * @param int $act_id Activity id
   * @param string $activityname Activity name
   * @return string Options of all activities
   */
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
  /**
   * Get searchbar for activities
   *
   * @param int $activityid Activity ID
   * @param string $activityname Activity name
   * @param array $startdate Start date from request vars
   * @param array $enddate End date from request vars
   * @param string $action Current action
   */
  function getSearchBar($activityid,&$activityname,$startdate,$enddate,$action)
  {
    $ui = &atkinstance("atk.ui.atkui");
    $page = &atkinstance("atk.ui.atkpage");

    $output='<form action="dispatch.php" method="get">';
    $output.='<input type="hidden" name="atknodetype" value="project.activity">';
    $output.='<input type="hidden" name="atkaction" value="'.$action.'">';

    $data = array();
    $data[]=array('<b>'.atktext('sethoursfilter',"project").'</b>');

      // we have to pass a 'dummy' record to the attributes to set their default value.
    $dummyrec = Array("startdate"=>array("year"=>substr($startdate,0,4),
                                         "month"=>substr($startdate,5,2),
                                         "day"=>substr($startdate,8,2)),
                      "enddate"=>array("year"=>substr($enddate,0,4),
                                       "month"=>substr($enddate,5,2),
                                       "day"=>substr($enddate,8,2)));

    $data[] = array(atktext("activity","project"),'<SELECT name="activityid">'.get_activities($activityid, $activityname).'</SELECT>');

    $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
    $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));

    $data[] = array(atktext("timespan","project").': ',$startdateatt->edit($dummyrec).' &nbsp;'.
                    atktext("until","project").
                    '&nbsp; '.$enddateatt->edit($dummyrec));

    $tbl = &atknew("atk.utils.atktablerenderer");
    //$tbl->setColSpan(0,0,2);
    $output.=$tbl->render($data);
    $output.='<input type="submit" value="'.atktext("refresh","project").'"></form><br>';
    $searchbar = $ui->renderBox(array("title"=>atktext("title_activitystats","project"),"content"=>$output));
    $page->addContent($searchbar);
  }


  /**
   * Get statistics for the choosen activity
   *
   * @param int $activityid Activity ID
   * @param string $activityname Activity name
   * @param array $startdate Start date from request vars
   * @param array $enddate End date from request vars
   */
  function getStats($activityid,$activityname,$startdate,$enddate)
  {

    $ui = &atkinstance("atk.ui.atkui");
    $page = &atkinstance("atk.ui.atkpage");

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
    $db = &atkGetDb();
    $records = $db->getrows($query);

    $hoursperemp = Array();
    $hoursperproj = Array();
    $total = 0;
    $output="";

    for ($i=0;$i<count($records);$i++)
    {
      $employee = $records[$i]["employee_lastname"].", ".$records[$i]["employee_firstname"];
      $hoursperemp[$employee] += $records[$i]["time"];
      $hoursperproj[$records[$i]["project"]] += $records[$i]["time"];
      $total += $records[$i]["time"];
    }


    $output.='<br><b>'.atktext("title_activitystats_peremployee","project").'</b><br><br>';
    $data = array();
    $data[] = array(atktext("name","project"),atktext("time","project"),atktext("percentage","project"));

    while (list($empname, $time) = each ($hoursperemp))
    {
      $percentage = round($time/$total*100);
      $imgwidth = max(1,($percentage*3));

      $data[] = array($empname,time_format($time),'<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    }

    $data[] = array(atktext("total","project"),time_format($total),'&nbsp;');
    $tbl = &atknew("atk.utils.atktablerenderer");
    $output.=$tbl->render($data,TBL_HEADER|TBL_FOOTER,"recordlist");

    $output.='<br><br><b>'.atktext("title_activitystats_perproject","project").'</b><br><br>';

    $data = array();
    $data[] = array(atktext("name","project"),atktext("time","project"),atktext("percentage","project"));

    while (list($projname, $time) = each ($hoursperproj))
    {
      $percentage = round($time/$total*100);
      $imgwidth = max(1,($percentage*3));
      $data[] = array($projname,time_format($time),'<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20">&nbsp; ('.$percentage.'%)');
    }

    $data[] = array(atktext("total","project"),time_format($total),'&nbsp;');

    $output.=$tbl->render($data,TBL_HEADER|TBL_FOOTER,"recordlist");

    $output.="<br><br>";
    $stats = $ui->renderBox(array("title"=>atktext("title_activitystats","project")." - ".$activityname,"content"=>$output));
    $page->addContent($stats);
  }

  // Main program
  getSearchBar($activityid,$activityname,$startdate,$enddate,$this->m_action);
  if($activityid) getStats($activityid,$activityname,$startdate,$enddate);
?>