<?php
 /**
   * Module Resource planning
   * Weekview resource planning
   *
   * This file generates a ganchart. After selecting a project in the listbox the gantchart of the
   * the selected project will be shown.
   *
   * $Id
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision
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

$projectid = $g_sessionManager->stackVar("projectid");

$data = $this->selectDb($where,"","",array("customer","description","status"),"");

// If we came from 'search all projects', a selectedprojectid var is filled.
if (isset($selectedprojectid)&&$selectedprojectid!="")
{
 $projectid=$selectedprojectid;
}

$view_all = $g_securityManager->allowed('resource_planning','view_all');

if ($outputType==0)
{
  $g_layout->ui_top(text("title_hours_stats"));
  $g_layout->output('<center>');
  $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');
  $g_layout->output('<input type="hidden" name="atknodetype" value="resource_planning.resource_planning">');
  $g_layout->output('<input type="hidden" name="atkaction" value="gantview">');

  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->output('</tr><tr>');
  $g_layout->td('<hr>'.text('title_resource_planning_search_by_project').':','colspan="2"');
  $g_layout->output('</tr><tr>');
  $g_layout->td(text('project').':');
  $projectatt = new atkplanProjectAttribute('projectid');

  // we have to pass a 'dummy' record to the attributes to set their default value.
  //$dummyrec = Array("projectid"=>$projectid);
  $g_layout->td($projectatt->edit($dummyrec));
  //$projectid= $dummyrec['projectid'];

  $g_layout->output('</tr><tr>');
  $g_layout->output('</tr><tr>');
  $g_layout->td('<hr>','colspan="2"');
  $g_layout->output('</tr><tr>');
  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form>');
  $g_layout->output('</center>');
  $g_layout->ui_bottom();
}

$g_layout->ui_top(text('Gant view planning'));
$g_layout->output('<br><img src="modules/resource_planning/gantchart.php?projectid='.$projectid.'?" border=0 align=center>');
$g_layout->ui_bottom();
?>