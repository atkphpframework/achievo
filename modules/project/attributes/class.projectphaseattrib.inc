<?php

define("AF_JS_UPDATEACTIVITY", AF_SPECIFIC_1);
include_once(moduleDir("project")."class.project.inc"); // used for some defines

class atkProjectPhaseAttribute extends atkAttribute
{
  var $m_nested = true; // nested in a form?
    
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function projectPhaseAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags);
  }
  
  function fetchValue($rec)
  {
    return Array("phaseid"=>$rec["phaseid"]);
  } 

  /**
   * Returns a piece of html code that can be used in a form to edit this
   * attribute's value.
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {        
    global $ATK_VARS;
    
    $selectedphaseid = $_REQUEST["selectedphaseid"];
    $atkaction = $_REQUEST["atkaction"];
    
    // The current phase (in case of edit of an existing record)
    $currentphase = $record[$this->fieldName()]["phaseid"];
    $currentactivity = $record["activityid"];  
    
    // The current value might be overriden by use of the 'search all phases' button
    if (isset($selectedphaseid)&&$selectedphaseid!="")
    {
      $currentphase = $selectedphaseid;
      $currentactivity = 0;
    }
  
    $phases = $this->getRecentPhases();   
    $phasearray = Array();
    
    $alreadyadded = false;
    
    // Check if the currently selected phase is already in the recent list.
    for ($i=0;$i<count($phases);$i++)
    {
      if ($phases[$i]['id']==$currentphase)
      {
        $alreadyadded = true;
      }      
    }
    
    // if not already added (by the recent list), we have to load it manually.
    if (!$alreadyadded&&$currentphase!="")
    {
      $phase = $this->getPhase($currentphase);      
      if (is_array($phase)) // Phase might have been deleted.
      {
        $phases[] = $phase;
      }
      else
      {
        // Apparently currentphase is set to something that no longer exists.
        // We must reset the value.
        $currentphase = "";
      }
    }    
    
    // Load Activities for each project
    for ($i=0, $_i=count($phases);$i<$_i;$i++)
    {
      $phaseid_arr[] = $phases[$i]["id"];      
    }
    if (count($phaseid_arr)>0)
    {
      $activities = $this->getActivities($phaseid_arr);
      for ($i=0, $_i=count($activities);$i<$_i;$i++)
      {
        $phasearray[$activities[$i]["phaseid"]][] = $activities[$i];
      }
    }

    if(!$this->hasFlag(AF_JS_UPDATEACTIVITY))
    {
      $result.= '<select name="phaseid">';   
    } 
    else
    {
      $result .= $this->javascript($phasearray);
      $result.= '<select name="phaseid" onChange="set_activities(this.options[this.selectedIndex].value,0)">';   
    }
    $result.= '<option value="">' . text('select');  
    
    for ($i=0;$i<count($phases);$i++)
    {
      $result .= '<option value="'.$phases[$i]["id"].'" ';
      
      // de juiste default selecteren
      if ($phases[$i]["id"]==$currentphase) $result.="selected";
      
      $result .= '>'.$phases[$i]["projectname"]." - ".$phases[$i]["name"];
    }
    
    $result .= '</select>&nbsp';
        
    /*
    if ($atkaction=="edit")
    {
      $targeturl_houradmin = dispatch_url("timereg.hours", "edit", array("atkselector" => 'hours.id='.$record['id'], "selectedphaseid" => "[id]", "atklevel"=>atkLevel()));      
    }
    elseif($atkaction=="report")
    {
      $targeturl_houradmin = dispatch_url("reports.hoursurvey", "report", array("selectedphaseid" => "[id]", "atklevel"=>atkLevel()));
      $saveform = false; // hoursurvey doesn't support session save..
    }
    else
    {
      $targeturl_houradmin = dispatch_url("timereg.hours", "admin", array("selectedphaseid" => "[id]", "atklevel"=>atkLevel()));
    }
    */
    $return_url = dispatch_url($ATK_VARS["atknodetype"], $ATK_VARS["atkaction"], array("selectedphaseid"=>"[id]", "atklevel"=>atkLevel()));
    
    $targeturl_phase = dispatch_url("project.phase", "select", array("atkfilter" => "projectid=[id]", "atktarget" => atkurlencode($return_url)));
    $result .= href(dispatch_url("project.projectselector", "select", array("atktarget" => atkurlencode($targeturl_phase))), text("searchallprojectsphases"), SESSION_NESTED,$this->m_nested);

    // dirty trick: the dummy attribute which starts the javascript for the 
    // correct activities must be notified of the currentphase.
    $GLOBALS["currentphase"]=$currentphase;    
    $GLOBALS["currentactivity"]=$currentactivity;
    
    return $result;
  }
  
  function display($rec)
  {    
    return $rec[$this->fieldName()]["projname"]." - ".$rec[$this->fieldName()]["phasename"];
  } 

  /**
   * We don't support searching 
   * @param $record array with fields
   * @return search field
   */
  function search($record="")
  {
    return "&nbsp;";
  }
  
  function db2value($rec)
  {
    return Array("projname"=>$rec["proj_name"],
                 "phaseid"=>$rec["phaseid"],
                 "phasename"=>$rec["phase_name"],
                 "activityname"=>$rec["activity_name"]);
  }  
  
  function value2db($rec)
  {
    return $rec[$this->fieldName()]["phaseid"];
  }
 
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
    if ($mode == "add" || $mode == "update")
    {
      $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));  
    }
    else    
    {      
      $query->addField($this->fieldName(),"","hours",$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      $query->addField("name","", "B", "proj_", false);
      $query->addField("name","", "A", "phase_", false);
      $query->addField("name","", "C", "activity_", false);
      $query->addJoin("activity", "C", "C.id=hours.activityid", false);       
      $query->addJoin("phase", "A","A.id=hours.phaseid", true);      
      $query->addJoin("project", "B","B.id=A.projectid", true);      
    }

    
  } 
  
  function getOrderByStatement()
  {
    return "B.name, A.name";
  }
        
  function getRecentPhases()   
  {
    global $g_securityManager, $ATK_VARS, $g_sessionManager;
    
    $user = getUser();

    $reportnodetype = $g_sessionManager->stackVar("reportnodetype");
    
    if (strstr($ATK_VARS["atknodetype"],"reports") || $reportnodetype=="hoursurvey")
      $projectPhaseFilter = "phase.status<>'archived' AND project.status<>'archived'";
    else
      $projectPhaseFilter = "phase.status='active' AND project.status='active'";

    $query = "SELECT DISTINCT phase.id, phase.name, project.name as projectname FROM phase,hours,project LEFT JOIN project_person ON (project_person.projectid = project.id)";
    $query.= "WHERE phase.id=hours.phaseid AND userid='".$user["id"]."' AND phase.status='active' AND project.status='active' AND phase.projectid=project.id ";
    
    if (!$g_securityManager->allowed("timereg.hours", "any_project"))
    {
      $query.= "AND (project_person.personid = ".$user["id"]." OR project.timereg_limit = ".PRJ_TIMEREG_ALL_USERS.")";
    }
    
    // 100 should be enough to get 5 unique recent projects.
    $query.= "ORDER BY hours.entrydate DESC";
    
    $db = &atkGetDb();
    $arr = $db->getrows($query,'0','100');
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
        $newarr[$j]['projectname'] = $arr[$i]['projectname']; //update because of phases
        $j++;
      }
    }
    return $newarr;
  }
  
  function getPhase($phaseid)
  {
    $db = &atkGetDb();
    
    $query = "SELECT phase.id, phase.name, project.name as projectname FROM project,phase ";
    $query.= "WHERE phase.projectid=project.id AND phase.id = $phaseid";            
    
    $arr = $db->getrows($query);
    
    $phase = $arr[0];  
    
    return $phase;
  }
  
  function getActivities($phaseid_arr) 
  {
    $db = &atkGetDb();
    
    $query = "SELECT phase_activity.phaseid, id, name, description, remarkrequired FROM activity, phase_activity ";
    $query.= "WHERE phase_activity.phaseid IN (".implode(",",$phaseid_arr).") AND activity.id = phase_activity.activityid ";
    $query.= "ORDER BY name";
  
    return $db->getrows($query);
  }
  
  function javascript($phasearray)
  {
    $result = '<script type="text/javascript" language="javascript">';
    $result.= 'if (parseInt(navigator.appVersion)>=3) version="n3"; else version="n2";
     
               function entry(activityid,activityname) { this.actid=activityid; this.actname=activityname; }
               activityarray = new Array();
               Array.prototype.count=0;
               Array.prototype.id=0;';

  
    reset($phasearray);

    $i=0;

    while (list($phaseid, $phaseinfoarray) = each($phasearray))
    {
      $result.='activityarray['.$i."] = new Array();\n";
      $result.='activityarray['.$i.'].id ='.$phaseid.";\n";

      for ( $j=0; $j<count($phaseinfoarray); $j++)
      {
        $descrip = $phaseinfoarray[$j]['name'];
        if ($phaseinfoarray[$j]['description']!="")
        {
          $descrip.=" (".$phaseinfoarray[$j]['description'].")";
        }
        $result.= 'activityarray['.$i.']['.$j.'] = new entry('.$phaseinfoarray[$j]['id'];
        $result.= ',"'.addslashes($descrip)."\");\n";
      }
      $result.= 'activityarray['.$i.'].count = ' .count($phaseinfoarray).";\n";
      $i++;
    }
    
    $result.='function set_activities(projectid, selectedactivityid)
              {
                if(version=="n3")
                {
                  // Find project.
                  var projectindex=0;
                  for(var i=0;i<activityarray.length;i++)
                  {
                    if (activityarray[i].id == projectid) projectindex = i;
                  }

                  with (document.entryform.activityid)
                  {
                    for (var i = options.length; i >= 0; i--) options[i] = null;

                    var somethingselected=false;

                    if (projectid!=0)
                    {

                      for(var i=0;i<activityarray[projectindex].count;i++)
                      {
                        options[i] = new Option(activityarray[projectindex][i].actname,activityarray[projectindex][i].actid);
                        if (activityarray[projectindex][i].actid == selectedactivityid)
                        {
                          options[i].selected=true;
                          document.entryform.activityid.options.selectedIndex=i;
                          somethingselected = true;
                        }
                      }
                    }
                    else
                    {
                      options[0] = new Option("'.text("firstselectproject").'",0);
                    }
                    if (!somethingselected) options[0].selected=true;
                  }
                }
              }
              
              </script>';
    return $result;
  }
  
  function dbFieldType()
  {
    return "number";
  }
  
  function equal($recA, $recB)
  {
    return $recA["phaseid"]["phaseid"]==$recB["phaseid"]["phaseid"];
  }
}
?>