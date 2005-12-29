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
      $this->m_flags |= NF_NO_EDIT;
      $this->m_type = "contract_project";
                        
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
      $actions["remove"] = dispatch_url($this->atkNodeType(), "remove", array("project_id"=>$record["id"]));
      $actions["view"] = dispatch_url("project.project", "view", array("atkselector"=>$this->primaryKey($record)));          	
    }
    
    function initial_values()
    {
    	$selector = $this->m_postvars['atkselector'];
    	$node = &getNode("organization.contracts");    	    	
    	$rec = $node->selectDb($selector, "", "", "", array("subject"));    	
    	
    	return array("name" => $rec[0]['subject']);    	
    }
  }

?>