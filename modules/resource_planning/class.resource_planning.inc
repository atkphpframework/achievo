<?php
 /**
   * Module Resource planning
   * Class resource_planning
   *
   * This class makes it possible to plan hours for employees en to view the personal planning for employees.
   *
   * $Id$
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision$
   *
   */
include_once $config_atkroot."/modules/resource_planning/class.planprojectattrib.inc";
include_once $config_atkroot."achievotools.inc";

class resource_planning extends atkNode
{

 function action_planweekview()
 {
   global $config_atkroot;
   include $config_atkroot.'modules/resource_planning/planweekview.inc';
 }

function action_planweekviewempl()
 {
   global $config_atkroot;
   include $config_atkroot.'modules/resource_planning/planweekviewempl.inc';
 }

function resource_planning()
  {
    global $ATK_VARS;
    $this->atkNode("resource_planning");

    $this->m_securityMap["edit"] = "resplanning";
    $this->m_securityMap["delete"] = "resplanning";
    $this->m_securityMap["view"] = "resplanning";
    $this->m_securityMap["list"] = "resplanning";
    $this->m_securityMap["update"] = "resplanning";

    $this->m_securityMap["planweekview"] = "resplanning";
    $this->m_securityMap["planweekviewempl"] = "myplanning";
    $this->m_securityMap["view"] = "myplanning";
    $this->m_securityMap["list"] = "myplanning";

    $this->add(new atkAttribute("id" ,AF_PRIMARY|AF_UNIQUE|AF_AUTO_INCREMENT|AF_HIDE));
    $phase = new atkManytoOneRelation("phaseid","phase", AF_OBLIGATORY);
    $phase->m_destinationFilter = $ATK_VARS["projectfilter"];
    $this->add(&$phase);

    $employee = new atkManytoOneRelation("employeeid","employee", AF_OBLIGATORY);
    $employee->m_destinationFilter = $ATK_VARS["employeefilter"];
    $this->add(&$employee);

    $this->add(new atkDateAttribute("plandate",AF_OBLIGATORY));
    $this->add(new atkDurationAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));
    $this->add(new atkTextAttribute("description"));

    $this->setTable("planning","planning");
    $this->setOrder("id");
  }

function action_list()
{
  $this->adminPage();
}

function recordActions($record, &$actions)
{
  if (!empty($this->m_postvars["noactions"]))
  $actions = array();
}

}
?>
