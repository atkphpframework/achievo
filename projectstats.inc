<?php
  include_once("achievotools.inc");

  global $g_layout;  
  global $startdate;
  global $enddate;  
  global $g_db; 
  global $selectedprojectid;
  
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
  
  $g_layout->ui_top(text("title_projectstats")); 

  $g_layout->output('<form action="dispatch.php" method="get">');
  $g_layout->output('<input type="hidden" name="atknodetype" value="project">');
  $g_layout->output('<input type="hidden" name="atkaction" value="'.$this->m_action.'">');
  $g_layout->table_simple();
  $g_layout->output('<tr>');
  $g_layout->td('<b>'.text('sethoursfilter').'</b>', 'colspan="2"');
  
  $g_layout->td('<input type="hidden" name="selectedprojectid" value="'.$selectedprojectid.'">');
  
  $g_layout->output('<tr></tr>');  
  
    // we have to pass a 'dummy' record to the attributes to set their default value.
  $dummyrec = Array("startdate"=>array("year"=>substr($startdate,0,4),
                                       "month"=>substr($startdate,5,2),
                                       "day"=>substr($startdate,8,2)),
                    "enddate"=>array("year"=>substr($enddate,0,4),
                                     "month"=>substr($enddate,5,2),
                                     "day"=>substr($enddate,8,2)));
                                                       
  $g_layout->td(text("timespan").': ');
  
  $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
  $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));
      
  $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                  text("until").
                  '&nbsp; '.$enddateatt->edit($dummyrec));
                                
  $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');
      
  $query = "SELECT 
              hours.time as time,              
              project.name as project,
              phase.name as phase,
              employee.name as employee,
              activity.name as activity
            FROM 
              hours, activity, phase, project, employee
            WHERE
              hours.activitydate >= '$startdate'
              AND hours.activitydate <= '$enddate'
              AND hours.activityid = activity.id
              AND hours.userid = employee.userid
              AND hours.phaseid = phase.id
              AND phase.projectid = project.id
              AND project.id = ".$selectedprojectid;                
  
  $records = $g_db->getrows($query);
  
  $hoursperact = array();
  $hoursperemp = array();
  $hoursperphase = array();
  
  $total = 0;
  
  // We display the projectname, which we can get from this query
  // (if there are no entries, we don't see a name. todo: fix this.
  $projname = $records[0]['project'];
  
  for ($i=0;$i<count($records);$i++)
  {
    $hoursperact[$records[$i]["activity"]] += $records[$i]["time"];
    $hoursperemp[$records[$i]["employee"]] += $records[$i]["time"];
    $hoursperphase[$records[$i]["phase"]] += $records[$i]["time"];
    $total += $records[$i]["time"];
  }
  
  $g_layout->ui_top(text("title_projectstats")." - ".$projname); 

  $g_layout->output('<br><b>'.text("title_projectstats_perphase").'</b><br><br>');
  
  $g_layout->output($g_layout->data_top());
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("name"));
  $g_layout->td_datatitle(text("time"));
  $g_layout->td_datatitle(text("percentage"));  
  $g_layout->output($g_layout->tr_bottom());
  
  while (list($phasename, $time) = each ($hoursperphase))
  {    
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($phasename);
    $g_layout->td(time_format($time));
    $percentage = round(($time/$total)*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(time_format($total));
  $g_layout->td_datatitle('&nbsp;');  
  $g_layout->output($g_layout->tr_bottom());
  
  $g_layout->output($g_layout->data_bottom());

  $g_layout->output('<br><b>'.text("title_projectstats_peremployee").'</b><br><br>');    
  
  $g_layout->output($g_layout->data_top());
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("name"));
  $g_layout->td_datatitle(text("time"));
  $g_layout->td_datatitle(text("percentage"));  
  $g_layout->output($g_layout->tr_bottom());
  
  while (list($empname, $time) = each ($hoursperemp))
  {    
    $g_layout->output($g_layout->tr_top());
    $g_layout->td($empname);
    $g_layout->td(time_format($time));    
    $percentage = round(($time/$total)*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }
  
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(time_format($total));
  $g_layout->td_datatitle('&nbsp;');  
  $g_layout->output($g_layout->tr_bottom());
  
  $g_layout->output($g_layout->data_bottom());

  $g_layout->output('<br><b>'.text("title_projectstats_peractivity") . '</b><br><br>');

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
    $g_layout->td(time_format($time));        
    $percentage = round(($time/$total)*100);
    $imgwidth = max(1,($percentage*3));
    $g_layout->td('<img src="images/stats_red50.gif" width="'.$imgwidth.'" height="20"> &nbsp;('.$percentage.'%)');
    $g_layout->output($g_layout->tr_bottom());
  }
   
  $g_layout->output($g_layout->tr_top());  
  $g_layout->td_datatitle(text("total"));
  $g_layout->td_datatitle(time_format($total));
  $g_layout->td_datatitle('&nbsp;');  
  $g_layout->output($g_layout->tr_bottom());
  
  $g_layout->output($g_layout->data_bottom());
  
  $g_layout->output('<br><br><br>');  

  $g_layout->ui_bottom();
?>
