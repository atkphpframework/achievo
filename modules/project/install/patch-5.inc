<?php

  // Convert hours to minutes
  $db->query("UPDATE phase SET initial_planning=initial_planning*60,current_planning=current_planning*60,max_phasetime=max_phasetime*60"); 
  
  // For the people who used a test version, we also convert the new project phaseplanning table
  $db->query("UPDATE project_phaseplanning SET initial_planning=initial_planning*60,current_planning=current_planning*60");
?>