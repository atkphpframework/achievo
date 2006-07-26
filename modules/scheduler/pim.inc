<?php

  /**
   * Generate the scheduler for the pim page
   *
   * @return string The scheduler for the pim page
   */
  function pimScheduler()
  {
    // Setup the pimview to view the current date for the current user
    $pimview = &atknew("modules.scheduler.views.scheduler_pimview");
    $pimview->setViewDate();
    $pimview->setViewUser(atkArrayNvl(getUser(), "id"));

    // Build the result containing the rendered pimview
    $res = $pimview->renderView();

    // Add a link to the scheduler if allowed
    $securitymanager = &atkGetSecurityManager();
    if ($securitymanager->allowed("scheduler.scheduler", "admin"))
      $res.= '<br>' . href(dispatch_url("scheduler.scheduler", "admin"), atkText("pim_gotoschedule", "scheduler"), SESSION_NESTED);

    // Return the result
    return $res;
  }

?>