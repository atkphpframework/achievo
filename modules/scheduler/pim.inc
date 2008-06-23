<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage scheduler
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
  
  
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