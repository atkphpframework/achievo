<?php

  /**
   * Project extension with a custom action to remove the project from a contract.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$
   */

  include_once(moduleDir("project")."class.project.inc"); // include original baseclass.

  class contract_project extends project
  {
    function contract_project()
    {
      $this->project();
      $this->m_flags |= NF_NO_FILTER;;
      $this->m_type = "contract_project";
      $statusattr = &$this->getAttribute("status");
      $statusattr->addFlag(AF_FORCE_LOAD);
      $customerattr = &$this->getAttribute("customer");
      $customerattr->setForceInsert(true);

      // Anyone who may edit contracts, may remove a project from the deposit.
      $this->m_securityMap["remove"] = "contracts.edit";

      // The rest of the actions has equal privileges to the original node.
      $this->setSecurityAlias("project.project");

      //clear parent filters.
      $this->m_fuzzyFilters = array();
      $this->m_filters = array();
    }

    /**
     * Remove a project from a contract.
     */
    function action_remove(&$handler)
    {
      if ($this->m_postvars["project_id"]!="")
      {
        $db = &atkGetDb();
        $db->query("UPDATE project SET contract_id = NULL WHERE id = '".$this->m_postvars["project_id"]."'");
      }
      $this->redirect();
    }

    function recordActions($record, &$actions, &$mraactions)
    {
    	$actions = array(); // Remove all default actions.
      if ($this->allowed("remove", $record)) $actions["remove"] = dispatch_url($this->atkNodeType(), "remove", array("project_id"=>$record["id"]));
      if ($this->allowed("view", $record)) $actions["view"] = dispatch_url("project.project", "view", array("atkselector"=>$this->primaryKey($record)));
      if ($this->allowed("edit", $record)) $actions["edit"] = dispatch_url("project.project", "edit", array("atkselector"=>$this->primaryKey($record)));
    }

    function initial_values()
    {
      $forceList = (isset($this->m_postvars['atkfilter'])) ? decodeKeyValueSet($this->m_postvars['atkfilter']) : array();
      $node = &atkGetNode("organization.contracts");
      $selector = sprintf("`contract`.`id`='%d'", $forceList["contract_id.id"]);
      $rec = $node->selectDb($selector, "", "", "", array("contractname", "customer"));
      $initial_values = parent::initial_values();
      $initial_values["name"] = $rec[0]['contractname'];
      $initial_values["customer"] = $rec[0]['customer'];
      return $initial_values;
    }
  }

?>