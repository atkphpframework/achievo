<?php
 /**
   * Module Resource planning
   * Weekview resource planning
   *
   * This file generates the weekview for planning hours. After selecting a project in the listbox the planning of the employees which may be planned for
   * the selected project will be shown.
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
global $g_theme;


// weekview and viewdate are variables that we need to remember..
$weekview = $g_sessionManager->stackVar("weekview");
$viewdate = $g_sessionManager->stackVar("viewdate");

if ($viewdate=="") $viewdate = strftime("%Y-%m-%d");

$projectid = $g_sessionManager->stackVar("projectid");

// If we came from 'search all projects', a selectedprojectid var is filled.
if (isset($selectedprojectid)&&$selectedprojectid!="")
{
 $projectid=$selectedprojectid;
}

$view_all = $g_securityManager->allowed('resource_planning','view_all');

if ($outputType==0)
{
  $g_layout->initGUI();
  $g_layout->ui_top(text("title_hours_stats"));
  $g_layout->output('<center>');
  $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');
  $g_layout->output('<input type="hidden" name="atknodetype" value="resource_planning.resource_planning">');
  $g_layout->output('<input type="hidden" name="atkaction" value="planweekview">');

  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->output('</tr><tr>');
  $g_layout->td('<hr>'.text('title_resource_planning_search_by_project').':','colspan="2"');
  $g_layout->output('</tr><tr>');
  $g_layout->td(text('project').':');
  $projectatt = new atkplanProjectAttribute('projectid');

  // we have to pass a 'dummy' record to the attributes to set their default value.
  $dummyrec = Array("projectid"=>$projectid);
  $g_layout->td($projectatt->edit($dummyrec));
  $projectid= $dummyrec['projectid'];

  $g_layout->output('</tr><tr>');
  $g_layout->output('</tr><tr>');
  $g_layout->td('<hr>','colspan="2"');
  $g_layout->output('</tr><tr>');
  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form>');
  $g_layout->output('</center>');
  $g_layout->ui_bottom();
}

if ($viewdate!="")
{
  $viewtime = mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
}
else
{
  $viewdate = date("Y-m-d");
  $viewtime = time();
}

// This makes the title bar shows the right title
if($projectid=='-1' OR $projectid=='') $g_layout->ui_top(text('title_resource_planning_planweekview_allselected').' '.strftime('%V',$viewtime));
else $g_layout->ui_top(text('title_resource_planning_planweekview_projectselected').' '.strftime('%V',$viewtime));

$weekday = strftime("%w",$viewtime);
if ($weekday==0) $weekday=7;

// Filling the weekarray with the correct dates en days
for ($i=0; $i<=8;$i++)
{
  $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
  $weekdates[$i]['day']=text(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
  $weekdates[$i]['stamp']=$viewtime-(86400*($weekday-$i));
}

// Building the query for the list with the planning for a week for employees who are related to the selected project above
$name = "atk".atkconfig("database")."query";
$query = new $name();
$query->addTable('project_person');
$query->addJoin('person', '', 'person.id=project_person.personid', FALSE);
$query->addJoin('planning', '', "planning.employeeid=project_person.personid AND planning.plandate BETWEEN '".$weekdates[1]['date']."' AND '".$weekdates[7]['date']."'", TRUE);
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

// If in the listbox above all projects is selected the planning for a week for all employees will be selected.
// TO DO: It is not possible to make a planning for people because no project is selected en achievo does not know what project the
// planned hours should be booked on.
$query->addCondition("person.role='employee'");
if(isset($projectid)&&$projectid!='-1') $query->addCondition("project_person.projectid='".$projectid."'");

$query->addOrderBy('person.lastname');
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
$prevnextlinks = '<br>'.href('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date'].'projectid='.$projectid, text('houradmin_previousweek'));
$prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));
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
  $g_layout->td_datatitle(text(weektotal));
  $g_layout->output($g_layout->tr_bottom());
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
    $daytotals = array();
    foreach($result as $employee => $day)
    {
      $g_layout->output($g_layout->tr_top('1','onmouseover="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \''.$g_theme["RowMouseOver"].'\'" onmouseout="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \'\'"'));
      $g_layout->td($employee, 'rowspan="3"');

      //For every day will be looked if there are planned items, if so they will be printed on the screen
      for ($w = 1; $w <= 7; $w++)
      {
        unset($daytotals[$w]);
        if (isset($day[$w]))
        {
          $projects = '';
          $times = '';
          foreach($day[$w] as $item)
          {
            $linkdate=$weekdates[$w][date];
            $viewdate = date("Y-m-d");

            // If the date of the printed day is smaller the the date now the planned item can only be viewed (no changes are allowed)
            // because you allways plan in the future
            if ($linkdate<$viewdate) $url=dispatch_url('resource_planning.resource_planning','view', array("atkselector" => "planning.id=$item[planning_id]"));
            else $url=dispatch_url('resource_planning.resource_planning','edit', array("atkselector" => "planning.id=$item[planning_id]", "employeefilter" => "person.id=$item[employee_id]", "projectfilter" => "projectid='$item[project_id]'"));

            // If the project has no abbreviation the first 10 characters of the name of the project will be shown
            if ($item[project_abbreviation]!='') $abbreviation = $item[project_abbreviation];
            else $abbreviation = substr($item[project_name], 0, 10);

            $projects.=href($url, $abbreviation, SESSION_NESTED).'<br>';
            $times.=time_format($item[planning_time]).'<br>';
            $daytotals[$w] += $item[planning_time];
          }
          $g_layout->td($projects, 'valign="top" nowrap');
          $g_layout->td($times, 'valign="top" nowrap');
        }
        else
        {
          $g_layout->td('');
          $g_layout->td('');
        }
      }
      $g_layout->td();
      $g_layout->output($g_layout->tr_bottom());
      $g_layout->output($g_layout->tr_top());

      //Now the daytotals will be displayed

      for ($w = 1; $w <= 7;$w++)
      {
        $g_layout->td();
        if (isset($daytotals[$w]))
        {
          $overtime = atkconfig("overtimethreshold");
          if ($daytotals[$w]>$overtime) $g_layout->td(time_format($daytotals[$w]), '<b><font color="#FF0000"');
          else
          {
            if ($daytotals[$w]==$overtime) $g_layout->td(time_format($daytotals[$w]), '<b><font color="#0000FF"');
            else $g_layout->td(time_format($daytotals[$w]), '<b><font color="#009900"');
          }
        }
        else $g_layout->td();
      }
      $weektotal = array_sum($daytotals);
      $g_layout->td(time_format($weektotal));
      $g_layout->output($g_layout->tr_bottom());
      $g_layout->output($g_layout->tr_top());


      //Now the add and view links will be added under the planned items
      for ($w = 1; $w <= 7;$w++)
      {
        $linkdate=$weekdates[$w]['date'];
        $linkname=$day[0];
        $datenow=date("Y-m-d");
        // If the date of the printed day is smaller then the date now there will be no add link available because you allways plan in the future
          if ($linkdate<$datenow)
        {
          $urla=dispatch_url('resource_planning.resource_planning', 'list', array("atkfilter" => "plandate='$linkdate' AND employeeid.id='$linkname'", "noactions" => "1"));
          $g_layout->td('&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp'.href($urla, text(view), SESSION_NESTED), 'colspan="2" align="left"');
        }
        else
        {
          // If there is no project selected (notset or -1) there will be no add link available because achievo doesn't know which project to plan on
          if (!isset($projectid) || $projectid==-1)
          {
            $urla=dispatch_url('resource_planning.resource_planning', 'list', array("atkfilter" => "plandate='$linkdate' AND employeeid.id='$linkname'"));
            $g_layout->td((href($urla, text(view), SESSION_NESTED)), 'colspan="2" align="center" nowrap');
          }
          else
          {
            $urla=dispatch_url('resource_planning.resource_planning', 'add', array("atkfilter" => "plandate='$linkdate' AND employeeid.id='$linkname'", "projectfilter" => "projectid='$projectid'"));
            $urlb=dispatch_url('resource_planning.resource_planning', 'list', array("atkfilter" => "plandate='$linkdate' AND employeeid.id='$linkname'"));
            $g_layout->td((href($urla, text(resource_planning_addplan), SESSION_NESTED)).' '.(href($urlb, text(view), SESSION_NESTED)), 'colspan="2" align="center" nowrap');
          }
        }
      }
      $g_layout->td();
      $g_layout->output($g_layout->tr_bottom());
    }
  }
  $g_layout->output($g_layout->data_bottom());
  if ($cntrec==0) $g_layout->output('<br>'.text('resource_planning_no_employee_by_project').'<br><br>');
  else $g_layout->output('<br>');
  $g_layout->ui_bottom();
?>