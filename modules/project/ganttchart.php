<?php
 /**
   * Module Resource planning
   * Ganttchart resource planning
   *
   * This file generates the ganttchart. .
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   */

chdir("../..");
$config_atkroot ="./";
include_once($config_atkroot."atk.inc");

include_once(moduleDir("graph")."jpgraph/jpgraph.php");
include_once(moduleDir("graph")."jpgraph/jpgraph_gantt.php");

atksession();

$projectid = $_REQUEST['projectid'];

/*function load_dependencies($phase_record,$dep_record)
{
  for ($i=0;$i<count($dep_record);$i++)
  {
    $before[] = $dep_record[$i]['dependency_phaseid_row'];
    $after[] = $dep_record[$i]['dependency_phaseid_col'];
  }

  return array("befores"=>$before, "afters"=>$after);
}*/

function addDefaultConditions(&$query)
{
  $query->addCondition("phase.startdate IS NOT null");
  $query->addCondition("phase.enddate IS NOT null");
}


// Select the dependency's  for the gantchart
$db = &atkGetDb();
$querydep = &$db->createQuery();
$querydep->addTable('phase');
$querydep->addJoin('project', '' ,'project.id=phase.projectid', FALSE);
$querydep->addJoin('dependency', '' ,'dependency.phaseid_row=phase.id', FALSE);
$querydep->addField('phaseid_row', ' ', 'dependency', 'dependency_');
$querydep->addField('phaseid_col', ' ', 'dependency', 'dependency_');
$querydep->addCondition("project.id='".$projectid."'");
addDefaultConditions($querydep);
$querystringdep = $querydep->buildSelect(TRUE);
$dbrecordsdep = $db->getrows($querystringdep);

$cntrec=count($dbrecordsdep);
for ($i=0;$i<$cntrec;$i++)
{
 $querydep->deAlias($dbrecordsdep[$i]);
}

// Get the database configuration parameters
$dbconfig = atkconfig("db");

// Select the PHASES which belong to the project
$queryphase = &$db->createQuery();
$queryphase->addTable('project');
$queryphase->addJoin('phase', '', 'project.id=phase.projectid', FALSE);
$queryphase->addField('id', ' ', 'phase', 'phase_');
$queryphase->addField('name', ' ', 'project', 'project_');
$queryphase->addField('name', ' ', 'phase', 'phase_');
$queryphase->addField('current_planning', ' ', 'phase', 'phase_');
$queryphase->addField('startdate', ' ', 'project', 'project_');
$queryphase->addField('phase.startdate AS phase_startdate');
$queryphase->addField('phase.enddate AS phase_enddate');
$queryphase->addCondition("project.id='".$projectid."'");
addDefaultConditions($queryphase);
$querystringphase = $queryphase->buildSelect(TRUE);
$dbrecordsphase = $db->getrows($querystringphase);

$cntrec=count($dbrecordsphase);
for ($i=0;$i<$cntrec;$i++)
{
  $queryphase->deAlias($dbrecordsphase[$i]);
}

for($i=0;$i<count($dbrecordsphase);$i++)
{
  $phase_ids[$i] = $dbrecordsphase[$i]['phase_id'];
  $phase_names[$i] = $dbrecordsphase[$i]['phase_name'];
  $phase_maxtimes[$i] = ($dbrecordsphase[$i]['phase_current_planning']/60)/8; // convert minutes to days
}

//$dep = load_dependencies($phase_ids,$dbrecordsdep);
// Select the TIMES that have been booked on the project
$querybooked = &$db->createQuery();
$querybooked->addTable('phase');
$querybooked->addJoin('hours', '', 'hours.phaseid=phase.id', FALSE);
$querybooked->addField('id', ' ', 'phase', 'phase_');
$querybooked->addField('name', ' ', 'phase', 'phase_');
$querybooked->addField('SUM(hours.time) AS hours');
$querybooked->addField('current_planning', ' ', 'phase', 'phase_');
$querybooked->addCondition("phase.projectid='".$projectid."'");
addDefaultConditions($querybooked);

$querybooked->addGroupBy("phase.id");

$querystringbooked = $querybooked->buildSelect(TRUE);
$dbrecordsbooked = $db->getrows($querystringbooked);

$cntrec=count($dbrecordsbooked);
for ($i=0;$i<$cntrec;$i++)
{
  $querybooked->deAlias($dbrecordsbooked[$i]);
}

// Select the TIMES that have been planned on the project
$queryplanned  = &$db->createQuery();
$queryplanned->addTable('project');
$queryplanned->addJoin('phase', '', 'project.id=phase.projectid', FALSE);

$queryplanned->addField('id', ' ', 'phase', 'phase_');
$queryplanned->addField('project.name AS project_name');
$queryplanned->addField('phase.name AS phase_name');
$queryplanned->addField('SUM(phase.current_planning) AS planning');
$queryplanned->addField('phase.id AS phase_id');
$queryplanned->addCondition("project.id='".$projectid."'");
addDefaultConditions($queryplanned);

$queryplanned->addGroupBy("project.id");
$queryplanned->addGroupBy("phase.id");

$querystringplanned = $queryplanned->buildSelect(TRUE);
$dbrecordsplanned = $db->getrows($querystringplanned);

$cntrec=count($dbrecordsplanned);
for ($i=0;$i<$cntrec;$i++)
{
  $queryplanned->deAlias($dbrecordsplanned[$i]);
}

//make an gant array, this array contains important information about the phases
$gant = array();
for($i=0;$i<count($dbrecordsphase);$i++)
{
  $gant[($dbrecordsphase[$i]['phase_id'])]['id'] = $dbrecordsphase[$i]['phase_id'];
  $gant[($dbrecordsphase[$i]['phase_id'])]['name'] = $dbrecordsphase[$i]['phase_name'];
  $gant[($dbrecordsphase[$i]['phase_id'])]['maxphasetime'] = $dbrecordsphase[$i]['phase_current_planning'];
  $gant[($dbrecordsphase[$i]['phase_id'])]['startdate'] = $dbrecordsphase[$i]['phase_startdate'];
  $gant[($dbrecordsphase[$i]['phase_id'])]['enddate'] = $dbrecordsphase[$i]['phase_enddate'];
}

//extend the gant array with the booked hours of the phase
for($i=0;$i<count($dbrecordsbooked);$i++)
{
  $gant[($dbrecordsbooked[$i]['phase_id'])]['booked'] = $dbrecordsbooked[$i]['hours'];
}

//extend the gant array with the planned hours of the phase
for($i=0;$i<count($dbrecordsplanned);$i++)
{
  $gant[($dbrecordsplanned[$i]['phase_id'])]['planned'] = $dbrecordsplanned[$i]['planning'];
}

//order the gant array with on the startdate of that phase
function cmp ($a, $b) {
    if ($a['startdate'] == $b['startdate']) return 0;
    return ($a['startdate'] < $b['startdate']) ? -1 : 1;
}

usort ($gant, "cmp");

$graph = new GanttGraph(0,0,"auto");
$graph->SetBox();
$graph->SetShadow();

// Add title and subtitle
//$graph->title->Set(atktext('resource_planning_projectsurvey').': '.$dbrecordsphase[0]['project_name']);
//$graph->subtitle->Set("(ganttex14.php)");

// Show day, week and month scale
$graph->ShowHeaders(GANTT_HDAY | GANTT_HWEEK | GANTT_HMONTH);

// Use the short name of the month together with a 2 digit year
// on the month scale
$graph->scale->month->SetStyle(MONTHSTYLE_SHORTNAMEYEAR2);
$graph->scale->month->SetFontColor("white");
$graph->scale->month->SetBackgroundColor("blue");

// 0 % vertical label margin
$graph->SetLabelVMarginFactor(1);

$i=0;
$activity=array();

$reverselookup = array();

foreach ($gant as $id=>$gantphase)
{
  $tmpGant =  new GanttBar($i, $gantphase['name'], $gantphase['startdate'], $gantphase['enddate']);
  $activity[$i] = $tmpGant;
  $activity[$i]->SetPattern(BAND_RDIAG, "yellow");
  $activity[$i]->SetFillColor("red");
  $activity[$i]->SetHeight(10);

  $reverselookup[$gantphase["id"]] = $i;

  $caption='';
  if($gantphase['booked'] <= $gantphase['planned'])
  {
    if(!empty($gantphase['booked']))
    {
      if(!empty($gantphase['planned']))
      {
        $tempb = $gantphase['booked']/$gantphase['planned'];
        $tempp = $gantphase['planned'];
        $activity[$i]->progress->Set($tempb);

        $caption .= '['.round($gantphase['booked']/60).'/';
        $caption .= round($gantphase['planned']/60).']';
      }
    }
    else
    {
      if(!empty($gantphase['planned'])&&$gantphase["maxhours"]>0)
      {
        $tempp = $gantphase['planned'] / (($gantphase['maxhours'])*60);
        $activity[$i]->progress->Set((0.0000000001), $tempp);

        $caption .= '[0/'.round($gantphase['planned']/60).']';
      }
    }
  }
  else
  {
    $activity[$i]->SetPattern(BAND_RDIAG, "red");
  }
  $activity[$i]->caption->Set($caption);
  //$graph->Add($activity[$i]);
  $i++;
}

atkimport("module.utils.dateutil");

// milestones
$milestonenode = &atkGetNode("project.deliverable");
$deliverables = $milestonenode->selectDb("project_id=".$projectid, "duedate");
for( $i=0, $_i=count($deliverables); $i<$_i; $i++)
{
  $due = date("Y-m-d", dateutil::arr2stamp($deliverables[$i]["duedate"]));
  $ms = new MileStone((count($gant))+($i), $deliverables[$i]["name"], $due, $due." (".$deliverables[$i]["name"].")");
  $graph->Add($ms);
}

// dependencies
for ($i=0, $_i=count($dbrecordsdep); $i<$_i; $i++)
{
  $activity[$reverselookup[$dbrecordsdep[$i]["dependency_phaseid_row"]]]->SetConstrain($reverselookup[$dbrecordsdep[$i]["dependency_phaseid_col"]], CONSTRAIN_ENDSTART);
}

for ($i=0, $_i=count($activity); $i<$_i; $i++)
{
  $graph->Add($activity[$i]);
}

//TO DO: find a good solution for errorhandling

//here you can set a subtitle
//$graph->subtitle->Set('title');


if (count($gant) == 0)
{
  $graph->SetDateRange(time(), time()+86400);
}

//Add only a vertical line with the actual date when this actual date is between the start- and enddate of the project
$startdateproject = "";
$enddateproject = "";
foreach($gant as $phase)
{
  if(isset($phase["startdate"]) && ($phase["startdate"]<$startdateproject || $startdateproject=="")) $startdateproject = $phase["startdate"];
  if(isset($phase["enddate"]) && ($phase["enddate"]>$enddateproject || $enddateproject=="")) $enddateproject = $phase["enddate"];
}

if($startdateproject <= (date("Y-m-d")) AND $enddateproject >= (date("Y-m-d")))
{
  $vline = new GanttVLine(date("Y-m-d"), date("d-m-Y"));
  $vline->SetDayOffset(0.5);
  $graph->Add($vline);
}

$graph->Stroke();
?>
