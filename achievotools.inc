<?php

  /**
   * Add a selected project to the session variable.
   * If the session variable is empty, it is filled with
   * the projects the user has recently booked on.
   */

  global $g_projectnamesize; // must declare global. this piece of code might
                             // be executed somewhere in a local function scope.
  $g_projectnamesize = 25;

  function updateSelectedProjects()
  {
    global $g_sessionManager, $g_db, $g_layout, $g_projectnamesize;   

    atkdebug("updateSelectedProject");
    $prj = array();
    //Get current sessionvar
    $session = &atkSessionManager::getSession();
    $prj =  $g_sessionManager->getValue("selectedprj","globals");

    //If session var is empty fill with recent projects
    if (count($prj) == 0)
    {
      $prj = getRecentProjects();      
    }

    //Delete atkselector from global vars, because the stackVar
    //function overwrites the stack val of atkselector
    //when it finds a value in $GLOBALS
    //$_REQUEST['atkselector'] = "";

 //Check if selected project already exists in session var
    $atkselect = $g_sessionManager->stackVar("atkselector");
       
    if (($atkselect != "") && isset($atkselect))
    {
       $projectid = sscanf($atkselect, "project.id='%d'");
       $prjId = $projectid[0];
    }
    else
    {
        $atkselect = $g_sessionManager->stackVar("selectedprojectid");
        if (($atkselect != "") && isset($atkselect))
        {
          $prjId = $atkselect;
        }
        else
        {
          $prjId = 0;
        }
    }

    // When no project is selected in the dropdown list
    // and the user select's a project we need to check
    // if the project is in the list. If not
    // we need to get it's name, so it can be displayed
    // in the dropdown list
    if ($prjId != 0)
    {
       $found = FALSE;
       foreach( $prj as $cur)
       {
         if ($cur['projectid']  == $prjId) $found = TRUE;
       }
       if (!$found)  //add project to dropdown list
       {

         $selectedPrj = Array();
         $selectedPrj['projectid'] = $prjId;

         //Get project name
         $query = "SELECT name FROM project WHERE id='".$selectedPrj['projectid']."'";
         $arr = $g_db->getrows($query);
         $selectedPrj['projectname'] = $arr[0]['name'];
         if (strlen($arr[0]['name']) > $g_projectnamesize)
         {           
           $selectedPrj['projectname'] = substr($arr[0]['name'],0,$g_projectnamesize-2).".."; //update because of phases
         }
         else
         {         
           $selectedPrj['projectname'] = $arr[0]['name']; //update because of phases
         }

         //Check maximum number of projects in session var
         if (count($prj) >= atkconfig("numberofrecentprojects")) array_shift($prj);
         array_push($prj,$selectedPrj);

       }
         //Reload the menu frame by creating javascript
         $js = "<script language='JavaScript'> parent.menu.location=\"menu.php?atkmenutop=projectmanagement&selectedproject=".$prjId."\";</script>";
         $g_layout->output($js);
     }


     // Copy prj to session var
     // Use global session because menu.php uses
     // a different namespace
     $g_sessionManager->globalVar("selectedprj",$prj, true);

    //atk_var_dump($g_sessionData);
  }

 /**
  * Get the projects the user recently booked hours on
  * @return array with recently used projects
  *
  */

  function getRecentProjects()
  {
    global $g_db, $g_user, $g_projectnamesize;

    $query = "SELECT DISTINCT project.name as projectname, project.id as projectid FROM project,phase,hours ";
    $query.= "WHERE phase.id=hours.phaseid AND userid='".$g_user["id"]."' AND phase.status='active' AND project.status='active' AND phase.projectid=project.id ";
    // 100 should be enough to get 5 unique recent projects.
    $query.= "ORDER BY hours.entrydate DESC";

    $arr = $g_db->getrows($query,'0','100');
        
    $newarr = Array();
    
    // For some reason, the above query returns double results (even though I 
    // do a 'select distinct'!)
    // I fail to see why, but I'll work around it by checking for doubles.
    $alreadyhad = array();    
    for ($i=0, $_i=count($arr); $i<$_i ;$i++)
    {                    
      if (!in_array($arr[$i]['projectid'], $alreadyhad))
      {
        $alreadyhad[] = $arr[$i]['projectid'];        
        if (strlen($arr[$i]['projectname']) > $g_projectnamesize)
        {
          $arr[$i]['projectname'] = substr($arr[$i]['projectname'],0,$g_projectnamesize-2).".."; //update because of phases        
        } 
        $newarr[] = $arr[$i];
      }
    }
    return $newarr;
  }

  /*
   * given a timestamp, this function calculates the weekstamp, which is a concatenation
   * of the year and the ISO week number. (e.g. 200134)
   */
  function weekstamp($timestamp)
  {
    $year = strftime("%Y",$timestamp);
    $week = strftime("%V",$timestamp);
    $month = strftime("%m",$timestamp);
    if ($week==1 && $month == 12)
    {
      // tricky.. first week of next year starts this year, so key must be incremented.
      return ($year+1).$week;
    }
    return $year.$week;
  }

  /*
   * Given a date in the form "2001-03-25", this function returns the date of the first
   * day (monday) of that week.
   */
  function startOfWeek($date)
  {
    $viewtime = mktime(0,0,0,substr($date,5,2),substr($date,8,2),substr($date,0,4));
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;
    return date("Y-m-d" ,$viewtime-(86400*($weekday-1)));
  }

  /*
   * Given a date in the form "2001-03-25", this function returns the date of the last
   * day (friday) of that week.
   */
  function endOfWeek($date)
  {
    $viewtime = mktime(0,0,0,substr($date,5,2),substr($date,8,2),substr($date,0,4));
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;
    return date("Y-m-d" ,$viewtime-(86400*($weekday-7)));
  }

  function usermail($to, $subject, $body, $add_header="")
  {
    if (is_array($to) && count($to)>0)
    {
      $to = implode(",",$to);
    }
    if ($to!="" && $subject!="")
    {
      mail($to,"[achievo notice] ".$subject,$body,"From: achievo\n".$add_header);
      atkdebug("Sent mail to $to");
    }
    else
    {
      atkdebug("No mail sent, missing to or subject");
    }
  }

  function time_format($time, $displayzero=false)
  {
    if ($time==0)
    {
      if ($displayzero) return "00:00";
      return "&nbsp;";
    }
    return floor($time/60).":".sprintf("%02d",($time%60));
  }



?>
