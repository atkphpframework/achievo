<?php
  function employee_time_format($time)
  {
    return floor($time/60).":".sprintf("%02d",($time%60));
  }

  global $g_layout;
  global $g_securityManager;
  global $startdate;
  global $enddate;
  global $g_user;  
  global $g_db; 
  global $userid;
  global $config_currency_symbol;
  
  function get_employees($user_id)
  {
    global $g_db;
  
    $sql = "SELECT name,userid 
            FROM employee
            WHERE status='active'
            ORDER BY name            
           ";

    $records = $g_db->getrows($sql);
    $employee_code='<OPTION VALUE="all">'.text("allusers");
    for($i=0;$i<count($records);$i++)
    { 
      if($user_id==$records[$i]["userid"]) { $sel="SELECTED"; } else { $sel=""; }
      $employee_code.='<OPTION VALUE="'.$records[$i]["userid"].'" '.$sel.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $employee_code;
  
  }
  
  if (is_array($startdate)) 
  {
    $startdate = $startdate["year"]."-".sprintf("%02d",$startdate["month"])."-".sprintf("%02d",$startdate["day"]);
  }
  else
  {
    if (!isset($startdate)||$startdate=="")
    {    
      $startdate =  date("Y-m-d" ,time()-(86400*7));
    }
  }
  if (is_array($enddate)) 
  {
    $enddate = $enddate["year"]."-".sprintf("%02d",$enddate["month"])."-".sprintf("%02d",$enddate["day"]);  
  }
  else
  {
    if (!isset($enddate)||$enddate=="")
    {
      $enddate = date('Y-m-d');
    }
  }
  
  if (!isset($userid)||$userid=="")
  {
    $userid=$g_user["name"];
  }
  
  $view_all = $g_securityManager->allowed('employee','view_all');
   
  $g_layout->ui_top(text("title_hours_stats")); 

  $g_layout->output('<form action="dispatch.php" method="get">');
  $g_layout->output('<input type="hidden" name="atknodetype" value="employee">');
  $g_layout->output('<input type="hidden" name="atkaction" value="'.$this->m_action.'">');
  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->td('<b>'.text('sethoursfilter').'</b>', 'colspan="2"');
  $g_layout->output('</tr><tr>');  
  
    // we have to pass a 'dummy' record to the attributes to set their default value.
  $dummyrec = Array("startdate"=>array("year"=>substr($startdate,0,4),
                                       "month"=>substr($startdate,5,2),
                                       "day"=>substr($startdate,8,2)),
                    "enddate"=>array("year"=>substr($enddate,0,4),
                                     "month"=>substr($enddate,5,2),
                                     "day"=>substr($enddate,8,2)));
                                                   
  $g_layout->td(text("name").':</b> ');   
  
  if($view_all)
  {
    $g_layout->td('<SELECT name="userid">'.get_employees($userid).'</SELECT>');
  }
  else
  {
    $g_layout->td($g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["name"].'">');
  }
  $g_layout->output('</tr><tr>');  
     
  $g_layout->td(text("timespan").': ');
  
  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));
      
  $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                  text("until").
                  '&nbsp; '.$enddateatt->edit($dummyrec));
                                
  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');
  
  $g_layout->ui_bottom();
      
  $query = "SELECT 
              hours.time as time,              
              project.name as project,
              activity.name as activity
            FROM 
              hours, activity, phase, project
            WHERE
              hours.activitydate >= '$startdate'
              AND hours.activitydate <= '$enddate'
              AND hours.activityid = activity.id
              AND hours.phaseid = phase.id
              AND phase.projectid = project.id";
  if ($userid!="all") $query.= " AND hours.userid = '$userid'";
  
  $records = $g_db->getrows($query);
  
  $hoursperact = Array();
  $hoursperproj = Array();
  $total = 0;
  
  for ($i=0;$i<count($records);$i++)
  {
    $hoursperact[$records[$i]["activity"]] += $records[$i]["time"];
    $hoursperproj[$records[$i]["project"]] += $records[$i]["time"];
    $total += $records[$i]["time"];
  }
 
  $g_layout->ui_top(text("title_employee_stats")." - ".$userid); 

  $g_layout->output('<br><b>'.text("title_employeestats_perproject").'</b><br><br>');
  
  $g_layout->output($g_layout->data_top());
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("name"));
  $g_layout->td_datatitle(text("time"));
  $g_layout->td_datatitle(text("percentage"));  
  $g_layout->output($g_layout->tr_bottom());
  
  while (list($projname, $time) = each ($hoursperproj))
  {    
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($projname);
    $g_layout->td(employee_time_format($time));    
    $percentage = round(($time/$total)*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(employee_time_format($total));
  $g_layout->td_datatitle('&nbsp;');  
  $g_layout->output($g_layout->tr_bottom());
  
  $g_layout->output($g_layout->data_bottom());

  $g_layout->output('<br><b>'.text("title_employeestats_peractivity").'</b><br><br>');
  
  $g_layout->output($g_layout->data_top());
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("name"));
  $g_layout->td_datatitle(text("time"));
  $g_layout->td_datatitle(text("percentage"));  
  $g_layout->output($g_layout->tr_bottom());
  
  while (list($actname, $time) = each ($hoursperact))
  {    
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($actname);
    $g_layout->td(employee_time_format($time));        
    $percentage = round(($time/$total)*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(employee_time_format($total));
  $g_layout->td_datatitle('&nbsp;');  
  $g_layout->output($g_layout->tr_bottom());
  
  $g_layout->output($g_layout->data_bottom());

//costregistration statistics
/*
  $query = "SELECT
              costregistration.value as costs,
              currency.symbol as cur,
              currency.value as value,
              project.name as project
            FROM 
              costregistration LEFT JOIN currency ON costregistration.currency = currency.symbol, project
            WHERE
              costregistration.costdate >= '$startdate'
              AND costregistration.costdate <= '$enddate'
              AND costregistration.projectid = project.id";
            if ($userid!="all") $query.= " AND costregistration.userid = '$userid'";

  $rec = $g_db->getrows($query);
  
  $costsperemp = array();
  $totalcosts = 0;

  for ($x=0;$x<count($rec);$x++)
  {
    //Calculate the costs to the default currency.
   
   if ($rec[$x]["cur"] == "") {
     $defcur = $rec[$x]["costs"];
   }
   else {
     $defcur = $rec[$x]["costs"] / $rec[$x]["value"];
   }    
        
    $costsperemp[$rec[$x]["project"]] += $defcur;
    $totalcosts += $defcur;
  }

  //for ($i=0;$i<count($records);$i++)
  //{
  //  $hoursperact[$records[$i]["activity"]] += $records[$i]["time"];
  //  $hoursperproj[$records[$i]["project"]] += $records[$i]["time"];
  //  $total += $records[$i]["time"];
  //}

  $g_layout->output('<br><b>'.text("title_employeestats_costs").'</b><br><br>');
  
  $g_layout->output($g_layout->data_top());
  
  $g_layout->output($g_layout->tr_top());
  $g_layout->td_datatitle(text("name"));
  $g_layout->td_datatitle(text("costs"));
  $g_layout->td_datatitle(text("percentage"));
  $g_layout->output($g_layout->tr_bottom());
  
  while (list($project, $costs) = each ($costsperemp))
  {
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($project);
    $g_layout->td($config_currency_symbol . " " . round($costs, 2));         
    $percentage = round(($costs/$totalcosts)*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }
  
  $g_layout->output($g_layout->tr_top());
  $g_layout->td_datatitle(text("total"));  
  $g_layout->td_datatitle($config_currency_symbol . " " . round($totalcosts, 2));
  $g_layout->td_datatitle('&nbsp;');     
  $g_layout->output($g_layout->tr_bottom());
  
  $g_layout->output($g_layout->data_bottom());
*/
  $g_layout->output('<br><br><br>');  

  $g_layout->ui_bottom();
?>
