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
  include_once(atkconfig("atkroot")."atk/utils/adodb-time.inc.php");
  atkimport("module.utils.dateutil");
  include_once("achievotools.inc");
  
  atksession();
  atksecure();
  
  global $g_securityManager;
  
  if ($g_securityManager->allowed("project.project", "any_project"))
  {
    $coordinator = $_REQUEST["coordinator"];
  }
  else 
  {
    $user = getUser();
    $coordinator = $user["id"];
  }
  
  function dashDate($undasheddate)
  {
    return substr($undasheddate, 0, 4)."-".
           substr($undasheddate, 4, 2)."-".
           substr($undasheddate, 6, 2);
  }
       
  $from = dashDate(dateUtil::startOfWeek($_REQUEST["from"]));
  $to = dashDate(dateUtil::endOfWeek($_REQUEST["to"]));
  $plannedbooked = $_REQUEST["plannedbooked"];
  if ($plannedbooked!="planned"&&$plannedbooked!="booked") $plannedbooked="planned"; // safeguard
    
  $db = &atkGetDb();
  
  $projects = $db->getrows("SELECT 
                              project.id, 
                              project.name,
                              MIN(phase.startdate) as startdate,
                              MAX(phase.enddate) as enddate,
                              SUM(phase.current_planning) as planned
                            FROM
                              project,
                              phase
                            WHERE
                              phase.status='active'
                              AND project.status='active'
                              AND phase.projectid = project.id
                              AND (phase.enddate IS NULL OR phase.enddate>='$from')
                              AND (phase.startdate <= '$to')
                              ".($coordinator!=""?"AND project.coordinator=$coordinator":"")."
                            GROUP BY
                              project.id,
                              project.name");   
  
  //make an gant array, this array contains important information about the phases
  $gant = array();
  for($i=0;$i<count($projects);$i++)
  {
    $gant[($projects[$i]['id'])] = $projects[$i];    
    
    $gant[($projects[$i]['id'])]['booked'] = getBooked($projects[$i]['id']);
  }    
  
  function getBooked($projectid)
  {
    $db = &atkGetDb();
    $rows = $db->getrows("SELECT sum(time) as booked FROM hours, phase
                            WHERE hours.phaseid = phase.id AND phase.projectid=".$projectid);
    return $rows[0]["booked"];
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
  
  $colornode = &getNode("project.mastergantt_colorconfig");
  
  foreach ($gant as $id=>$gantphase)
  {  
    // Projects that extent the selected period should be cut off.
    if ($gantphase['startdate']<$from) $gantphase['startdate']=$from;
    if ($gantphase['enddate']>$to || $gantphase['enddate']=='') $gantphase['enddate']=$to;
    
    $caption = "[".time_format($gantphase[$plannedbooked], true)."]";    
    $activity[$i] = &new GanttBar($i, $gantphase['name'], $gantphase['startdate'], $gantphase['enddate'], $caption);
    
    $colorbase = $gantphase[$plannedbooked];
    
    $color = $colornode->getColor($colorbase/60);
    $activity[$i]->SetPattern(BAND_SOLID, $color);    
    $activity[$i]->SetHeight(10);
          
    $activity[$i]->SetFillColor($color);        
        
    $i++;
  }
  
  atkimport("module.utils.dateutil");    
     
  for ($i=0, $_i=count($activity); $i<$_i; $i++)
  {
    $graph->Add($activity[$i]);
  }    
  
  $graph->Stroke();

?>
