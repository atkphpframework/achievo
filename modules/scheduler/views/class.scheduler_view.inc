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

atkimport("modules.scheduler.utils.schedulertools");

/**
 * Abstract view class
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_view
{
  var $m_model;
  var $m_view;
  var $m_viewTitle="";
  var $m_userPrefs=array();

 /**
  * Calender view constructor
  * @return void
  */
  function scheduler_view()
  {
    $this->m_model = &atkinstance("module.scheduler.scheduler_model");
  }

  function &getInstance($name)
  {
    static $s_instance = null;
    if (is_null($s_instance))
    {
      eval ("\$s_instance = new scheduler_".$name."view();");
    }
    return $s_instance;
  }

 /**
  * Create default view header html
  *
  * @todo Convert to template
  * @return string default view header
  */
  function _view_header_html($params)
  {
    $view_header = "";
    foreach($params["links"] as $label => $url)
      $view_header .= sprintf("<a href='%s'>%s</a>&nbsp;", $url, atkText($label, "scheduler"));
    $view_header.= "<br><br><table width='100%'>";
    $view_header.= "<tr>";
    $view_header.= "<td valign='top' align='left'>".$params["nav"]."</td>";
    $view_header.= "<td valign='top' align='right'>".$params["selector"]."</td>";
    $view_header.= "</tr>";
    $view_header.= "<tr><td valign='top' align='left'><b>".$params["viewtitle"]."</b></td>";
    $view_header.= "</tr></table>";
    return $view_header;
  }

  /**
   * Returns the default view header
   *
   * @return string default view header
   */
  function _view_header()
  {
    $links = array();
    $links["additem"] = session_url(dispatch_url("scheduler.scheduler", "add"),SESSION_NESTED);
    $views = array("day", "week", "month", "birthday" /*, "year", "employee"*/);
    foreach($views as $view)
      $links[$view."view"] = session_url(dispatch_url("scheduler.scheduler", "admin", array("view"=>$view)));
    $links["refreshview"] = session_url(dispatch_url("scheduler.scheduler", "admin", array("view"=>$this->m_view, "viewdate"=>$this->m_model->m_viewdate)));

    $viewheaderparams = array(
      "links" => $links,
      "nav" => $this->_getNav(),
      "selector" => $this->m_model->getSelector(),
      "viewtitle" => $this->m_viewTitle,
    );
    return $this->_view_header_html($viewheaderparams);
  }

 /**
  * Returns the default view navigation
  * @return string default view navigation
  */
  function _getNav()
  {
  }


  /**
   * Render the view
   * @return string A rendered view
   */
  function render($request)
  {
    $this->m_model->setViewDate($request["viewdate"]);
    $this->m_model->setSmallViewDate($request["small_viewdate"]);
    $this->m_model->setViewUser($request["viewusers"]);
    $this->m_model->setView($this->m_view);
		$this->m_model->setCalendarStartDay(atkconfig('startday',1));
    $this->m_userPrefs = $this->m_model->getUserSchedulerPrefs();
    return $this->_render();
  }

}

?>