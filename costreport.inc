<?php

  global $g_layout;
  global $g_securityManager;
  //global $startdate;
  //global $enddate;
  global $g_user;
  global $atkorderby;
  global $g_db;
  global $projectid;
  global $selectedprojectid;
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
  if (!isset($outputType))
  {
    $outputType=0;   //default=standard
  }
  
//  global $col;
  
  $startdate = $this->m_postvars["startdate"];
  $enddate = $this->m_postvars["enddate"];
  $col = $this->m_postvars["col"];

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

  
  // Determine collumn suppresslist:
  $arr_col = array("costdate","description","projectid","paymethod","value", "currency");
 
  $suppress = array(); 
  if (!is_array($col)||count($col)==0)
  {
    // No collumns selected. Default->select all.
    for ($i=0;$i<count($arr_col);$i++)
    {
      $col[]=$arr_col[$i];
    }
  }

  for ($i=0;$i<count($arr_col);$i++)
  {
    if (!in_array($arr_col[$i],$col)) $suppress[]= $arr_col[$i];
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
  
  // If we came from 'search all projects', a selectedprojectid var is filled.
  if (isset($selectedprojectid)&&$selectedprojectid!="")
  {
    $projectid=$selectedprojectid;
  }
  
  $view_all = $g_securityManager->allowed('costregistration','view_all');
  
 if ($outputType==0)
  {  

    $g_layout->ui_top(text("title_costregistration_stats")); 

    $g_layout->output('<form action="dispatch.php" method="get" name="entryform">');    
        
    $g_layout->output('<input type="hidden" name="atknodetype" value="costregistration">');
    $g_layout->output('<input type="hidden" name="atkaction" value="'.$this->m_action.'">');
    $g_layout->table_simple();
    $g_layout->output('<tr>');
    $g_layout->td('<b>'.text('setcostsfilter').'</b>', 'colspan="2"');
    $g_layout->output('</tr><tr>');  
    $g_layout->td('<hr>','colspan="2"');
    $g_layout->output('</tr><tr>');
    $g_layout->td(text('project').':');
    $projectatt = new atkProjectAttribute('projectid');
    $projectatt->m_owner = "costreport";
    
    // we have to pass a 'dummy' record to the attributes to set their default value.
    $dummyrec = Array("projectid"=>$projectid,
                      "startdate"=>array("year"=>substr($startdate,0,4),
                                         "month"=>substr($startdate,5,2),
                                         "day"=>substr($startdate,8,2)),
                      "enddate"=>array("year"=>substr($enddate,0,4),
                                       "month"=>substr($enddate,5,2),
                                      "day"=>substr($enddate,8,2)),
                      "atkorderby"=>$atkorderby);                    
                                         
    $g_layout->td($projectatt->edit($dummyrec));
    $g_layout->output('</tr><tr>');
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
    $g_layout->td('<hr>','colspan="2"');
    $g_layout->output('</tr><tr>');
     
    $g_layout->td(text("timespan").': ');
  
    $startdateatt = new atkDateAttribute("startdate","F d Y","d F Y", 0, date("Ymd"));
    $enddateatt = new atkDateAttribute("enddate","F d Y","d F Y", 0, date("Ymd"));
      
    $g_layout->td($startdateatt->edit($dummyrec).' &nbsp;'.
                    text("until").
                    '&nbsp; '.$enddateatt->edit($dummyrec));
                    
    $g_layout->output('</tr><tr>');  
    $g_layout->td('<hr>','colspan="2"');
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

    //$g_layout->td(text("PDF"));
    //$g_layout->td('<input type="checkbox" name="pdf" value="1">');
    //$g_layout->output('</tr><tr>');  

    $g_layout->output('</tr><tr>');
     
    $g_layout->td(text("orderby").': ');
    
    $listcol = array();
    for ($i=0;$i<count($arr_col);$i++) $listcol[] = "costregistration.".$arr_col[$i];
  
    $orderatt = new atkListAttribute("atkorderby",$arr_col, $listcol);
          
    $g_layout->td($orderatt->edit($dummyrec));
    
    $g_layout->output('</tr></table><input type="submit" value="'.text("refresh").'"></form><br>');
   
  }
  
  // Create Filter
  $filter="costdate>='".$startdate."' AND costdate<='".$enddate."'";
  if(!empty($projectid)&&$projectid!="") $filter.=" AND costregistration.projectid=".$projectid;
   
  if (!$view_all)
  {
    $filter.=" AND costregistration.userid='".$g_user["name"]."'";    
  }
  else
  {
    if ($userid!="all")
    {
      $filter.=" AND costregistration.userid='".$userid."'";
    }
  }  
  
   $data = $this->selectDb($filter,$atkorderby);

  //var_dump($data[0]);
  if ($outputType=="0")
  {
    $g_layout->ui_bottom();
    $g_layout->ui_top(text("title_costregistration_costs")." ".text("of")." ".$userid." ".text("report_intimespan")." ".$startdate." t/m ".$enddate);
    $g_layout->output('<br><br>');
    $g_layout->output($this->recordList($data, array(""),true,$suppress));
    $g_layout->output('<br><br>');
    $g_layout->ui_bottom(); 
   }
  if ($outputType=="1")
    { 
    $g_layout->output($this->specialRecordList($data,"<tr>","<td >","</td>","</tr>", "0",$suppress));
    }
  if ($outputType=="2")
  {
   // specail RecordList can export to file
    $this->specialRecordList($data, "", "", ";", "\r\n", "1", $suppress);
  }

?>