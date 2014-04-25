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
 * Controller for the scheduler views
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_controller
{

    function &getInstance()
    {
        static $s_ins;
        if (is_null($s_ins)) {
            $s_ins = new scheduler_controller();
        }
        return $s_ins;
    }

    /**
     * Constructor
     *
     */
    function controller()
    {
        
    }

    /**
     * handle HTTP requests
     *
     * @param $request array Array with the request vars
     */
    function handleRequest($request)
    {
        $model = &atkinstance("module.scheduler.scheduler_model");

        $view = &$this->getView($request["view"]);
        if ($view !== false && is_object($view)) {
            $ret = $view->render($request);
        } else {
            $ret = atktext("view doesn't exist (" . $request["view"] . "view)");
        }
        return $ret;
    }

    function &getView($view)
    {
        $valid_views = array("day", "week", "month", "birthday", "pim");

        if (in_array($view, $valid_views)) {
            atkimport("modules.scheduler.views.scheduler_" . $view . "view");
            $view = scheduler_view::getInstance($view);
            return $view;
        }
        return false;
    }

}

?>