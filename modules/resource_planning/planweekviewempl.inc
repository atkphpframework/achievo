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
$query->addJoin('employee', '', 'employee.userid=employee_project.employeeid', FALSE);
$query->addJoin('planning', '', "planning.employeeid=employee_project.employeeid AND planning.date BETWEEN '".$weekdates[1][date]."' AND '".$weekdates[7][date]."'", TRUE);
$query->addJoin('phase', '', 'planning.phaseid=phase.id', TRUE);
$query->addJoin('project', '', 'phase.projectid=project.id', TRUE);

$query->addField('id', ' ', 'planning', 'planningid');
$query->addField('date', ' ', 'planning');
$query->addField('time', ' ', 'planning');
$query->addField('name', ' ', 'project', 'projectname');
$query->addField('name', ' ', 'employee', 'employeename');
$query->addField('userid', ' ', 'employee', 'userid');
$query->addField('abbreviation', ' ', 'project', 'projectab');
$query->addField('id', ' ', 'project', 'qprojectid');

$query->addCondition("employee.userid='".$userid."'");

$query->addOrderBy('date');

$querystring = $query->buildSelect(TRUE);
$dbrecords = $g_db->getrows($querystring);

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
      $employee = $dbrecords[$i][4];
      $day = date("w",strtotime($dbrecords[$i][1]));
      if ($day==0) $day=7;
      $result[$employee][$day][] = $dbrecords[$i];
      $result[$employee][0]=$dbrecords[$i][5];
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
          $projects='';
          $times='';
          foreach($day[$w] as $item)
          {
            $linkdate=$weekdates[$w][date];
            $url=dispatch_url('resource_planning.resource_planning','view', array("atkselector" => "planning.id=$item[0]", "employeefilter" => "employee.userid='$item[5]'", "projectfilter" => "projectid='$item[7]'"));
            $projects.=href($url, $item[6], SESSION_NESTED).'<br>';
            $times.=time_format($item[2]).'<br>';
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
        $linkdate=$weekdates[$w][date];
        $linkname=$day[0];
        $datenow=date("Y-m-d");
        $urla=dispatch_url('resource_planning.resource_planning', 'list', array("atkfilter" => "date='$linkdate' AND employeeid.userid='$linkname'", "noactions" => "1"));
        $g_layout->td('&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp'.href($urla, text(view), SESSION_NESTED), 'colspan="2" align="left"');
      }
      $g_layout->output($g_layout->tr_bottom());
    }
  }
  $g_layout->output($g_layout->data_bottom());
  $g_layout->output('<br>');
  $g_layout->ui_bottom();
?>