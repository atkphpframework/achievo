<?php

global $g_layout;

global $startdate;
global $enddate;

if ($startdate=='') $startdate = '2000-01-01';
if ($enddate=='') $enddate = date('Y-m-d');

// **********

$where = "id=".$this->m_postvars['selectedprojectid'];
$data = $this->selectDb($where,"","","","");
reset($data);
$project_record = current($data);

// **********

$g_layout->initGUI();
$g_layout->ui_top("Bill setup");

$project_info = '<br><table border=0>';
$project_info.= '<tr><td align="right"><b>Name : </b></td><td>'.$project_record['name'].'</td></tr>';
$project_info.= '<tr><td align="right"><b>Coordinator : </b></td><td>'.$project_record['coordinator']['name'].'</td></tr>';
$project_info.= '<form action="dispatch.php?atknodetype=project&atkaction=billing&selectedprojectid='.$this->m_postvars['selectedprojectid'].'" method="post">
                 <tr><td align="right"><b>Period : </b></td><td>
                 <input type="text" size="10" maxlength="10" name="startdate" value="'.$startdate.'">
                 until
                 <input type="text" size="10" maxlength="10" name="enddate" value="'.$enddate.'">
                 <input type="submit" value="Continue">
                 </td></tr></form>';
$project_info.= '</table><br>';

$g_layout->output($project_info);

$g_layout->ui_bottom();
?>