 <?php

global $g_layout;

global $startdate;
global $enddate;

if ($startdate=='') $startdate = '2000-01-01';
if ($enddate=='') $enddate = date('Y-m-d');

// **********

function load_phases($phase_record,$field)
{
   for ($i=0;$i<count($phase_record);$i++) {
      $phase[] = $phase_record[$i][$field];
   }

   return $phase;
}

function time_str($in)
{
   $hours = floor($in);
   $min = $in-$hours;
   switch ($min)
   {
     case 0.25: $min='15'; break;
     case 0.50: $min='30'; break;
     case 0.75: $min='45'; break;
     default: $min='00';
   }
   return $hours.':'.$min;
}

function hours_list($hours,$id)
{
   $exits = 0;
   while (list($key,$value) = each($hours))
   {
      if ($key == $id) $exits = 1;
   }
   if ($exits == 0) $hours[$id] = 0;

   return $hours;
}

function get_namelist($class,$key)
{
  $table = new $class;

  $data = $table->selectDb("","","","","");
  for ($i = 0; $i<count($table->m_records); $i++)
  {
    $record = $data[$i];
    $list[$record[$key]] = $record['name'];
  }
  return $list;
}

function activity_hours_table($activity_hours,$names)
{
   $total = 0;
   while (list($key,$value) = each($activity_hours)) $total += $value;
   reset($activity_hours);

   $out = '<table border=0 bgcolor="#000000"><tr><td>';
   $out.= '<table border=0 cellspacing=1 cellpadding=2>';
   $out.= '<tr bgcolor="#ffffff">
           <td width="200"><b>Activity</b></td>
           <td width="40"><b>Time</b></td></tr>';
   while (list($key,$value) = each($activity_hours))
   {
      $out.= '<tr bgcolor="#bbbbbb">';
      $name = $names[$key]; if ($name=='') $name = '?';
      $out.= '<td>'.$name.'</td>';
      $out.= '<td align="right">'.time_str($value).'</td>';
      $out.= '</tr>';
   }
   $out.= '<tr bgcolor="#dddddd"><td><b>Total</b></td><td align="right"><b>'.time_str($total).'</b></td></tr>';
   $out.= '</table>';
   $out.= '</td></tr></table><br><br>';

   return $out;
}
// **********************************************************************
// M A I N
// **********************************************************************

$where = "id=".$this->m_postvars['selectedprojectid'];
$data = $this->selectDb($where,"","","","");
reset($data);
$project_record = current($data);

$phase = load_phases($project_record['phase'],'id');

$g_layout->initGUI();
$g_layout->ui_top("Bill");

$project_info = '<br><table border=0>';
$project_info.= '<tr><td align="right"><b>Name : </b></td><td>'.$project_record['name'].'</td></tr>';
$project_info.= '<tr><td align="right"><b>Coordinator : </b></td><td>'.$project_record['coordinator']['name'].'</td></tr>';
/*
$project_info.= '<form action="dispatch.php3?atknodetype=project&atkaction=billing&selectedprojectid='.$this->m_postvars['selectedprojectid'].'" method="post">
                 <tr><td align="right"><b>Period : </b></td><td>
                 <input type="text" size="10" maxlength="10" name="startdate" value="'.$startdate.'">
                 until
                 <input type="text" size="10" maxlength="10" name="enddate" value="'.$enddate.'">
                 <input type="submit" value="Refresh">
                 </td></tr></form>';
*/
$project_info.= '<tr><td align="right"><b>Period : </b></td>
                 <td>'.$startdate.' until '.$enddate.'</td></tr>';
$project_info.= '</table><br>';

$g_layout->output($project_info);

// **********

$hours = new hours;
$activity_hours = array();
for ($i=0;$i<count($phase);$i++)
{
   $where = "projectid='".$phase[$i]."' AND date>='".$startdate."' AND date<='".$enddate."'";
   $data = $hours->selectDb($where,"","","","");
   for ($i=0;$i<count($hours->m_records);$i++)
   {
      $hours_record = $data[$i];

      $activityid = $hours_record['activityid'];
      $activity_hours = hours_list($activity_hours,$activityid);
      $activity_hours[$activityid] += ($hours_record['hours'] + ($hours_record['minutes']/60));
   }
}

$g_layout->output(activity_hours_table($activity_hours,get_namelist('activity','id')));

// **********

$g_layout->ui_bottom();
?>