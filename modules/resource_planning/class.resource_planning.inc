<?php
  /**
   * Module Resource planning
   * Class resource_planning
   *
   * This class makes it possible to plan hours for employees en to view the personal planning for employees.
   *
   * $Id
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision
   *
   */
  include_once $config_atkroot."/modules/resource_planning/class.planprojectattrib.inc";
  include_once $config_atkroot."achievotools.inc";

  userelation("atkmanytoonerelation");
  useattrib("atkdateattribute");
  useattrib("atktextattribute");
  useattrib("atkdurationattribute");

  class resource_planning extends atkNode
  {

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

      $this->m_securityMap["gantview"] = "gant";

      $this->add(new atkAttribute("id" ,AF_AUTOKEY));

      $phase = new atkManytoOneRelation("phaseid","project.phase", AF_OBLIGATORY);
      $phase->m_destinationFilter = $ATK_VARS["projectfilter"];
      $this->add($phase);

      $employee = new atkManytoOneRelation("employeeid","employee.employee", AF_OBLIGATORY);
      $employee->m_destinationFilter = $ATK_VARS["employeefilter"];
      $this->add($employee);

      $this->add(new atkDateAttribute("plandate",AF_OBLIGATORY));
      $this->add(new atkTextAttribute("description"));
      $this->add(new atkDurationAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));

      $this->setTable("planning","planning");
      $this->setOrder("id");
    }

    function action_list()
    {
      $this->adminPage();
    }

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

    function action_gantview()
    {
      global $config_atkroot;
      include $config_atkroot.'modules/resource_planning/gantview.inc';
    }

    function recordActions($record, &$actions)
    {
      if (!empty($this->m_postvars["noactions"]))
      $actions = array();
    }

    function phaseid_display(&$record, $mode)
    {
      // For the view planning in the PIM the projectname should als be displayed because if there are duplicate phasenames in different
      // projects it isn't clear for which project the planning is meant

      // TO DO: Find a better solution so that this functions gets the projectname by an parameter. This will make the query superfluous

      global $ATK_VARS;
      global $g_db;            

      if ($ATK_VARS["atknodetype"] != "pim")
        return $this->m_attribList["phaseid"]->display($record, $mode);
      else
      {
        $phaseid=$record["phaseid"]["id"];
        $name = "atk".atkconfig("database")."query";
        $query = new $name();
        $query->addTable('project');
        $query->addJoin('phase','', 'phase.projectid = project.id', FALSE);
        $query->addCondition('phase.id = '.$phaseid);
        $query->addField('name', '', 'project');
        $querystring = $query->buildSelect(TRUE);
        $projectname = $g_db->getrows($querystring);
        return $projectname[0]['name'].' - '.$record["phaseid"]["name"];
      }
    }

  }

?>