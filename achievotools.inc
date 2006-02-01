<?php

  /**
   * Add a selected project to the session variable.
   * If the session variable is empty, it is filled with
   * the projects the user has recently booked on.
   */

  global $g_projectnamesize; // must declare global. this piece of code might
                             // be executed somewhere in a local function scope.
  $g_projectnamesize = 20;

  function updateSelectedProjects()
  {
    global $g_sessionManager, $g_projectnamesize;

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
         $db = &atkGetDb();
         $query = "SELECT name FROM project WHERE id='".$selectedPrj['projectid']."'";
         $arr = $db->getrows($query);
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
         if (count($prj) >= atkconfig("numberofrecentprojects"))
         {
           array_shift($prj); // remove the first
           $prj[count($prj)-1] = $selectedPrj;
         }
         else
         {
           $prj[] = $selectedPrj;
         }

       }

       //Reload the menu frame by creating javascript
       $script = "var newlocation=\"menu.php?atkmenutop=projectmanagement&selectedproject=".$prjId."\";
                  var oldlocation=parent.menu.location.toString();
                  if (oldlocation.indexOf(newlocation) == -1) {parent.menu.location = newlocation;}";
       $page = &atkinstance("atk.ui.atkpage");
       $page->register_loadScript($script);
     }

     // Copy prj to session var
     // Use global session because menu.php uses
     // a different namespace
     $g_sessionManager->globalVar("selectedprj", $prj, true);
  }

 /**
  * Get the projects the user recently booked hours on
  * @return array with recently used projects
  *
  */

  function getRecentProjects()
  {
    global $g_user, $g_projectnamesize;

    $db = &atkGetDb();

    $query = "SELECT DISTINCT project.name as projectname, project.id as projectid FROM project,phase,hours ";
    $query.= "WHERE phase.id=hours.phaseid AND userid='".$g_user["id"]."' AND phase.status='active' AND project.status='active' AND phase.projectid=project.id ";
    $query.= "ORDER BY projectname";

    $arr = $db->getrows($query,'0','100');

    $newarr = Array();

    // For some reason, the above query returns double results (even though I
    // do a 'select distinct'!)
    // I fail to see why, but I'll work around it by checking for doubles.
    $alreadyhad = array();
    for ($i=0, $_i=count($arr); $i<$_i;$i++)
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
  function weekstamp($timestamp)  // in achievotools.inc
  {
    /*
     * JN 02-06-26: FIX: Time locks based on week numbers not working on win32.
     *                   Reason is PHP: not all strftime functions are working on win32.
     *                   Solution: We have to calculate week of year manually.
     */

    $year = strftime("%Y",$timestamp);
    $yearStart = mktime(0,0,0,1,1,$year);
    $yearEnd = mktime(0,0,0,12,31,$year);

    $daysLost = strftime("%w",$yearStart);
    if ($daysLost > 4) {
      $daysLost -= 9;
    } else {
      $daysLost -= 2;
    }
    // $week = bcdiv(strftime("%j",$timestamp)+$daysLost+7,7,0);
    $week = "".floor((strftime("%j",$timestamp)+$daysLost+7)/7);

    /*
    atkdebug("(weekstamp): date: ".strftime("%a %d %m %Y",$timestamp).
             " week: ".$week.
             " day: ".strftime("%j",$timestamp).
             " jan1: ".strftime("%a",$yearStart).
             " dec31: ".strftime("%a",$yearEnd).
             " w1dLastY: ".($daysLost+1));
    */

    if ($week == 0) {
      //atkdebug("(weekstamp): adjust to previous year");
      return weekstamp(mktime(0,0,0,12,31,($year-1)));
    }

    if ($week == 53)
    {
      if ((strftime("%w", $yearEnd) >= 1) && (strftime("%w", $yearEnd)<= 3))
      {
        //atkdebug("(weekstamp): adjust to following year");
        return ($year+1)."01";
      }
    }

    return $year.sprintf("%02d", $week);
  }

  /*
   * Given a date in the form "2001-03-25", this function returns the date of the first
   * day (monday) of that week.
   * @deprecated use dateUtil::startOfWeek instead (note that it has a slightly different format)
   */
  function startOfWeek($date)
  {
    $viewtime = adodb_mktime(0,0,0,substr($date,5,2),substr($date,8,2),substr($date,0,4));
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;
    return date("Y-m-d" ,$viewtime-(86400*($weekday-1)));
  }

  /*
   * Given a date in the form "2001-03-25", this function returns the date of the last
   * day (friday) of that week.
   * @deprecated use dateUtil::endOfWeek instead (note that it has a slightly different format)
   */
  function endOfWeek($date)
  {
    $viewtime = adodb_mktime(0,0,0,substr($date,5,2),substr($date,8,2),substr($date,0,4));
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;
    return date("Y-m-d" ,$viewtime-(86400*($weekday-7)));
  }

  /**
   * Generic achievo mail function used when sending mails
   *
   * @param string/array $to Recipients e-mail address(es) (may not be empty, otherwise usermail fails and returns false)
   * @param string $subject Subject to be used when sending the mail (will be prefixed by [achievo notice], but may not be empty, otherwise usermail fails and returns false)
   * @param string $body Mail content
   * @param string $add_header SMTP Mail headers to be added to the mail
   * @return boolean True if succesfull, false if not
   */
  function usermail($to, $subject, $body, $add_header="")
  {
    // Determine the sender's name by using the mail_sender configuration variable
    $sender = atkconfig("mail_sender", "achievo");

    // If multiple recipients in array, then convert the recipients array to a comma seperated string
    if (is_array($to) && count($to)>0)
    {
      $to = implode(",", $to);
    }

    // If no recipient or subject found, dont attempt to sent
    if (($to=="") || ($subject==""))
    {
      atkdebug("No mail sent, missing to or subject");
      return false;
    }

    // Attempt to send the mail, and store the result in $result
    $result = mail($to, "[achievo notice] " . $subject, $body, "From: " . $sender . "\r\n" . $add_header);

    // Show debug info
    atkdebug($result ? "Sent mail to $to" : "Sending of mail to $to failed");

    // Use the mail result as return value
    return $result;
  }

  function time_format($time, $displayzero=false)
  {
    if ($time==0)
    {
      if ($displayzero) return "00:00";
      return "&nbsp;";
    }
    return ($time<0?"-":"").floor(abs($time)/60).":".sprintf("%02d",(abs($time)%60));
  }



?>
