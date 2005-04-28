<?php

  // phase date fields
  $setup->addColumn("phase", "startdate", "date", true);  
  $setup->addColumn("phase", "enddate", "date", true);  
  
  // by default, set phase dates equal to project dates
  $recs = $g_db->getrows("SELECT startdate, enddate, id FROM project");
  for($i=0, $_i=count($recs); $i<$_i; $i++)
  {
    $g_db->query("UPDATE phase SET startdate='".$recs[$i]["startdate"]."', enddate='".$recs[$i]["enddate"]."' WHERE projectid=".$recs[$i]["id"]);
  }
  
  // initial planning
  $setup->addColumn("phase", "initial_planning", "int(10)", false, "0");
  $g_db->query("UPDATE phase SET initial_planning = max_hours"); 
  
  $setup->alterColumn("phase", "max_hours", "current_planning", "int(10)", true);
    
  $setup->installNode("project.phaseplanning");
?>