<?php

  /**
   * Add a selected project to the session variable.
   * If the session variable is empty, it is filled with
   * the projects the user has recently booked on.
   */

  $projectnamesize = 25;

  function updateSelectedProjects()
  {
    GLOBAL $g_sessionManager, $g_db, $g_layout;

     //global $g_sessionData;
     //atk_var_dump($g_sessionData);

    atkdebug("updateSelectedProject");
    $prj = array();
    //Get current sessionvar
    $prj =  $g_sessionManager->getValue("selectedprj","globals");

    //If session var is empty fill with recent projects
    if (count($prj) == 0)
    {
      $prj = getRecentProjects();
    }

    //Delete atkselector from global vars, because the stackVar
    //function overwrites the stack val of atkselector
    //when it finds a value in $GLOBALS
    $GLOBALS['atkselector'] = "";

 //Check if selected project already exists in session var
    $atkselect = $g_sessionManager->stackVar("atkselector");
    if (($atkselect != "") && isset($atkselect))
    {
       $projectid = sscanf($atkselect, "project.id=\'%d\'");
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
         if (strlen($arr[0]['name']) > $projectnamesize)
         {
           $selectedPrj['projectname'] = substr($arr[0]['name'],0,$projectnamesize-2).".."; //update because of phases
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
    global $g_db, $g_user, $projectnamesize;

    $query = "SELECT DISTINCT project.name as projectname, project.id as projectid FROM project,phase,hours ";
    $query.= "WHERE phase.id=hours.phaseid AND userid='".$g_user["name"]."' AND phase.status='active' AND project.status='active' AND phase.projectid=project.id ";
    // 100 should be enough to get 5 unique recent projects.
    $query.= "ORDER BY hours.entrydate DESC";

    $arr = $g_db->getrows($query,'0','100');
    $j=0;
    $alreadyhad = Array();
    $newarr = Array();
    for ($i=0;$i<count($arr)&&$j<$GLOBALS["config_numberofrecentprojects"];$i++)
    {
      if (!in_array($arr[$i]['projectid'],$alreadyhad))
      {
        $alreadyhad[$j]=$arr[$i]['projectid'];
        $newarr[$j]['projectid'] = $arr[$i]['projectid'];

        if (strlen($arr[$i]['name']) > $projectnamesize)
        {
          $newarr[$j]['name'] = substr($arr[$i]['name'],0,18)."..";
        }
        else
        {
          $newarr[$j]['name'] = $arr[$i]['name'];
        }
        if (strlen($arr[$i]['projectname']) > $projectnamesize)
        {
          $newarr[$j]['projectname'] = substr($arr[$i]['projectname'],0,18).".."; //update because of phases
          atkdebug("If ok ".strlen($newarr[$j]['projectname']));

        }
        else
        {
          $newarr[$j]['projectname'] = $arr[$i]['projectname']; //update because of phases
        }
        atkdebug("projectname ".strlen($newarr[$j]['projectname']));
        $j++;
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
