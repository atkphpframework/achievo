<?php
// **********************************************************************
//  MILESTONES BEGIN 
// **********************************************************************

global $g_layout, $g_db, $selectedprojectid;

include_once("achievotools.inc");

function load_dependencies($phase_record,$dep_record)
{  
  for ($i=0,$_i=count($dep_record);$i<$_i;$i++)
  {          
    $before[] = $dep_record[$i]['first'];
    $after[] = $dep_record[$i]['last'];
  }
    
  return array("befores"=>$before, "afters"=>$after);
}

function init_milestones($phase_ids)
{
   $m[0]['in']  = array ();
   $m[0]['out'] = $phase_ids;
   $m[0]['deadline'] = 0;

   $m[1]['in']  = $phase_ids;
   $m[1]['out'] = array ();
   $m[1]['deadline'] = 0;

   return $m;
}
// **********************************************************************
//  FUNCTION MILESTONE DEADLINE CALC
// **********************************************************************
function calc_milestone_deadlines(&$m,$phase,$phase_maxtime)
{   
     for ($i=0,$_i=count($m);$i<$_i;$i++)
   {
      for ($j=0,$_j=count($phase);$j<$_j;$j++)
      {
         $out_m = get_current_milestone($m,$phase[$j],'out');
         $in_m  = get_current_milestone($m,$phase[$j],'in');
            
         if (($in_m == $i) &&
             ($m[$i]['deadline'] <  $m[$out_m]['deadline']+($phase_maxtime[$j]+0)))
         {    
            $m[$i]['deadline'] = $m[$out_m]['deadline']+($phase_maxtime[$j]+0);
         }   
      }     
   }     
}
// **********************************************************************
// ORDER FUNCTION
// **********************************************************************

function check_pos($thing,$order)
{
   $pos = -1;
   for ($i=0,$i=count($order);$i<$_i&&$pos==-1;$i++)
   {
      if ($order[$i] == $thing) $pos = $i;   
   }
   return $pos;
}

function swap(&$array,$posa,$posb)
{
   $temp = $array[$posa];
   $array[$posa] = $array[$posb];
   $array[$posb]  = $temp;      
}

function order_milestones(&$m,$path)
{
   for ($i=0,$_i=count($m);$i<$_i;$i++)
   {
      $m_order[$i] = $i;
   }
   
   for ($i=0,$_i=count($path);$i<$_i;$i++)
   {
      do 
      {
         $changes = 0;
         
         for ($j=1,$_j=count($path[$i]['milestone']);$j<$_j;$j++)
         {
            $before = $path[$i]['milestone'][$j-1];
            $after  = $path[$i]['milestone'][$j];
            
            $current_pos_before = check_pos($before,$m_order);
            $current_pos_after = check_pos($after,$m_order);
            
            if (($current_pos_before != -1) && ($current_pos_after != -1))
            {
               if ($current_pos_before > $current_pos_after) 
               {
                  swap($m_order,$current_pos_before,$current_pos_after);
                  $changes = 1;
               }
            }   
         } 
      } while ($changes == 1);   
   }
      
   $m_old = $m;
   for ($i=0,$_i=count($m);$i<$_i;$i++)
   {
      $number = $m_order[$i];
      $m[$i]  = $m_old[$number];
   }      
}


function deadline_calc($startdate,$workdays)
{
   
   $today = array($startdate['month']+0,$startdate['day']+0,$startdate['year']+0);      
   
   $months = array("january","february","march","april","may","june",
               "july","august","september","october","november","december");
   
   include 'nondays.inc';
   
   for ($i=0,$j=$today[1];$i<=$workdays;$j++) 
   {
      if (!atk_in_array(date('D',mktime(0,0,0,$today[0],$j,$today[2])),$nonday) && 
          !atk_in_array(date('Ymd',mktime(0,0,0,$today[0],$j,$today[2])),$nondate)) $i++;         
   }  
   $stamp = mktime(0,0,0,$today[0],$j,$today[2]);
   
   $month = text($months[date('n',$stamp)-1]);
   $year = date('Y',$stamp);
   $day = text(strtolower(date('D',$stamp)));
   $daynr = date('j',$stamp); 
   return $day."<br>".$month." ".$daynr."<br>".$year;
}

function print_milestones($m,$phase_ids,$phase_names,$hoursperphase,$project_record)
{
  global $g_layout;
  
  $echo.= $g_layout->data_top();
  
  $echo.= $g_layout->tr_top();
  
  $echo .= $g_layout->ret_td_datatitle(text("deadline").' <img src="images/arrow_right.gif">','colspan="2" align="right"');
  
  for($counter=0,$_counter=count($m);$counter<$_counter;$counter++)
  { 
     $deadline = deadline_calc($project_record['startdate'],$m[$counter]['deadline']);
     $echo.= $g_layout->ret_td_datatitle($deadline, 'colspan="2" align="center"');   
  }
  $echo.= $g_layout->ret_td_datatitle(text('time'));
  
  $echo.= $g_layout->tr_bottom().$g_layout->tr_top();
  
  $echo.= $g_layout->ret_td_datatitle(text("phase").' <img src="images/arrow_down.gif">');
  $echo.= $g_layout->ret_td_datatitle(text("milestones").' <img src="images/arrow_right.gif">','align="right"');
  
  for($counter=0,$_counter=count($m);$counter<$_counter;$counter++)
  { 
    $echo.= $g_layout->ret_td("m".$counter.'<br><img src="images/arrow_down.gif">','colspan="2" align="center"');   
  }  
  $echo.= $g_layout->ret_td_datatitle(text("booked")."/".text("planned"));
  $echo.= $g_layout->tr_bottom();
  
  $totalbooked=0;
  $totalplanned=0;

  for ($i=0,$_i=count($phase_ids);$i<$_i;$i++)
  { 
    $echo.= $g_layout->tr_top();
    $echo.= $g_layout->ret_td($phase_names[$i],'colspan="2"');
    $bgcolor='black';
   
    $number_cols = count($m)*2;
    
    $cell = '<table width="100%" cellspacing="0" cellspacing="0" border="0">'.$g_layout->tr_top();
   
    for($j=0,$_j=count($m);$j<$_j;$j++)
    { 
      if ($bgcolor=='red')
      {
        $cell.= "<td bgcolor='#555555' background='images/stats_red50.gif'>&nbsp;</td>";
      }
      else 
      {
        $cell.= "<td bgcolor='#555555'>&nbsp;</td>";
      }
      if (atk_in_array($phase_ids[$i],$m[$j]['in']))  $bgcolor='black';
      if (atk_in_array($phase_ids[$i],$m[$j]['out'])) $bgcolor='red';
      if ($bgcolor=='red')
      {
        $cell.= "<td bgcolor='#555555' background='images/stats_red50.gif'>&nbsp;</td>";
      }      
      else 
      {
        $cell.= "<td bgcolor='#555555'>&nbsp;</td>";       
      }
    }
   
    $cell.= $g_layout->tr_bottom()."</table>";
    
    $echo.= $g_layout->ret_td($cell, 'colspan="'.$number_cols.'" bgcolor="#555555"');
        
    $booked_time  = $hoursperphase[$phase_ids[$i]]; 
    $totalbooked += $booked_time;
    $planned_time = $project_record['phase'][$i]['max_hours']*60;
    $totalplanned += $planned_time;
    if ($booked_time > $planned_time) $text_color='ff0000'; else $text_color='000000';
   
    $echo.= $g_layout->ret_td('<font color="#'.$text_color.'"> '.time_format($booked_time, true)." / ".time_format($planned_time, true)." </font>");
    $echo.= $g_layout->tr_bottom();
  }
  
  $echo.=$g_layout->tr_top();
  $echo.=$g_layout->ret_td_datatitle(text(total).':','align="right" colspan="'.($number_cols+2).'"');
  $echo.=$g_layout->ret_td_datatitle(time_format($totalbooked, true)." / ".time_format($totalplanned, true));
  $echo.=$g_layout->tr_bottom();

  $echo.= $g_layout->data_bottom();  

  return $echo;

}

// **********************************************************************
function del_edge(&$m, $current_m, $in, $out) 
{

   $del = $in;
   $edge_type = 'in';
   for ($counter=0;$counter<2;$counter++)
   {
      if ($del != '')
      {
         for ($i=0,$_i<count($m[$current_m][$edge_type]);$i<$_i;$i++)
         {   
            if ($del != $m[$current_m][$edge_type][$i]) $new[] = $m[$current_m][$edge_type][$i];
         }
         $m[$current_m][$edge_type] = $new;   
      }
   $del = $out;
   $edge_type = 'out';
   }
      
}
// **********************************************************************
function ins_edge(&$m, $current_m, $in, $out) 
{

   if ($in!='')  $m[$current_m]['in'][]  = $in;
   if ($out!='') $m[$current_m]['out'][] = $out;
}

// **********************************************************************
function ins_milestone(&$m, $before, $after)
{

   $m[count($m)] = $m[count($m)-1];   //doesn't work in php3
   
   $m[count($m)-1]['in']  = $m[count($m)-2]['in']; 
   $m[count($m)-1]['out'] = $m[count($m)-2]['out']; 
   $m[count($m)-1]['deadline'] = $m[count($m)-2]['deadline'];

   $m[count($m)-2]['in']  = array (); 
   $m[count($m)-2]['out'] = array ();
   $m[count($m)-2]['deadline'] = 0;

   $current_m      = get_current_milestone($m,$before,'in');   
   del_edge($m, $current_m, $before, '');
   $current_m      = get_current_milestone($m,$after,'out');   
   del_edge($m, $current_m, '', $after);

   ins_edge($m, count($m)-2, $before, $after);   
}
// **********************************************************************
function get_phases($milestone,$edge_type)
{
   $phases = $milestone[$edge_type];
   
   if (count($phases)==0) $phases = array(); 

   return $phases;   // returns a array with phases
}
// **********************************************************************
function get_current_milestone($m,$phase,$edge_type)
{
   $number = '';
   for($counter=0,$_counter=count($m);$counter<$_counter;$counter++) 
   {
      for ($i=0,$_i<count($m[$counter][$edge_type]);$i<$_i;$i++) 
      {
         if ($m[$counter][$edge_type][$i]== $phase) 
         {
            $number = $counter;
         }   
      }
   }

   return $number;
}
// **********************************************************************
//PATH FUNCTIONS
// **********************************************************************
function is_edge($m,$phase)
{
   $result=0;
   for ($i=0,$_i=count($m['in']);$i<$_i&&$result==0;$i++)
   {
      if ($m['in'][$i]==$phase) $result=1;   
   }
   
   for ($i=0,$_i=count($m['out']);$i<$_i&&$result==0;$i++)
   {
      if ($m['out'][$i]==$phase) $result=1;   
   }  
   
   return $result;
}
// **********************************************************************
function cycle_check($path)
{
   $cycle=0;
   for ($i=0,$_i=count($path);$i<$_i&&$cycle==0;$i++)
   {
      $phase = $path[$i];
      $counter=0;
      
      for ($j=0;$j<count($path);$j++) 
      {
         if ($path[$j]==$phase) $counter++; 
      }
      if ($counter>1) $cycle=1;
   }
   return $cycle;
}
// **********************************************************************
function complete_check($m,$phase)
{
   
   for ($i=0;$i<count($m);$i++)
   {
      if (is_edge($m[$i],$phase)) $milestone=$i;
   }
   
   if (count($m[$milestone]['out'])==0) $complete=1; else $complete=0;
   
   return $complete;
}
// **********************************************************************
function create_new_path()
{
   $path['phase']       = array();
   $path['milestone']   = array();
   $path['time']        = 0;
   $path['cycle']       = 0; 
   $path['complete']    = 0;

   return $path;
}

function new_paths($m,$path,$current_path_nr,$out_phases)
{
   if ($path == '') $path[0] = create_new_path();

   $temp = $path[$current_path_nr]; 
    
   $path[$current_path_nr]['phase'][]     = $out_phases[0]; 
   $path[$current_path_nr]['milestone'][] = get_current_milestone($m,$out_phases[0],'in');
   $path[$current_path_nr]['time']        = 0;
   $path[$current_path_nr]['cycle']       = cycle_check($path[$current_path_nr]['phase']);
   $path[$current_path_nr]['complete']    = complete_check($m,$out_phases[0]);
   
   $new_path_nr = count($path);
   for ($i=1;$i<count($out_phases);$i++) 
   {
      
      $path[$new_path_nr] = $temp;
      
      $path[$new_path_nr]['phase'][]      = $out_phases[$i];
      $path[$new_path_nr]['milestone'][]  = get_current_milestone($m,$out_phases[$i],'in');
      $path[$new_path_nr]['time']         = 0;      
      $path[$new_path_nr]['cycle']        = cycle_check($path[$new_path_nr]['phase']);
      $path[$new_path_nr]['complete']     = complete_check($m,$out_phases[$i]);
    
      $new_path_nr++;
      
   }
   
   return $path;
}
// **********************************************************************
function get_all_paths($m)
{

  $path = '';
  $current_path_nr = 0;
  $out_phases = get_phases($m[0],'out');                            // start with milestone 0 

  do 
  {   
    $path = new_paths($m,$path,$current_path_nr,$out_phases);
    
    for ($cycle=0,$i=0;$i<count($path)&&$cycle==0;$i++) 
    {
      if ($path[$i]['cycle']==1) $cycle = 1;
    }

    if ($cycle==0) 
    {
      for ($complete=1,$i=0;$i<count($path)&&$complete==1;$i++) 
      {
         if ($path[$i]['complete']==0)                               // incomplete path
         {
            $complete = 0;
            $current_path_nr = $i;      
            $milestone = $path[$i]['milestone'][count($path[$i]['milestone'])-1]; 
            $out_phases =  get_phases($m[$milestone],'out');
         }   
      }
    }
  } while ($cycle==0 && $complete==0);

  if ($cycle==1) $path = '';

  return $path;
}
// ***********************************************************
function get_phase_pos($path,$phase)
{
   $pos = -1;

   for ($i=0;$i<count($path);$i++)
   {
      if ($path[$i]==$phase) $pos = $i;
   }
   return $pos;
}

// ***********************************************************
// ***********************************************************
//  MILESTONES MAIN
// ***********************************************************
// ***********************************************************

  $where = "project.id=".$selectedprojectid;
  $data = $this->selectDb($where,"","",array("customer","description","status"),"");
  reset($data);
  $project_record = current($data);
  
  // put phase info into a more accessible array..
  $phaseinfo = $project_record['phase'];
  for ($i=0;$i<count($phaseinfo);$i++)
  {
    $phase_ids[] = $phaseinfo[$i]['id'];
    $phase_names[] = $phaseinfo[$i]['name'];
    $phase_maxtimes[] = $phaseinfo[$i]['max_phasetime'];
  }

  $dep       = load_dependencies($phase_ids,$project_record['dependencies']['deps']); 
  $m         = init_milestones($phase_ids);
 
  $hoursquery = "SELECT 
                   sum(time) as time, phase.id as phase
                 FROM 
                   phase, hours
                 WHERE
                   phase.id = hours.phaseid
                   AND phase.projectid = ".$selectedprojectid."
                 GROUP BY 
                   phase.id";
  
  $recs = $g_db->getrows($hoursquery);  
  for ($i=0;$i<count($recs);$i++)
  {
    $hoursperphase[$recs[$i]["phase"]] = $recs[$i]["time"];
  }  

  // Startpoints
  $befores = $dep["befores"];
  $afters = $dep["afters"];
  
  for ($i=0;$i<count($befores)&&$error==0;$i++)
  {
    $before_phase  = $befores[$i]; 
    $after_phase = $afters[$i];

    $m_old = $m;    
    $nonew_m = -2;
    $dependency = '?';
    $error = 0;   

    do 
    {         
      $path = get_all_paths($m);
      
      // CALC CURRENT DEPENDENCY
      
      if ($path[0]['phase'][0] != '') 
      {   
         $dep_status = 0;
         for ($path_nr=0;$path_nr<count($path)&&$dep_status==0;$path_nr++)
         {            
            $before_pos = get_phase_pos($path[$path_nr]['phase'],$before_phase);
            $after_pos  = get_phase_pos($path[$path_nr]['phase'],$after_phase);
             
            if ($before_pos!='-1' && $after_pos!='-1') 
            { 
               if ($before_pos>=$after_pos) $dep_status = -1;
               else $dep_status = 1;            
            }
         }
      } 
      else 
      {          
         $dep_status = -1;
      }
       
      // EVALUATE   SITUATION
     
      if ($dep_status>0)
      {
         $dependency = 'ok';
      }      
      elseif ($nonew_m<-1) 
      {
        $nonew_m = -1;
      }      
      else
      {
         $m = $m_old;          //restore milestone table
         switch ($nonew_m)
         {
         case '-1':
            $nonew_m=1;
            break;
         case '1':
            $nonew_m=0;
            break;
         default:
            $error=1;
         }
      }     
      
      // CHANGE MILESTONE TABLE
      
      if ($dependency!='ok' && $error==0) 
      {
         switch ($nonew_m)
         {
            case '-1':
               // m_after_out = m_before_in
               $current_m = get_current_milestone($m,$after_phase,'out');                              
               del_edge($m, $current_m, '', $after_phase);
               $current_m = get_current_milestone($m,$before_phase,'in');               
               ins_edge($m, $current_m,'',$after_phase);
            break;
                  
            case '1':
               // m_before_in = m_after_out
               $current_m = get_current_milestone($m,$before_phase,'in');
               del_edge($m, $current_m, $before_phase,'');
               $current_m = get_current_milestone($m,$after_phase,'out');
               ins_edge($m, $current_m, $before_phase, '');
            break;
               
            default:
               //insert new milestone 
               ins_milestone($m, $before_phase, $after_phase);               
         }
      } 
    } 
    while ($dependency!='ok' && $error==0);
  }

  // **********

  if ($error) 
  {
     echo "<br><b>ERROR : </b>";
     echo $before_phase.' before '.$after_phase;
  } 
  else 
  { 
     order_milestones($m,$path);
     calc_milestone_deadlines($m,$phase_ids,$phase_maxtimes);   
           
     $milestones_table = print_milestones($m,$phase_ids,$phase_names,$hoursperphase,$project_record);
  }
      
  $g_layout->ui_top(text("title_projectplanning")); 

  $project_info = '<br><table border=0>';
  $project_info.= '<tr><td align="right"><b>'.text("name").': </b></td><td>'.$project_record['name'].'</td></tr>';
  $project_info.= '<tr><td align="right"><b>'.text("coordinator").': </b></td><td>'.$project_record['coordinator']['name'].'</td></tr>';
  $project_info.= '</table><br>';

  $g_layout->output($project_info);   
  if (count($phase_ids)==0) 
  {
    $g_layout->output('<b>'.text('nophasesdefined'),'</b><br><br>');   
  }
  else 
  {
    $g_layout->output($milestones_table);   
    $g_layout->output('<br><br>');   
  }

  $g_layout->ui_bottom(); 
?>
