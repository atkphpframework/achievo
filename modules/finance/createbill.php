<?php

global $g_layout, $g_db, $activityid, $startdate, $enddate, $g_user, $userid, $bill_id, $reghours, $config_currency_symbol;

$g_layout->register_script("javascript/check.js");

 function get_activities($act_id)
  {
    global $g_db, $activityid;
    // Get the activities
    $sql = "SELECT id,name
            FROM activity
            ORDER BY name
           ";
    $records = $g_db->getrows($sql);
   	if($act_id==-1) { $sel="SELECTED"; } else { $sel=""; }
    $activity_code='<OPTION VALUE="all" SELECTED>'.text("allactivities");
    for($i=0;$i<count($records);$i++)
    {
      $activity_code.='<OPTION VALUE="'.$records[$i]["id"].'"'.$sel.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $activity_code;
  }

 function get_employees($user_id)
  {
    global $g_db;

    $sql = "SELECT lastname,userid,firstname
            FROM person
            WHERE status='active' AND role='employee'
            ORDER BY lastname
           ";

    $records = $g_db->getrows($sql);
    $employee_code='<OPTION VALUE="all">'.text("allusers");
    for($i=0;$i<count($records);$i++)
    {
      if($user_id==$records[$i]["userid"]) { $sel="SELECTED"; } else {$sel=""; }
	$employee_code.='<OPTION VALUE="'.$records[$i]["userid"].'"'.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
    }
    return $employee_code;
  }

  function time_format($time)
  {
    if ($time==0) return "&nbsp;";
    return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
  }

  //Query to get projectid
  if ($bill_id == "") {
    $sql = "SELECT projectid FROM bill where id = " .$rec["billid"]["id"];
    $activityid = "all";
    $userid = "all";
    $calcoption = $rec["calcoption"];
  }
  else {
    $sql = "SELECT projectid FROM bill where id = " .$bill_id;
  }

  $projectrec=$g_db->getrows($sql);

  $g_layout->ui_top(text("title_hourspecify"));
  $g_layout->output('<form action="dispatch.php" method="get">');
  $g_layout->output(session_form());
  $g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill_line">');
  $g_layout->output('<input type="hidden" name="atkaction" value="createbill">');
  $g_layout->output('<input type="hidden" name="bill_line_id" value="'.$bill_line_id.'">');

  if ($bill_id == "") {
    $g_layout->output('<input type="hidden" name="bill_id" value='.$rec["billid"]["id"].'>');
    $g_layout->output('<input type="hidden" name="calcoption" value='.$rec["calcoption"].'>');
  }
  else {
    $g_layout->output('<input type="hidden" name="bill_id" value='.$bill_id.'>');
    $g_layout->output('<input type="hidden" name="calcoption" value='.$calcoption.'>');
  }
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
  $g_layout->td(text("name").':</br> ');
  $g_layout->td('<SELECT name="userid">'.get_employees($userid).'</SELECT>');
  $g_layout->output('</tr><tr>');

  $g_layout->td(text("timespan").': ');
  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));

  $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.text("until").'&nbsp; '.$enddateatt->edit($dummyrec));
  $g_layout->output('</tr><tr>');
  $g_layout->td('<INPUT TYPE="checkbox" NAME="reghours" VALUE="yes">'.'</br>');
  $g_layout->td(text("bill_reghours").'</br> ');
  $g_layout->output('</tr><tr>');

  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');
  $g_layout->ui_bottom();

  $start_date = $startdate["year"]."-".$startdate["month"]."-".$startdate["day"];
  $end_date = $enddate["year"]."-".$enddate["month"]."-".$enddate["day"];

  //Query to get all the hours with there rates

  if ($reghours == "yes") {
   $sql = "SELECT
            hours.id AS hoursid,
	    hours.remark,
	    hours.userid AS user,
	    hours.entrydate,
	    hours.time,
	    rate.*,
 	    activity.name AS activityname
	  FROM
   	    hours,
	    rate,
	    phase,
	    project
          LEFT JOIN customer ON project.customer = customer.id
	  LEFT JOIN activity ON hours.activityid = activity.id
	  WHERE
   	    hours.phaseid = phase.id
   	    AND project.id = phase.projectid
	    AND phase.projectid = " .$projectrec["0"]["projectid"];
   $sql.= " AND hours.registered != 1";
   $sql.= " AND (rate.userid = hours.userid
              OR rate.activityid = hours.activityid
              OR rate.projectid = project.id
              OR rate.customerid = customer.id)";
    $sql.= " ORDER BY hours.id, rate.priority DESC, rate.rate DESC";
  }
  else {
    $sql = "SELECT
            hours.id AS hoursid,
	    hours.remark,
	    hours.userid AS user,
	    hours.entrydate,
	    hours.time,
	    rate.*,
 	    activity.name AS activityname
	  FROM
   	    hours,
	    rate,
	    phase,
	    project
          LEFT JOIN customer ON project.customer = customer.id
	  LEFT JOIN activity ON hours.activityid = activity.id
	  WHERE
   	    hours.phaseid = phase.id
   	    AND project.id = phase.projectid
	    AND phase.projectid = " .$projectrec["0"]["projectid"];
    $sql.= " AND hours.activitydate >= '$start_date'
	    AND hours.activitydate <= '$end_date'";
   $sql.= " AND hours.registered != 1";
   $sql.= " AND (rate.userid = hours.userid
              OR rate.activityid = hours.activityid
              OR rate.projectid = project.id
              OR rate.customerid = customer.id)";
    if ($userid!="all") $sql.= " AND hours.userid = '$userid'";
    if ($activityid!="all") $sql.= " AND hours.activityid = '$activityid'";
    $sql.= " ORDER BY hours.id, rate.priority DESC, rate.rate DESC";
  }

$hourrec=$g_db->getrows($sql);

$g_layout->ui_top(text("title_selecthours"));

$g_layout->output('<br><b></b><br>');
$g_layout->output($g_layout->data_top());

$g_layout->output($g_layout->tr_top());
$g_layout->td_datatitle(text("hours_date"));
$g_layout->td_datatitle(text("hours_owner"));
$g_layout->td_datatitle(text("hours_activity"));
$g_layout->td_datatitle(text("hours_remark"));
$g_layout->td_datatitle(text("hours_time"));
$g_layout->td_datatitle(text("hours_rate"));
$g_layout->td_datatitle(text("header_register"));
if ($calcoption == 'calc') $g_layout->td_datatitle("Put on Bill");
$g_layout->td("");
$g_layout->output($g_layout->tr_bottom());

$g_layout->output('<FORM action="dispatch.php" name="billform" method="post">');
$g_layout->output(session_form());
$g_layout->output('<input type="hidden" name="atknodetype" value="finance.bill_line">');
$g_layout->output('<input type="hidden" name="atkaction" value="addtobill">');
$g_layout->output('<input type="hidden" name="bill_id" value='.$bill_id.'>');
$g_layout->output('<input type="hidden" name="bill_line_id" value='.$bill_line_id.'>');
$g_layout->output('<input type="hidden" name="calcoption" value='.$calcoption.'>');

$x=1;
for ($i=0;$i<count($hourrec);$i++)
{
  if ($hourrec[$i]["hoursid"] == $hourrec[$i-1]["hoursid"])
  {
  }
  else
  {
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($hourrec[$i]["entrydate"]);
    $g_layout->td($hourrec[$i]["user"]);
    $g_layout->td($hourrec[$i]["activityname"]);
    $g_layout->td($hourrec[$i]["remark"]);
    $g_layout->td(time_format($hourrec[$i]["time"]));
    $g_layout->td($config_currency_symbol . " " . round($hourrec[$i]["rate"],2));
    $g_layout->td("<CENTER>".'<input type="checkbox" name="checkA'.$x.'" value="'.$hourrec[$i]["hoursid"].'" onClick="checkA(this)">'."<CENTER>");
    if ($calcoption == 'calc') $g_layout->td("<CENTER>".'<input type="checkbox" name="checkB'.$x.'" value="'.$hourrec[$i]["hoursid"].'" onClick="checkB(this)">'."<CENTER>");
    $g_layout->output('<input type="hidden" name="rate'.$x.'" value="'.$hourrec[$i]["rate"].'">');
    $g_layout->td("");
    $g_layout->output($g_layout->tr_bottom());
    $x++;
  }
}

$g_layout->output('<input type="hidden" name="hourcount" value="'.$x.'">');

$g_layout->output($g_layout->tr_top());
if ($calcoption == 'calc')
{
 $y = 9;
}
else
{
 $y = 8;
}
for ($i=0;$i<$y;$i++)
{
  $g_layout->td("");
}
$g_layout->output($g_layout->tr_bottom());

$g_layout->output($g_layout->tr_top());
$g_layout->td("");
$g_layout->td("");
$g_layout->td("");
$g_layout->td("");
$g_layout->td("");
$g_layout->td("");

if ($calcoption == 'calc')
{
  $g_layout->td("<CENTER>".'<input type="checkbox" name="A" onClick="checkallA()"'."<CENTER>");
}
else
{
  $g_layout->td("<CENTER>".'<input type="checkbox" name="A" onClick="checkC()"'."<CENTER>");
  //$g_layout->td("<CENTER>".'<input type="checkbox" name="A" onClick="hallo()"'."<CENTER>");
}
if ($calcoption == 'calc') $g_layout->td("<CENTER>".'<input type="checkbox" name="B" onClick="checkallB()"'."<CENTER>");
$g_layout->td("(Un)Check All");
$g_layout->output($g_layout->tr_bottom());
$g_layout->output($g_layout->data_bottom());

$g_layout->table_simple();
$g_layout->output('<tr>');
$g_layout->td('<BR><input type="submit" value="'.text("addtobill").'">');
$g_layout->output('</tr>');
$g_layout->output("</FORM>");
$g_layout->output('</table>');

?>