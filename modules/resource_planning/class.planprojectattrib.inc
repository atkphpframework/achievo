<?php
 /**
   * Module Resource planning
   * Class PlanProjectAttribute
   *
   * This class fills a pulldownbox with projects. The projects are those where recently have been planned hours on. You can specify the number of
   * projects in the config.inc.php. If there is still room left in the pulldownbox the recently planned project will be extended with projects where
   * recently have been registered hours on.
   *
   * $Id$
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision$
   *
   */

class atkPlanProjectAttribute extends atkAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function atkPlanProjectAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags);
  }

  /**
   * Returns a piece of html code that can be used in a form to edit this
   * attribute's value.
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {
    global $selectedprojectid, $atkaction, $atklevel, $PHP_SELF;

    // The current phase (in case of edit of an existing record)

    $currentproject = $record['projectid'];


    // The current value might be overriden by use of the 'search all phases' button
    if (isset($selectedprojectid)&&$selectedprojectid!="")
    {
      $currentproject = $selectedprojectid;
    }

    $projects = $this->getRecentProjects();
    $projectarray = Array();

    $alreadyadded = false;

    // Check if the currently selected project is already in the recent list.
    for ($i=0;$i<count($projects);$i++)
    {
      if ($projects[$i]['id']==$currentproject)
      {
        $alreadyadded = true;
      }
    }

    // if not already added (by the recent list), we have to load it manually.
    if (!$alreadyadded&&$currentproject!="")
    {
      $project = $this->getProject($currentproject);
      $projects[] = $project;
    }

    $result.= '<select name="projectid">';

    $result.= '<option value="-1">'.text("allprojects");

    for ($i=0;$i<count($projects);$i++)
    {
      $result .= '<option value="'.$projects[$i]["id"].'" ';

      // select the right default
      if ($projects[$i]["id"]==$currentproject) $result.="selected";

      $result .= '>'.$projects[$i]["name"];
    }

    $result .= '</select>&nbsp';

    $targeturl = $PHP_SELF."?selectedprojectid=[id]&atklevel=$atklevel";
    $result .= href(dispatch_url("project", "select", array("atkfilter" => "project.status='active'", "atktarget" => $targeturl)), text("houradmin_searchallprojects"),SESSION_NESTED);

    return $result;
  }

  function getRecentProjects()
  {
    global $g_db, $g_user;

    $query = "SELECT project.name as name, project.id as id, planning.plandate as plandate FROM project,phase,planning ";
    $query.= "WHERE phase.id=planning.phaseid
                AND project.status='active'
                AND phase.projectid=project.id
                AND planning.phaseid=phase.id ";

    $query.= "ORDER BY planning.plandate DESC";

    // N*20 should be enough to get N unique recent projects.
    $arr = $g_db->getrows($query,'0',$GLOBALS["config_numberofrecentprojects"]*20);
    $j=0;
    $alreadyhad = Array();
    $newarr = Array();
    //Fill the array with  planned projects
    for ($i=0;$i<count($arr)&&$j<$GLOBALS["config_numberofrecentprojects"];$i++)
    {
      if (!in_array($arr[$i]['id'],$alreadyhad))
      {
        $alreadyhad[$j]=$arr[$i]['id'];
        $newarr[$j]['id'] = $arr[$i]['id'];
        $newarr[$j]['name'] = $arr[$i]['name'];
        $j++;
      }
    }
    //If the projectsarray has room left, it wille be filled with project where have been registered hours on
    if ($j<$GLOBALS["config_numberofrecentprojects"])
    {
      $query = "SELECT project.name as name, project.id as id FROM project,phase,hours ";
      $query.= "WHERE phase.id=hours.phaseid
                AND project.status='active'
                AND phase.projectid=project.id
                AND hours.phaseid=phase.id ";

      $query.= "ORDER BY hours.entrydate DESC";

      // (N-J)*20 should be enough to get N-J unique recent projects.
      $arr = $g_db->getrows($query,'0',($GLOBALS["config_numberofrecentprojects"]-$j)*20);
      for ($i=0;$i<count($arr)&&$j<floor($GLOBALS["config_numberofrecentprojects"]);$i++)
      {
        if (!in_array($arr[$i]['id'],$alreadyhad))
        {
          $alreadyhad[$j]=$arr[$i]['id'];
          $newarr[$j]['id'] = $arr[$i]['id'];
          $newarr[$j]['name'] = $arr[$i]['name'];
          $j++;
        }
      }
    }
  //finally we have to sort the entire array, so it wil be alfabetically ordered in the pulldownbox
  sort($newarr);
  return $newarr;
  }

  function getProject($projectid)
  {
    global $g_db;

    $query = "SELECT id,name FROM project ";
    $query.= "WHERE id = $projectid ";

    $arr = $g_db->getrows($query);

    $project = $arr[0];
    return $project;
  }

}
?>