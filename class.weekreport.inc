<?php

include_once("achievotools.inc");

class timeDisplayAttribute extends atkAttribute
{
  function display($rec)
  {
    $time = $rec[$this->fieldName()];
    if ($time==0) return "";
    return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);  
  }
}

function status_cmp($a, $b)
{
  if ($a["status"]==$b["status"])
  {
    if ($a["week"]==$b["week"]) return 0;
    return ($a["week"] < $b["week"]) ? -1 : 1;
  }
  return ($a["status"] < $b["status"]) ? -1 : 1;
}

function week_cmp($a, $b)
{  
  if ($a["week"]==$b["week"]) return 0;
  return ($a["week"] < $b["week"]) ? -1 : 1; 
}




class weekreport extends atkNode
{

  function weekreport()
  {              
    $this->atkNode("weekreport",NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT); // node() constructor is *not* called automatically!
    $this->addAttribute(new atkAttribute("week"));
    $this->addAttribute(new atkAttribute("startdate"));
    $this->addAttribute(new atkAttribute("enddate"));
    $this->addAttribute(new timeDisplayAttribute("mon"));
    $this->addAttribute(new timeDisplayAttribute("tue"));
    $this->addAttribute(new timeDisplayAttribute("wed"));
    $this->addAttribute(new timeDisplayAttribute("thu"));
    $this->addAttribute(new timeDisplayAttribute("fri"));
    $this->addAttribute(new timeDisplayAttribute("sat"));
    $this->addAttribute(new timeDisplayAttribute("sun"));
    $this->addAttribute(new timeDisplayAttribute("total",AF_TOTAL));
    $this->addAttribute(new timeDisplayAttribute("contract",AF_TOTAL));
    $this->addAttribute(new timeDisplayAttribute("overtime",AF_TOTAL));
    $this->addAttribute(new atkAttribute("status"));      	
  }

  function get_employee($user_id)
  {
    global $g_db;  
    $sql = "SELECT * FROM employee WHERE status='active' AND userid='$user_id'";
    $record = $g_db->getrows($sql);
    return $record[0];
  }
		 
  function get_employees($userid)
  {
    global $g_db;
  
    $sql = "SELECT name,userid 
            FROM employee
            WHERE status='active'
            ORDER BY name
           ";

    $records = $g_db->getrows($sql);
    for($i=0;$i<count($records);$i++)
    { 
      if ($userid==$records[$i]["userid"]) $selected="selected"; 
      else $selected="";
      $employee_code.='<OPTION VALUE="'.$records[$i]["userid"].'" '.$selected.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $employee_code;
  }
  

  function action_report()
	{
    global $g_layout;
    global $g_securityManager;

    global $g_user;
    global $atkorderby;
    global $g_db;    
    global $userid;
    
//  global $printable;
//  global $export;
  
    /** $outputType gives control over the kind of output: 
    *        0 standard(onscreen in compliance with theme, default) 
    *        1 printable (onscreen in a format that easily can be printed on paper))
    *             [was $printable]
    *        2 export ( to a file on local computer ex. achievo.csv)
    */
    global $outputType;
    if (!isset($outputType)||$outputType=="") $outputType=0; 
    
    if (!isset($atkorderby)||$atkorderby=="") $atkorderby = "week";
  

    $view_all = $g_securityManager->allowed('weekreport','view_all');
  
    $startdate = $this->m_postvars["startdate"];
    $enddate = $this->m_postvars["enddate"];
  
    $col = $this->m_postvars["col"];  

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
    
    if ($outputType==0)
    {  
      $g_layout->ui_top(text("title_employee_weekreport")); 
  
      $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');    
          
      $g_layout->output('<input type="hidden" name="atknodetype" value="weekreport">');
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
        $g_layout->td('<SELECT name="userid">'.$this->get_employees($userid).'</SELECT>');
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
                        
      $g_layout->output('</tr><tr>');
              
      $g_layout->td('<b>'.text("report_output_options").'</b>','colspan="2"');
     
      $g_layout->output('</tr><tr>');      

      $g_layout->td(text("report_output_type"));
      $g_layout->td('<select name="outputType">
                     <option VALUE="0" selected>'.text("report_output_standard").'
                     <option value="1">'.text("report_output_printable").'
                     <option value="2">'.text("report_output_export").'
                     </select>');                     
        
      $g_layout->output('</tr><tr>');                   
                 
      $g_layout->td(text("orderby").': ');
    
      $g_layout->td('<select name="atkorderby">
                     <option VALUE="week" '.($atkorderby=="week"?"selected":"").'>'.text("week").'
                     <option VALUE="status" '.($atkorderby=="status"?"selected":"").'>'.text("status").'                     
                     </select>');                     
      
    
    
      $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');
   
    }
    
    $weekdata = array();    
    
    // first fill the weeks and start/end dates..
    $startstamp = mktime(0,0,0,substr($startdate,5,2),
                               substr($startdate,8,2),
                               substr($startdate,0,4));
    $endstamp = mktime(0,0,0,substr($enddate,5,2),          
                             substr($enddate,8,2),
                             substr($enddate,0,4));
                             
    $nrofdays = ceil(($endstamp-$startstamp)/(24*60*60));
                             
    for ($i=0;$i<=$nrofdays;$i++)
    {  
      $curstamp = $startstamp + ((24*60*60)*$i);
      if (strftime("%w",$curstamp)==1)
      {
        $key = weekstamp($curstamp);      
        $weekdata[$key]["week"] = $key;
        $weekdata[$key]["startdate"] = strftime("%d-%m-%Y",$curstamp);
        $weekdata[$key]["enddate"] = strftime("%d-%m-%Y",$curstamp+(6*24*60*60));
        $i+=6;
      }
    }
    
    $report_start_week = weekstamp($startstamp);      
    $report_end_week = weekstamp($endstamp);      
       
    // determine contracthours per week..
    $query = "SELECT
                uc_hours, startdate, enddate
              FROM
                 usercontract
              WHERE
                userid ='$userid'
                AND startdate <= '$enddate'
                AND enddate >= '$startdate'
              ORDER BY
                startdate";
    $contracts = $g_db->getrows($query);
        
    for ($i=0;$i<count($contracts);$i++)
    {
      $start_week = weekstamp(mktime(0,0,0,substr($contracts[$i]["startdate"],5,2),
                                             substr($contracts[$i]["startdate"],8,2),
                                             substr($contracts[$i]["startdate"],0,4)));
      $end_week = weekstamp(mktime(0,0,0,substr($contracts[$i]["enddate"],5,2),
                                           substr($contracts[$i]["enddate"],8,2),
                                           substr($contracts[$i]["enddate"],0,4)));
      for ($j=$start_week;$j<=$end_week;$j++)
      {
        if ($j>=$report_start_week && $j<=$report_end_week && substr($j,4)<=52)
        {
          $weekdata[$j]["week"] = $j;
          $weekdata[$j]["contract"] = $contracts[$i]["uc_hours"]*60;             
        }
      }
    }
   
    // collect data..
    $query = "SELECT  
                sum(time) as time, activitydate
              FROM
                hours
              WHERE             
                userid = '$userid'
              AND
                activitydate BETWEEN '$startdate' AND '$enddate'
              GROUP BY
                activitydate";
                
    $raw = $g_db->getrows($query);
    
    $strdow = array("sun","mon","tue","wed","thu","fri","sat");
    
    for ($i=0;$i<count($raw);$i++)
    {
      $stamp = mktime(0,0,0,substr($raw[$i]["activitydate"],5,2),
                            substr($raw[$i]["activitydate"],8,2),
                            substr($raw[$i]["activitydate"],0,4));
      $weeknr = strftime("%V",$stamp);
      $dow = strftime("%w",$stamp);
      $key = weekstamp($stamp);
      $weekdata[$key]["week"] = $key;
      $weekdata[$key][$strdow[$dow]] = $raw[$i]["time"];
      $weekdata[$key]["total"] = $weekdata[$key]["total"]+$raw[$i]["time"];
      
      // We need to display the first date and last date of each week. 
      // Since we are looping days now, it is easiest if we calculate the 
      // beginning of a week here.
    }
    
    // Collect locks..
    $query = "SELECT 
                week
              FROM 
                hours_lock
              WHERE
                (userid = '$userid' OR userid = '*')
              AND
                week between '$report_start_week' and '$report_end_week'";
    $locks = $g_db->getrows($query);
    for ($i=0;$i<count($locks);$i++)
    {
      $weekdata[$locks[$i]["week"]]["status"]=text("locked");
    }
    
    // weekdata is an associative array, indexed by weeknumber.
    // it must be converted to a normal array in order to be usable
    // by recordlist functions.
    // we also need to calculate some numbers.
    $rldata = array();
    foreach ($weekdata as $weeknr => $data)
    {
      $data["overtime"] = max(0,($data["total"]-$data["contract"]));
      $rldata[] = $data;
    }
    
    // sorting..
    if ($atkorderby=="status")
    {
      usort($rldata,"status_cmp");
    }
    else
    {
      usort($rldata,"week_cmp");
    }

    if ($outputType=="0")
    {
      $g_layout->ui_bottom();
      $g_layout->ui_top(text("title_weekreport")." ".text("of")." ".$userid." ".text("report_intimespan")." ".$startdate." t/m ".$enddate);
      
      $emprec = $this->get_employee($userid);      ;
      $employeedata = '<br>'.$g_layout->ret_table_simple().'<tr>';
                  
      $employeedata.= $g_layout->ret_td(text("name").": ");
      $employeedata.= $g_layout->ret_td($emprec["name"]);
      $employeedata.= '</tr><tr>';      
      $employeedata.= $g_layout->ret_td(text("email").": ");
      $employeedata.= $g_layout->ret_td($emprec["email"]);
      $employeedata.= '</tr></table>';
      
      $g_layout->output($employeedata);
      $g_layout->output('<br><br>');
      $g_layout->output($this->recordList($rldata, array(),false,$suppress));
      $g_layout->output('<br><br>');
      $g_layout->ui_bottom(); 
    }
    else if ($outputType=="1")
    { 
      $g_layout->output($this->specialRecordList($rldata,"<tr>","<td >","</td>","</tr>", "0",$suppress));
    }
    else if ($outputType=="2")
    {
      // specail RecordList can export to file
      $this->specialRecordList($rldata, "", "", ";", "\r\n", "1", $suppress);
    }  
  }
}

?>