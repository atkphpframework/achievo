<?php
 /**
   * Module Resource planning
   * Weekview employee
   *
   * This file generates the weekview for viewing the personal planning of employees.
   *
   * $Id$
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision$
   *
   */

global $g_layout;
global $g_securityManager;
global $g_db;
global $projectid;
global $selectedprojectid;
global $userid;
global $weekview;
global $viewdate;
global $g_sessionManager;
global $g_user;

$userid=$g_user["name"];

// weekview and viewdate are variables that we need to remember..
$weekview = $g_sessionManager->stackVar("weekview");
$viewdate = $g_sessionManager->stackVar("viewdate");

if ($viewdate=="") $viewdate = strftime("%Y-%m-%d");

$projectid = $g_sessionManager->stackVar("projectid");
$view_all = $g_securityManager->allowed('resource_planning','view_all');

if ($viewdate!="")
{
  $viewtime = mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
}
else
{
  $viewdate = date("Y-m-d");
  $viewtime = time();
}
$g_layout->initGUI();
$g_layout->ui_top(text('title_resource_planning_planweekviewempl').' '.strftime('%V',$viewtime));

$weekday = strftime("%w",$viewtime);
if ($weekday==0) $weekday=7;

// Filling the weekarray with the correct dates en days
for ($i=0; $i<=8;$i++)
{
  $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
  $weekdates[$i]['day']=text(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
  $weekdates[$i]['stamp']=$viewtime-(86400*($weekday-$i));
}

// Building the query for the list with the planning for a week for an employee
$name = "atk".atkconfig("database")."query";
$query = new $name();
$query->addTable('employee_project');
$query->addJoin('person', '', 'person.id=employee_project.employeeid', FALSE);
$query->addJoin('planning', '', "planning.employeeid=employee_project.employeeid AND planning.plandate BETWEEN '".$weekdates[1]['date']."' AND '".$weekdates[7]['date']."'", TRUE);
$query->addJoin('phase', '', 'planning.phaseid=phase.id', TRUE);
$query->addJoin('project', '', 'phase.projectid=project.id', TRUE);

$query->addField('id', ' ', 'planning', 'planning_');
$query->addField('plandate', ' ', 'planning', 'planning_');
$query->addField('time', ' ', 'planning', 'planning_');
$query->addField('name', ' ', 'project', 'project_');
$query->addField('lastname', ' ', 'person', 'employee_');
$query->addField('firstname', ' ', 'person', 'employee_');
$query->addField('userid', ' ', 'person', 'employee_');
$query->addField('id', ' ', 'person', 'employee_');
$query->addField('abbreviation', ' ', 'project', 'project_');
$query->addField('id', ' ', 'project', 'project_');

$query->addCondition("person.userid='".$userid."'");

$query->addOrderBy('plandate');

$querystring = $query->buildSelect(TRUE);
$dbrecords = $g_db->getrows($querystring);

// Call to function deAlias for every item in the result of the query, so we can see the aliasnames as defined by addField
$cntrec=count($dbrecords);
for ($i=0;$i<$cntrec;$i++)
{
  $query->deAlias($dbrecords[$i]);
}

// First a line with previous and next week links containing the viewdate 1 day before the first day of the week that is viewed
// or 1 day after the last day of the week that is viewed
$prevnextlinks = '<br>'.href('dispatch.php?resource_planning.resource_planning&planweekviewempl=1&viewdate='.$weekdates[0]['date'], text('houradmin_previousweek'));
$prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?resource_planning.resource_planning&planweekviewempl=1&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));
$prevnextlinks.='<br><br>';
$g_layout->output($prevnextlinks);

// Start printing the top of the table
$g_layout->output($g_layout->data_top());
$g_layout->output($g_layout->tr_top());
$g_layout->td_datatitle(text(employee));

for ($i=1;$i<=7;$i++)
{
  // Printing the days of the week and their dates
  $g_layout->td_datatitle(text($weekdates[$i]['day']).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')','colspan="2" align="center"');    }
  $g_layout->output($g_layout->tr_bottom());
  $cntrec=count($dbrecords);
  if ($cntrec>0)
  {
    for ($i = 0; $i < count($dbrecords); $i++)
    {
      $employee = $dbrecords[$i]['employee_lastname'].", ".$dbrecords[$i]['employee_firstname'];
      $day = date("w",strtotime($dbrecords[$i]['planning_plandate']));
      if ($day==0) $day=7;
      $result[$employee][$day][] = $dbrecords[$i];
      $result[$employee][0]=$dbrecords[$i]['employee_id'];
    }

    // For each employee the planning for the whole week will be shown
    foreach($result as $employee => $day)
    {
      $g_layout->output($g_layout->tr_top('1','onmouseover="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \''.$g_theme["RowMouseOver"].'\'" onmouseout="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \'\'"'));
      $g_layout->td($employee, 'rowspan="2"');

      //For every day will be looked if there are planned items, if so they will be printed on the screen
      for ($w = 1; $w <= 7; $w++)
      {
        if (isset($day[$w]))
        {
          $projects = '';
          $times = '';
          foreach($day[$w] as $item)
          {
            $linkdate=$weekdates[$w]['date'];
            $url=dispatch_url('resource_planning.resource_planning','view', array("atkselector" => "planning.id=$item[planning_id]"));

            // If the project has no abbreviation the first 10 characters of the name of the project will be shown
            if ($item['project_abbreviation']!='') $abbreviation = $item['project_abbreviation'];
            else $abbreviation = substr($item['project_name'], 0, 10);

            $projects.=href($url, $abbreviation, SESSION_NESTED).'<br>';
            $times.=time_format($item['planning_time']).'<br>';
          }
          $g_layout->td($projects, 'valign="top"');
          $g_layout->td($times, 'valign="top"');
        }
        else
        {
          $g_layout->td('');
          $g_layout->td('');
        }
      }
      $g_layout->output($g_layout->tr_bottom());
      $g_layout->output($g_layout->tr_top());

      //Now the viewlink will be added under the planned items
      for ($w = 1; $w <= 7;$w++)
      {
        $linkdate=$weekdates[$w]['date'];
        $linkname=$day[0];
        $datenow=date("Y-m-d");
        $urla=dispatch_url('resource_planning.resource_planning', 'list', array("atkfilter" => "plandate='$linkdate' AND employeeid.id='$linkname'", "noactions" => "1"));
        $g_layout->td('&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp'.href($urla, text(view), SESSION_NESTED), 'colspan="2" align="left"');
      }
      $g_layout->output($g_layout->tr_bottom());
    }
  }
  $g_layout->output($g_layout->data_bottom());
  $g_layout->output('<br>');
  $g_layout->ui_bottom();
?>