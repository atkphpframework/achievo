<?php
include_once("class.hours.inc");
useattrib("atkNumberAttribute");

class hours_approve extends hours
{
  // Dummy function
  function weekviewHeader() {}
  
  function hours_approve()
  {
    global $g_sessionManager;
    $g_sessionManager->pageVar("weekid");
    $g_sessionManager->pageVar("approved");
    $this->hours();
  }
  
  function convertheck()
  {
    global $g_db;
    
    $showtable = $g_db->getrows("SHOW TABLES LIKE 'hours_approved'");
    
    if (!empty($showtable[0]))
    {
      return href(dispatch_url("timereg.hours_approve","convert_old"),"Converteer oude gegevens");
    }
  }
  
  function action_admin(&$handler)
  {
    global $viewdate, $g_layout, $_POST, $ATK_VARS;
    
    $formbegin = '<form name="entryform" enctype="multipart/form-data" '."\n".
                 'action="dispatch.php" method="post" onsubmit="globalSubmit(this)">'."\n".session_form();
    $g_layout->output($formbegin);
    
    if ($viewdate) $output = $this->approvescreen();
    $output.= $this->approvedweekview();
    return $output;
  }  
  
  function approvescreen()
  {
    global $ATK_VARS;
    $this->m_viewdate = $ATK_VARS["viewdate"]; 
    $output.= $this->weekview();
    return $output;
  }
  
  function getWeekviewData($viewtime)
  {
    $output = parent::getWeekviewData($viewtime);
    if ($this->m_postvars['approved']==1)
      $output.= "<br />".atkButton(text("disapprove"),"dispatch.php?atkaction=disapprove");
    else if ($this->m_postvars['approved']==0)
      $output.= "<br />".atkButton(text("approve"),"dispatch.php?atkaction=approve");
      
    return $output;
  }
  
  function approvedweekview()
  {
    global $g_layout, $g_user, $g_db;
    
    if (is_numeric($this->m_postvars['year']) && strlen($this->m_postvars['year'])==4)
      $year = trim($this->m_postvars['year']);
    else 
     $year = date('Y');
    
    $yearattrib = new atkNumberAttribute("year",0,6);
    $g_layout->initGUI();
    $g_layout->ui_top(text('title_houradmin_approve'));

    $g_layout->output(text('year')." : ".$yearattrib->edit(array("year"=>$year)));
    $g_layout->output('<input type="submit" name="atkbtn1" value="'.text("view").'">');
    
    $formend = '</form>';
    $g_layout->output($formend);
    
    $g_layout->output($this->showEmployees($g_user['id'], $year));
    $g_layout->output('<br />'.text('hours_approve_color_explain'));
    $g_layout->output("<br /><br />".$this->convertheck());
    $g_layout->ui_bottom();
  }
  
  function getdatefromweek($yearweek)
  {
    $weekEndDay = 0;
      // 0 = Sunday, 1 = Monday etc.
    $year = (int) substr($yearweek, 0, 4);
    $week = (int) substr($yearweek, -2, 2);
    $dayOfYear = 4 + (($week - 1) * 7);
    $date = mktime(0, 0, 0, 1, $dayOfYear, $year);
    $dayOfWeek = date("w", $date);
    $daysToAdd = ($weekEndDay - $dayOfWeek + 7) % 7;
    $date += $daysToAdd * 24*60*60;
    return $date;
  }
    
  function showEmployees($userid, $year)
  {
    global $prevnextlinks, $g_db;

    $employeenode = new atkNode("tools");
    $employeenode = &getNode("employee.employee");
    if ($userid) $where = "person.supervisor = '$userid'";
    $data = $employeenode->selectDb($where, null, null, null, array('id', 'lastname', 'firstname', 'supervisor'));

    $users = array();
    foreach ($data as $key => $user)
    {
      $users[$key]["id"]   = $user["id"];
      $users[$key]["name"] = $user["firstname"].' '.$user["lastname"];
      $users[$key]["weeks"]=array();
      
      $houslocknode = &getNode("timereg.hours_lock");
      $lockedweeks = $houslocknode->selectDb("hours_lock.userid='".$user["id"]."'");
      
      foreach ($lockedweeks as $lockedweek)
      {
        if (substr($lockedweek["week"],0,4)==$year)
        {
          $users[$key]["weeks"][substr($lockedweek["week"],4)]["id"]= $lockedweek['id'];
          $users[$key]["weeks"][substr($lockedweek["week"],4)]["approved"] = $lockedweek["approved"];
        }
      }
      ksort($users[$key]["weeks"]);
    }
    return $this->makeLinks($users,$year);
  }
   
  function makeLinks($users, $year)
  {
    global $g_layout;
    
    $output.= $g_layout->ret_table_simple();
    
    $output.="<thead><tr><th>".text('user')."</th><th align='center'>".text('weeks')."</th></tr></thead>\n";
    
    foreach ($users as $user)
    {
      $output.= "\n<tr>";
      $output.= "<td>".$user["name"]."</td>\n";
      
      foreach ($user['weeks'] as $weeknumber => $weekdata)
      {
        if ($weeknumber<10) $prefix = 0;
        else $prefix = null;
        
        if ($weekdata["approved"]==1)
          $color = "#009900";
        else 
          $color = "#FF0000";

        $weeks.=href('dispatch.php?viewuser='.$user["id"].'&weekview=1&viewdate='.date("Y-m-d",$this->getdatefromweek($year.$prefix.$weeknumber))."&weekid=".$weekdata["id"]."&approved=".$weekdata["approved"],$weeknumber, SESSION_DEFAULT, false, 'style="color: '.$color.';"')."&nbsp;";
      }
      $output.= "<td>$weeks</td>\n";
      $output.= "</tr>";
      $weeks="";
    }
    $output.= "</table>";
    return $output;
  }
  
  function action_approve(&$handler)
  {
    $this->updateHoursLockApproval("1");
  }
  
  function action_disapprove(&$handler)
  {
    $this->updateHoursLockApproval("0");
  }
  
  function updateHoursLockApproval($approval)
  {
    global $ATK_VARS, $g_sessionManager;
    $viewdate = $this->m_postvars['viewdate'];
    
    $hourslocknode = &getNode("timereg.hours_lock");
    
    if (is_numeric($this->m_postvars["weekid"]))
    {
      $record = $hourslocknode->selectDb("hours_lock.id='".$this->m_postvars["weekid"]."'");
      
      $record[0]["approved"]=$approval;
      $hourslocknode->updateDb($record[0]);
    }
    
    $g_sessionManager->pageVar("viewdate","");
    $this->redirect();
  }
  
  function action_convert_old(&$handler)
  {
    global $g_db;
    $lockedhoursbyweek = array();
    $approvedhours = $g_db->getrows("SELECT * FROM hours_approved");
    $lockedhours = $g_db->getrows("SELECT * FROM hours_lock");
    
    foreach ($lockedhours as $lockedhour)
    {
      $lockedhoursbyweek[$lockedhour['week']][$lockedhour['userid']] = $lockedhour;
    }
    
    foreach ($approvedhours as $approvedhour)
    {
      if ($lockedhoursbyweek[$approvedhour['week']][$approvedhour['userid']])
      {
        $g_db->query("UPDATE `hours_lock` SET `approved` = '1' WHERE `id` = '".$lockedhoursbyweek[$approvedhour['week']][$approvedhour['userid']]['id']."'");
      }
      else 
      {
        $g_db->query("INSERT INTO `hours_lock` (`week`,`userid` ,`approved`) VALUES ('".$approvedhour['week']."', '".$approvedhour['userid']."', '1');");
      }
    }
    
    $g_db->query("DROP TABLE `hours_approved`");
  }
}

?>