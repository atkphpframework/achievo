<?php

class atkProjectPhaseAttribute extends atkAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function atkProjectPhaseAttribute($name, $flags=0)
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
    global $selectedphaseid, $atkaction, $atklevel;
    
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
      $phases[] = $phase;
    }    
    
    // Load Activities for each project
    for ($i=0;$i<count($phases);$i++)
    {
      $phasearray[$phases[$i]["id"]] = $this->getActivities($phases[$i]["id"]);
    }

    if($atkaction=="hoursurvey")
    {
      $result.= '<select name="phaseid">';   
    } 
    else
    {
      $result .= $this->javascript($phasearray);
      $result.= '<select name="phaseid" onChange="set_activities(this.options[this.selectedIndex].value,0)">';   
    }
    $result.= '<option value="">---';  
    
    for ($i=0;$i<count($phases);$i++)
    {
      $result .= '<option value="'.$phases[$i]["id"].'" ';
      
      // de juiste default selecteren
      if ($phases[$i]["id"]==$currentphase) $result.="selected";
      
      $result .= '>'.$phases[$i]["projectname"]." - ".$phases[$i]["name"];
    }
    
    $result .= '</select>&nbsp';
    
    $saveform = true; // keep form in session..
    if ($atkaction=="edit")
    {
      $targeturl_houradmin = dispatch_url("hours", "edit", array("atkselector" => 'hours.id='.$record['id'], "selectedphaseid" => "[id]", "atklevel"=>$atklevel));      
    }
    elseif($atkaction=="hoursurvey")
    {
      $targeturl_houradmin = dispatch_url("hours", "hoursurvey", array("selectedphaseid" => "[id]", "atklevel"=>$atklevel));
      $saveform = false; // hoursurvey doesn't support session save..
    }
    else
    {
      $targeturl_houradmin = dispatch_url("hours", "admin", array("selectedphaseid" => "[id]", "atklevel"=>$atklevel));
    }
    
    $targeturl_phase = dispatch_url("phase", "select", array("atkfilter" => "projectid=[id]", "atktarget" => atkurlencode($targeturl_houradmin)));
    $result .= href(dispatch_url("project", "select", array("atktarget" => atkurlencode($targeturl_phase))), text("houradmin_searchallprojectsphases"), SESSION_NESTED,$saveform);

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
    if ($mode == "select")
    {
      
      $query->addField($this->fieldName(),"","hours",$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      $query->addField("name","", "B", "proj_", false);
      $query->addField("name","", "A", "phase_", false);
      $query->addField("name","", "C", "activity_", false);
      $query->addJoin("activity", "C", "C.id=hours.activityid", false);       
      $query->addJoin("phase", "A","A.id=hours.phaseid", true);      
      $query->addJoin("project", "B","B.id=A.projectid", true);      

    }

    else if ($mode == "add" || $mode == "edit")
      $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
  } 
        
  function getRecentPhases()   
  {
    global $g_db, $g_user;
     
    $query = "SELECT phase.id, phase.name, project.name as projectname FROM project,phase,hours ";
    $query.= "WHERE phase.id=hours.phaseid AND userid='".$g_user["name"]."' AND phase.status='active' AND project.status='active' AND phase.projectid=project.id ";
    // 100 should be enough to get 5 unique recent projects.
    $query.= "ORDER BY hours.entrydate DESC";
    
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
        $newarr[$j]['projectname'] = $arr[$i]['projectname']; //update because of phases
        $j++;
      }
    }
    return $newarr;
  }
  
  function getPhase($phaseid)
  {
    global $g_db, $g_user;
     
    $query = "SELECT phase.id, phase.name, project.name as projectname FROM project,phase ";
    $query.= "WHERE phase.projectid=project.id AND phase.id = $phaseid";            
    
    $arr = $g_db->getrows($query);
    
    $phase = $arr[0];  
    
    return $phase;
  }
  
  function getActivities($phaseid) 
  {
    global $g_db;
    
    $query = "SELECT id, name, description, remarkrequired, billable FROM activity, phase_activity ";
    $query.= "WHERE phase_activity.phaseid = $phaseid AND activity.id = phase_activity.activityid ";
    $query.= "ORDER BY name";
  
    return $g_db->getrows($query);
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
        $result.= ',"'.$descrip."\");\n";
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
                      options[0] = new Option("'.text("houradmin_firstselectproject").'",0);
                    }
                    if (!somethingselected) options[0].selected=true;
                  }
                }
              }
              
              </script>';
    return $result;
  }
}
?>
