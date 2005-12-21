<?php
	include_once(moduleDir("project")."class.project.inc"); // include original baseclass.
  
  class contract_projectselector extends project
  {
    function contract_projectselector($name="contract_projectselector", $flags=0)
    {
      $this->project($name,$flags);
      //$this->m_filters = array();		//clear parent filters
      
      //only show projects that are not linked to a contract yet
      $this->addFilter("(project.contract_id IS NULL OR project.contract_id = '')");            
      $this->setSecurityAlias("organization.contracts");
    }
  }
?>