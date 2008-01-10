<?php

  /**
   * Generate the scheduler for the pim page
   *
   * @return string The scheduler for the pim page
   */
  function pimScheduler()
  {
     $controller = &atkinstance("module.scheduler.scheduler_controller");
     $request = array("view"=>"pim",
                      "viewdate"=>null,
                      "small_viewdate"=>null,
                      "viewusers"=>array(atkArrayNvl(atkgetUser(), "id")));
                         
     $res = $controller->handleRequest($request);

    // Add a link to the scheduler if allowed
    $securitymanager = &atkGetSecurityManager();
    $res.="<br>";
    
    if ($securitymanager->allowed("scheduler.scheduler", "add"))
      $res.= href(dispatch_url("scheduler.scheduler", "add"), atkText("add"), SESSION_NESTED);

    if ($securitymanager->allowed("scheduler.scheduler", "admin"))
      $res.= "&nbsp;&nbsp;&nbsp;".href(dispatch_url("scheduler.scheduler", "admin"), atkText("pim_gotoschedule", "scheduler"), SESSION_NESTED);

    // Return the result
    return $res;
  }

?>