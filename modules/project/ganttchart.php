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

global $g_layout;
global $config_language;

$projectid = $_REQUEST['projectid'];

function load_dependencies($phase_record,$dep_record)
{
  for ($i=0;$i<count($dep_record);$i++)
  {
    $before[] = $dep_record[$i]['dependency_phaseid_row'];
    $after[] = $dep_record[$i]['dependency_phaseid_col'];
  }

  return array("befores"=>$before, "afters"=>$after);
}

function init_milestones($phase_ids)
{
   $m[0]['in']  = array ();
   $m[0]['out'] = $phase_ids;
   $m[0]['deadline'] = 0;

   $m[1]['in']  = $phase_ids;
   $m[1]['out'] = array ();
   $m[1]['deadline'] = 0;

   return $m;
}

function get_all_paths($m)
{

  $path = '';
  $current_path_nr = 0;
  $out_phases = get_phases($m[0],'out');                            // start with milestone 0

  do
  {
    $path = new_paths($m,$path,$current_path_nr,$out_phases);

    for ($cycle=0,$i=0;$i<count($path)&&$cycle==0;$i++)
    {
      if ($path[$i]['cycle']==1) $cycle = 1;
    }

    if ($cycle==0)
    {
      for ($complete=1,$i=0;$i<count($path)&&$complete==1;$i++)
      {
         if ($path[$i]['complete']==0)                               // incomplete path
         {
            $complete = 0;
            $current_path_nr = $i;
            $milestone = $path[$i]['milestone'][count($path[$i]['milestone'])-1];
            $out_phases =  get_phases($m[$milestone],'out');
         }
      }
    }
  } while ($cycle==0 && $complete==0);

  if ($cycle==1) $path = '';

  return $path;
}

function get_phases($milestone,$edge_type)
{
   $phases = $milestone[$edge_type];

   if (count($phases)==0) $phases = array();

   return $phases;   // returns a array with phases
}

function new_paths($m,$path,$current_path_nr,$out_phases)
{
   if ($path == '') $path[0] = create_new_path();

   $temp = $path[$current_path_nr];

   $path[$current_path_nr]['phase'][]     = $out_phases[0];
   $path[$current_path_nr]['milestone'][] = get_current_milestone($m,$out_phases[0],'in');
   $path[$current_path_nr]['time']        = 0;
   $path[$current_path_nr]['cycle']       = cycle_check($path[$current_path_nr]['phase']);
   $path[$current_path_nr]['complete']    = complete_check($m,$out_phases[0]);

   $new_path_nr = count($path);
   for ($i=1;$i<count($out_phases);$i++)
   {

      $path[$new_path_nr] = $temp;

      $path[$new_path_nr]['phase'][]      = $out_phases[$i];
      $path[$new_path_nr]['milestone'][]  = get_current_milestone($m,$out_phases[$i],'in');
      $path[$new_path_nr]['time']         = 0;
      $path[$new_path_nr]['cycle']        = cycle_check($path[$new_path_nr]['phase']);
      $path[$new_path_nr]['complete']     = complete_check($m,$out_phases[$i]);

      $new_path_nr++;

   }

   return $path;
}

function create_new_path()
{
   $path['phase']       = array();
   $path['milestone']   = array();
   $path['time']        = 0;
   $path['cycle']       = 0;
   $path['complete']    = 0;

   return $path;
}

function get_current_milestone($m,$phase,$edge_type)
{
   $number = '';
   for($counter=0;$counter<count($m);$counter++)
   {
      for ($i=0;$i<count($m[$counter][$edge_type]);$i++)
      {
         if ($m[$counter][$edge_type][$i]== $phase)
         {
            $number = $counter;
         }
      }
   }

   return $number;
}

function cycle_check($path)
{
   $cycle=0;
   for ($i=0;$i<count($path)&&$cycle==0;$i++)
   {
      $phase = $path[$i];
      $counter=0;

      for ($j=0;$j<count($path);$j++)
      {
         if ($path[$j]==$phase) $counter++;
      }
      if ($counter>1) $cycle=1;
   }
   return $cycle;
}

function complete_check($m,$phase)
{

   for ($i=0;$i<count($m);$i++)
   {
      if (is_edge($m[$i],$phase)) $milestone=$i;
   }

   if (count($m[$milestone]['out'])==0) $complete=1; else $complete=0;

   return $complete;
}

function is_edge($m,$phase)
{
   $result=0;
   for ($i=0;$i<count($m['in'])&&$result==0;$i++)
   {
      if ($m['in'][$i]==$phase) $result=1;
   }

   for ($i=0;$i<count($m['out'])&&$result==0;$i++)
   {
      if ($m['out'][$i]==$phase) $result=1;
   }

   return $result;
}

function get_phase_pos($path,$phase)
{
   $pos = -1;

   for ($i=0;$i<count($path);$i++)
   {
      if ($path[$i]==$phase) $pos = $i;
   }
   return $pos;
}

function del_edge(&$m, $current_m, $in, $out)
{

   $del = $in;
   $edge_type = 'in';
   for ($counter=0;$counter<2;$counter++)
   {
      if ($del != '')
      {
         for ($i=0;$i<count($m[$current_m][$edge_type]);$i++)
         {
            if ($del != $m[$current_m][$edge_type][$i]) $new[] = $m[$current_m][$edge_type][$i];
         }
         $m[$current_m][$edge_type] = $new;
      }
   $del = $out;
   $edge_type = 'out';
   }
}

function ins_edge(&$m, $current_m, $in, $out)
{

   if ($in!='')  $m[$current_m]['in'][]  = $in;
   if ($out!='') $m[$current_m]['out'][] = $out;
}

function ins_milestone(&$m, $before, $after)
{
   $m[count($m)] = $m[count($m)-1];   //doesn't work in php3

   $m[count($m)-1]['in']  = $m[count($m)-2]['in'];
   $m[count($m)-1]['out'] = $m[count($m)-2]['out'];
   $m[count($m)-1]['deadline'] = $m[count($m)-2]['deadline'];

   $m[count($m)-2]['in']  = array ();
   $m[count($m)-2]['out'] = array ();
   $m[count($m)-2]['deadline'] = 0;

   $current_m      = get_current_milestone($m,$before,'in');
   del_edge($m, $current_m, $before, '');
   $current_m      = get_current_milestone($m,$after,'out');
   del_edge($m, $current_m, '', $after);

   ins_edge($m, count($m)-2, $before, $after);
}

function order_milestones(&$m,$path)
{
   for ($i=0;$i<count($m);$i++)
   {
      $m_order[$i] = $i;
   }

   for ($i=0;$i<count($path);$i++)
   {
      do
      {
         $changes = 0;

         for ($j=1;$j<count($path[$i]['milestone']);$j++)
         {
            $before = $path[$i]['milestone'][$j-1];
            $after  = $path[$i]['milestone'][$j];

            $current_pos_before = check_pos($before,$m_order);
            $current_pos_after = check_pos($after,$m_order);

            if (($current_pos_before != -1) && ($current_pos_after != -1))
            {
               if ($current_pos_before > $current_pos_after)
               {
                  swap($m_order,$current_pos_before,$current_pos_after);
                  $changes = 1;
               }
            }
         }
      } while ($changes == 1);
   }

   $m_old = $m;
   for ($i=0;$i<count($m);$i++)
   {
      $number = $m_order[$i];
      $m[$i]  = $m_old[$number];
   }
}

function check_pos($thing,$order)
{
   $pos = -1;
   for ($i=0;$i<count($order)&&$pos==-1;$i++)
   {
      if ($order[$i] == $thing) $pos = $i;
   }
   return $pos;
}

function calc_milestone_deadlines(&$m,$phase,$phase_maxtime)
{
   for ($i=1;$i<count($m);$i++)
   {
      for ($j=0;$j<count($phase);$j++)
      {
         $out_m = get_current_milestone($m,$phase[$j],'out');
         $in_m  = get_current_milestone($m,$phase[$j],'in');

         if (($in_m == $i) &&
             ($m[$i]['deadline'] <  $m[$out_m]['deadline']+($phase_maxtime[$j]+0)))
         {
            $m[$i]['deadline'] = $m[$out_m]['deadline']+($phase_maxtime[$j]+0);
         }
      }
   }
}

function swap(&$array,$posa,$posb)
{
   $temp = $array[$posa];
   $array[$posa] = $array[$posb];
   $array[$posb]  = $temp;
}

// Select the dependency's  for the gantchart
$name = "atk".atkconfig("database")."query";
$querydep  = new $name();
$querydep->addTable('phase');
$querydep->addJoin('project', '' ,'project.id=phase.projectid', FALSE);
$querydep->addJoin('dependency', '' ,'dependency.phaseid_row=phase.id', FALSE);
$querydep->addField('phaseid_row', ' ', 'dependency', 'dependency_');
$querydep->addField('phaseid_col', ' ', 'dependency', 'dependency_');
$querydep->addCondition("project.id='".$projectid."'");
$querystringdep = $querydep->buildSelect(TRUE);
$dbrecordsdep = $g_db->getrows($querystringdep);

$cntrec=count($dbrecordsdep);
for ($i=0;$i<$cntrec;$i++)
{
 $querydep->deAlias($dbrecordsdep[$i]);
}

//atk_var_dump('dependency');
//atk_var_dump($dbrecordsdep);

// Select the PHASES which belong to the project
$name = "atk".atkconfig("database")."query";
$queryphase = new $name();
$queryphase->addTable('project');
$queryphase->addJoin('phase', '', 'project.id=phase.projectid', FALSE);
$queryphase->addField('id', ' ', 'phase', 'phase_');
$queryphase->addField('name', ' ', 'project', 'project_');
$queryphase->addField('name', ' ', 'phase', 'phase_');
$queryphase->addField('max_phasetime', ' ', 'phase', 'phase_');
$queryphase->addField('max_hours', ' ', 'phase', 'phase_');
$queryphase->addField('startdate', ' ', 'project', 'project_');
$queryphase->addCondition("project.id='".$projectid."'");
$querystringphase = $queryphase->buildSelect(TRUE);
$dbrecordsphase = $g_db->getrows($querystringphase);

$cntrec=count($dbrecordsphase);
for ($i=0;$i<$cntrec;$i++)
{
  $queryphase->deAlias($dbrecordsphase[$i]);
}

//atk_var_dump('phase');
//atk_var_dump($dbrecordsphase);

for($i=0;$i<count($dbrecordsphase);$i++)
{
  $phase_ids[$i] = $dbrecordsphase[$i]['phase_id'];
  $phase_names[$i] = $dbrecordsphase[$i]['phase_name'];
  $phase_maxtimes[$i] = $dbrecordsphase[$i]['phase_max_phasetime'];
}


//atk_var_dump('phase_ids');
//atk_var_dump($phase_ids);

//atk_var_dump('phase_names');
//atk_var_dump($phase_names);

//atk_var_dump('phase_maxtimes');
//atk_var_dump($phase_maxtimes);


$dep = load_dependencies($phase_ids,$dbrecordsdep);

//atk_var_dump('dep');
//atk_var_dump($dep);

$m = init_milestones($phase_ids);

//atk_var_dump('m');
//atk_var_dump($m);

  // Startpoints
  $befores = $dep["befores"];
  $afters = $dep["afters"];

  for ($i=0;$i<count($befores)&&$error==0;$i++)
  {
    $before_phase  = $befores[$i];
    $after_phase = $afters[$i];

    $m_old = $m;
    $nonew_m = -2;
    $dependency = '?';
    $error = 0;

    do
    {
      $path = get_all_paths($m);

      // calculate current dependency

      if ($path[0]['phase'][0] != '')
      {
         $dep_status = 0;
         for ($path_nr=0;$path_nr<count($path)&&$dep_status==0;$path_nr++)
         {
            $before_pos = get_phase_pos($path[$path_nr]['phase'],$before_phase);
            $after_pos  = get_phase_pos($path[$path_nr]['phase'],$after_phase);

            if ($before_pos!='-1' && $after_pos!='-1')
            {
               if ($before_pos>=$after_pos) $dep_status = -1;
               else $dep_status = 1;
            }
         }
      }
      else
      {
         $dep_status = -1;
      }

      // evaluate situation

      if ($dep_status>0)
      {
         $dependency = 'ok';
      }
      elseif ($nonew_m<-1)
      {
        $nonew_m = -1;
      }
      else
      {
         $m = $m_old;          //restore milestone table
         switch ($nonew_m)
         {
         case '-1':
            $nonew_m=1;
            break;
         case '1':
            $nonew_m=0;
            break;
         default:
            $error=1;
         }
      }

      // change milestone table

      if ($dependency!='ok' && $error==0)
      {
         switch ($nonew_m)
         {
            case '-1':
               // m_after_out = m_before_in
               $current_m = get_current_milestone($m,$after_phase,'out');
               del_edge($m, $current_m, '', $after_phase);
               $current_m = get_current_milestone($m,$before_phase,'in');
               ins_edge($m, $current_m,'',$after_phase);
            break;

            case '1':
               // m_before_in = m_after_out
               $current_m = get_current_milestone($m,$before_phase,'in');
               del_edge($m, $current_m, $before_phase,'');
               $current_m = get_current_milestone($m,$after_phase,'out');
               ins_edge($m, $current_m, $before_phase, '');
            break;

            default:
               //insert new milestone
               ins_milestone($m, $before_phase, $after_phase);
         }
      }
    }
    while ($dependency!='ok' && $error==0);
  }

  // **********

  if ($error)
  {
     echo "<br><b>ERROR : </b>";
     echo $before_phase.' before '.$after_phase;
  }
  else
  {
     order_milestones($m,$path);
     calc_milestone_deadlines($m,$phase_ids,$phase_maxtimes);
  }

// Select the TIMES that have been booked on the project
$name = "atk".atkconfig("database")."query";
$querybooked = new $name();
$querybooked->addTable('phase');
$querybooked->addJoin('hours', '', 'hours.phaseid=phase.id', FALSE);
$querybooked->addField('id', ' ', 'phase', 'phase_');
$querybooked->addField('name', ' ', 'phase', 'phase_');
$querybooked->addField('SUM(hours.time) AS hours');
$querybooked->addField('max_phasetime', ' ', 'phase', 'phase_');
$querybooked->addField('max_hours', ' ', 'phase', 'phase_');
$querybooked->addCondition("phase.projectid='".$projectid."'");

$querybooked->addGroupBy("phase.id");

$querystringbooked = $querybooked->buildSelect(TRUE);
$dbrecordsbooked = $g_db->getrows($querystringbooked);

$cntrec=count($dbrecordsbooked);
for ($i=0;$i<$cntrec;$i++)
{
  $querybooked->deAlias($dbrecordsbooked[$i]);
}

// Select the TIMES that have been planned on the project
$name = "atk".atkconfig("database")."query";
$queryplanned  = new $name();
$queryplanned->addTable('project');
$queryplanned->addJoin('phase', '', 'project.id=phase.projectid', FALSE);
$queryplanned->addJoin('planning', '', 'planning.phaseid=phase.id', FALSE);

$queryplanned->addField('id', ' ', 'phase', 'phase_');
$queryplanned->addField('project.name AS project_name');
$queryplanned->addField('phase.name AS phase_name');
$queryplanned->addField('SUM(planning.time) AS planning');
$queryplanned->addField('phase.id AS phase_id');

$queryplanned->addCondition("project.id='".$projectid."'");

$queryplanned->addGroupBy("project.id");
$queryplanned->addGroupBy("phase.id");

$querystringplanned = $queryplanned->buildSelect(TRUE);
$dbrecordsplanned = $g_db->getrows($querystringplanned);

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
  $gant[($dbrecordsphase[$i]['phase_id'])]['maxphasetime'] = $dbrecordsphase[$i]['phase_max_phasetime'];
  $gant[($dbrecordsphase[$i]['phase_id'])]['maxhours'] = $dbrecordsphase[$i]['phase_max_hours'];
}

//extend the gant array with the start and enddate of the phase
for($i=0;$i<count($m);$i++)
{
  if(!empty($m[$i]['in']))
  {
    for($j=0;$j<count($m[$i]['in']);$j++) $gant[($m[$i]['in'][$j])]['enddate'] = $m[$i]['deadline'];
  }
 if(!empty($m[$i]['out']))
 {
   for($j=0;$j<count($m[$i]['out']);$j++) $gant[($m[$i]['out'][$j])]['startdate'] = $m[$i]['deadline'];
 }
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

$year = $dbrecordsphase[1]['project_startdate'][0].
        $dbrecordsphase[1]['project_startdate'][1].
        $dbrecordsphase[1]['project_startdate'][2].
        $dbrecordsphase[1]['project_startdate'][3];

$month = $dbrecordsphase[1]['project_startdate'][5].
         $dbrecordsphase[1]['project_startdate'][6];

$day = $dbrecordsphase[1]['project_startdate'][8].
       $dbrecordsphase[1]['project_startdate'][9];

$datestamp = mktime(0,0,0,$month, $day, $year);

//the startpoint of each phase must be added with the startdate of the project
for($i=0;$i<count($gant);$i++)
{
  $durationdays = $gant[$i]['startdate'];
  if(!$i==0) $durationdays++;
  $startdate = ($datestamp + ($durationdays*3600*24));
  $gant[$i]['startdate'] = date('Y-m-d', $startdate);
}

//the endpoint of each phase must be added with the startdate of the project
for($i=0;$i<count($gant);$i++)
{
  $durationdays = $gant[$i]['enddate'];
  $startdate = ($datestamp + ($durationdays*3600*24));
  $gant[$i]['enddate'] = date('Y-m-d', $startdate);
}

$graph = new GanttGraph(0,0,"auto");
$graph->SetBox();
$graph->SetShadow();

// Add title and subtitle
//$graph->title->Set(text('resource_planning_projectsurvey').': '.$dbrecordsphase[0]['project_name']);
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

for($i=0;$i<count($gant);$i++)
{
  $activity[$i] = new GanttBar($i*2, $gant[$i]['name'], $gant[$i]['startdate'], $gant[$i]['enddate']);
  $activity[$i]->SetPattern(BAND_RDIAG, "yellow");
  $activity[$i]->SetFillColor("red");
  $activity[$i]->SetHeight(10);

  //to let each phase ends with an circle containing the number of current milestone, include the code beneath
  //$activity[$i]->rightMark->Show();
  //$activity[$i]->rightMark->title->Set("M".($i+1));
  //$activity[$i]->rightMark->SetType(MARK_FILLEDCIRCLE);
  //$activity[$i]->rightMark->SetWidth(10);
  //$activity[$i]->rightMark->SetColor("red");
  //$activity[$i]->rightMark->SetFillColor("red");

  // to show the milestones between the phase, include the code beneath
  //$milestone = new MileStone(($i*2)+1,"  Milestone ".($i+1),$gant[$i]['enddate'],"M ".($i+1));
  //$graph->Add($milestone);

  $caption='';
  if(($gant[$i]['booked']+ $gant[$i]['planned']) <= ($gant[$i]['maxhours'])*60)
  {
    if(!empty($gant[$i]['booked']))
    {
      if(!empty($gant[$i]['planned']))
      {
        $tempb = $gant[$i]['booked'] / (($gant[$i]['maxhours'])*60);
        $tempp = $gant[$i]['planned'] / (($gant[$i]['maxhours'])*60);
        $activity[$i]->progress->Set($tempb);
        $activity[$i]->planned->Set($tempb, $tempp);

        $caption .= '['.round($gant[$i]['booked']/60).', ';
        $caption .= round($gant[$i]['planned']/60).', ';
        $caption .= round($gant[$i]['maxhours']- ($gant[$i]['planned']/60) - ($gant[$i]['booked']/60)).']';
      }
      else
      {
        $tempb = $gant[$i]['booked'] / (($gant[$i]['maxhours'])*60);
        $activity[$i]->progress->Set($tempb);

        $caption .= '['.round($gant[$i]['booked']/60).', ';
        $caption .= round($gant[$i]['maxhours'] - ($gant[$i]['booked']/60)).']';
      }
    }
    else
    {
      if(!empty($gant[$i]['planned']))
      {
        $tempp = $gant[$i]['planned'] / (($gant[$i]['maxhours'])*60);
        $activity[$i]->planned->Set((0.0000000001), $tempp);

        $caption .= '['.round($gant[$i]['planned']/60).', ';
        $caption .= round($gant[$i]['maxhours'] - ($gant[$i]['planned']/60)).']';

      }
      else
      {
        $caption .='['.round($gant[$i]['maxhours']).']';
      }
    }
    $graph->Add($activity[$i]);
  }
  else
  {
    $activity[$i]->SetPattern(BAND_RDIAG, "red");
    $activity[$i]->caption->Set('error');
    $graph->Add($activity[$i]);
  }
  $activity[$i]->caption->Set($caption);
}

//TO DO: find a good solution for errorhandling

//here you can set a subtitle
//$graph->subtitle->Set('title');

//Add only a vertical line with the actual date when this actual date is between the start- and enddate of the project
$startdateproject=$gant[0]["startdate"];
$enddateproject=$gant[count($gant)-1]["enddate"];
if($startdateproject <= (date("Y-m-d")) AND $enddateproject >= (date("Y-m-d")))
{
  $vline = new GanttVLine(date("Y-m-d"), date("d-m-Y"));
  $vline->SetDayOffset(0.5);
  $graph->Add($vline);
}

//TO DO:  make the gantchart multi-language
//if($config_language=='nederlands.lng')$graph->scale->SetDateLocale("nl_NL");
//$graph->scale->SetDateLocale("se_SE");

//$g_layout->outputflush();
$graph->Stroke();

?>
