<?php

define("AF_FORMSUBMIT", AF_SPECIFIC_1);

class atkProjectAttribute extends atkAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function atkProjectAttribute($name, $flags=0)
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
    $selectedprojectid = $_REQUEST["selectedprojectid"];

    // The current phase (in case of edit of an existing record)
    $currentproject = $record[$this->fieldName()];
    if (is_array($currentproject)) $currentproject = $currentproject["projectid"];

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
    $result.= '<option value="">---';

    for ($i=0;$i<count($projects);$i++)
    {
      $result .= '<option value="'.$projects[$i]["id"].'" ';

      // de juiste default selecteren
      if ($projects[$i]["id"]==$currentproject) $result.="selected";

      $result .= '>'.$projects[$i]["name"];
    }

    $result .= '</select>&nbsp';

    $targeturl = $_SERVER["PHP_SELF"]."?selectedprojectid=[id]&atklevel=".atkLevel();

    if ($this->hasFlag(AF_FORMSUBMIT)) $submit = true; else $submit = false;
    $result .= href(dispatch_url("project.project", "select", array("atkfilter" => "project.status='active'", "atktarget" => $targeturl)), text("houradmin_searchallprojects"),SESSION_NESTED, $submit);

    return $result;
  }

  function display($rec)
  {
    return $rec[$this->fieldName()]["projname"];
  }

  function db2value($rec)
  {
    return Array("projname"=>$rec["proj_name"], "projectid"=>$rec["projectid"]);
  }

  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
    if ($mode == "select" || $mode == "admin")
    {
      //$query->addField($this->fieldName(),"","hours",$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      $query->addField("name","", "B", "proj_", false);
      $query->addField("projectid","", "costregistration", "", false);
      //$query->addField("name","", "A", "phase_", false);
      //$query->addField("name","", "C", "activity_", false);
      //$query->addJoin("activity", "C", "C.id=hours.activityid", false);
      //$query->addJoin("phase", "A","A.id=hours.phaseid", true);
      $query->addJoin("project", "B","B.id=costregistration.projectid", true);

    }

    else if ($mode == "add" || $mode == "edit")
      $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
  }

  function getRecentProjects()
  {
    global $g_db, $g_user;

    $query = "SELECT project.name as name, project.id as id FROM project,phase,hours ";
    $query.= "WHERE phase.id=hours.phaseid AND userid='".$g_user["name"]."'
                AND phase.status='active'
                AND project.status='active'
                AND phase.projectid=project.id ";
    $query.= "ORDER BY hours.entrydate DESC";

    // 100 should be enough to get 5 unique recent projects.
    $arr = $g_db->getrows($query,'0','100');
    $j=0;
    $alreadyhad = Array();
    $newarr = Array();
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
