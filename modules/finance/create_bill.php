<?php

/* Create Bill functions for billing_project */
	
/* specify_hours 
* Show all ours of the project with a checkbox, default all hours on
*
*/
function time_format($time)
{
  if ($time==0) return "&nbsp;";
  return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
}  

function specify_hours($rec)
{
  global $g_layout,$g_db;
	var_dump($rec);
	// Get all hours from the project
	$sql = "SELECT hours.id,hours.userid,hours.remark,hours.activitydate,hours.time,
								 phase.name as phase_name,activity.name as activity_name,
								 project.name as project_name
				  FROM hours,phase,activity,project
				  WHERE hours.phaseid=phase.id
					  AND hours.activityid=activity.id
						AND phase.projectid=project.id
					  AND phase.projectid=".$rec["project_id"]["id"];
	
	$nrows=$g_db->getrows($sql);
	$g_layout->ui_top("Hours specify");
	$g_layout->output("<center>Select the hours for the bill<br><br>");
	$g_layout->output('<a href="javascript:alleboxjesaan();">All boxes on</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;');
	$g_layout->output('<a href="javascript:alleboxjesuit();">All boxes off</a>');
	
	$g_layout->output('<form action="./modules/finance/create_bill.php" method="post">');
	$g_layout->output('<input type="hidden" name="specify_post" value=1>');
	$g_layout->output('<input type="hidden" name="project_id" value='.$rec["project_id"]["id"].'>');
	$g_layout->output('<input type="hidden" name="bill_id" value='.$rec["bill_id"].'>');
	$g_layout->table_simple(1);
	$g_layout->output("<tr>");
	
	$g_layout->td_datatitle("");
	$g_layout->td_datatitle("Owner");
	$g_layout->td_datatitle("Project/Phase");
	$g_layout->td_datatitle("Activity");
	$g_layout->td_datatitle("Remark");
	$g_layout->td_datatitle("Date");
	$g_layout->td_datatitle("Time");
	$g_layout->output("</tr>");
	for($i=0;$i<count($nrows);$i++)
	{
	  $g_layout->output("<tr>");
	  $g_layout->td('<input type="checkbox" name="hour_id['.$nrows[$i]["id"].']" value=1">');
	  $g_layout->td($nrows[$i]["userid"]);
	  $g_layout->td($nrows[$i]["project_name"]." - ".$nrows[$i]["phase_name"]);
	  $g_layout->td($nrows[$i]["activity_name"]);
	  $g_layout->td($nrows[$i]["remark"]);
	  $g_layout->td($nrows[$i]["activitydate"]);
	  $g_layout->td(time_format($nrows[$i]["time"]));
	  $g_layout->output("</tr>");
	}
	$g_layout->output("</table>");
	$g_layout->output('<center><input type="submit" name="submit" value="Create Bill"></center>');
	$g_layout->output("</form>");
	$g_layout->ui_bottom();
	
}

function create_bill($bill_id,$project_id,$hour_id,$specify=0,$printable=0)
{
  global $g_layout,$g_db;
	
  echo "Create bill for :<br><br>";
	
	if($specify&&is_null($hour_id))
	{
	  echo "No hours Selected";
	}
	elseif($specify&&!is_null($hour_id))
	{
	  // Use specified hours
		$hours = "WHERE hours.id IN (";
		
		// Loop true all hours
		foreach ($hour_id as $key => $value)
		{
		  $hours.=$key.",";
		}
		// Remove the last  ','
		$hours = substr($hours,0,strlen($hours)-1);
		$hours.=") ";
	}
	else
	{
	  // Use all hours
		$hours="";
	}
	
	$sql = "SELECT * 
					FROM hours,phase
					WHERE phase.projectid=".$project_id." ";
  if(!empty($hours)) $sql.= "AND ".$hours;
	
	$nrows=$g_db->getrows($sql);
	
	for($i=0;$i<count($nrows);$i++)
	{
	  
	}
	
	
	

}
	
// Check if rec is set
if(!is_null($rec))
{
  // is comming from Edit / Add mode
	// check if specify hours is on
	if($rec["specify_hours"])
	{
	  specify_hours($rec);
	}
	else
	{
	  atkdebug("NO Specify ?");
	}
}
elseif($specify_post==1)
{
  create_bill($bill_id,$project_id,$hour_id,1);
}
else
{
  atkdebug("NO REC ??? NO POST ???");
  // is comming from view mode or what ever??
}







?>