<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atklistattribute");
useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdummyattribute");
useattrib("atkfuzzysearchattribute");

useattrib("organization.hiddenattribute");

class contracts extends atkNode
{
  var $m_archive;

  function contracts()
  {
    $this->atkNode("contracts",NF_ADD_LINK | NF_EDITAFTERADD);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("contractnumber",AF_SEARCHABLE, 20));
    $this->add(new atkManyToOneRelation("contracttype","organization.contracttype",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->add(new atkManyToOneRelation("customer","organization.organization",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->add(new atkListAttribute("billing_period",array("monthly","quarterly","yearly", "per_incident"),"",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("period_price",AF_OBLIGATORY|AF_HIDE_LIST, 13, 5));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y", 0, 0, AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkDateAttribute("enddate","F d Y","d F Y", 0, 0, AF_OBLIGATORY));
    $this->add(new atkListAttribute("status", array("active", "archived"), "", AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkTextAttribute("description", TEXT_SMALL, AF_OBLIGATORY));    
    $this->add(new atkOneToManyRelation("projects", "organization.contract_project", "contract_id", AF_HIDE_LIST|AF_READONLY));

    
    //@todo: remove HiddenAttribute, use the name of a button to check if it has been clicked
    //example : $this->m_postvars["atkselector"]
    $this->add(new atkHiddenAttribute("postprojectaction","",AF_HIDE));    
    //$this->add(new atkDummyAttribute("postprojectaction","<input type=\"hidden\" id=\"postprojectaction\" >",AF_HIDE));

    $this->setTable("contract","contract");
    $this->setOrder("startdate");
  }
  
  
  //@todo use atkButton 
  //$result[] = atkButton("knipje", dispath...
  function getFormButtons($mode, $record)
  {
		$result = array();
    
		$onclickstringnew = "onclick=\"document.getElementById('postprojectaction').value='add';\"";
		$onclickstringselect = "onclick=\"document.getElementById('postprojectaction').value='select';\"";
		$onclickstringnone = "onclick=\"document.getElementById('postprojectaction').value='';\"";
		
    if ($mode=="add")
    {
      $result[] = '<input type="submit" class="btn_save" '.$onclickstringnew.' value="'.atktext("save_and_create_new_project","organization","contracts").'">';
      $result[] = '<input type="submit" class="btn_save" '.$onclickstringselect.' value="'.atktext("save_and_select_existing_project","organization","contracts").'">';
      if ($this->m_action=="add")
      {
        // if action is admin, we don't show the cancelbutton..
        $result[] ='<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel","organization","contracts").'">';
      }
    }
    // edit mode
    else if ($mode == "edit")
    { 
    	// if atklevel is 0 or less, we are at the bottom of the session stack,
      // which means that 'saveandclose' doesn't close anyway, so we leave out
      // the 'saveandclose' button. Unless, a feedback screen is configured.
      if (atkLevel() >0 || hasFlag($this->m_feedback["update"], ACTION_SUCCESS))
      {
        $result[] = '<input type="submit" class="btn_save" name="atksaveandclose" '.$onclickstringnone.'value="'.text('saveandclose').'">';
      }
      
      $result[] = '<input type="submit" class="btn_save" name="atknoclose" '.$onclickstringnew.' value="'.atktext("save_and_create_new_project","organization","contracts").'">';
      $result[] = '<input type="submit" class="btn_save" name="atknoclose" '.$onclickstringselect.' value="'.atktext("save_and_link_existing_project","organization","contracts").'">';
      $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel","organization","contracts").'">';
      
      
    }

    return $result;
  }
  
  
  
  function postAdd($record)
  {  	  	
  	$this->HandleRedirect($record); 		
 		return true; 		
  }
  	
 	function postUpdate($record)
 	{  	 	
  	$this->HandleRedirect($record); 		
 		return true; 		
 	}
 	
 	
 	function HandleRedirect($record)
 	{		
		$action = $record["postprojectaction"];
		
 		if($action != "")
 		{
 			$return_url = dispatch_url("organization.contracts", "edit", array("projectid"=>"[id]", "atklevel"=>atkLevel()));
 			$target_url = dispatch_url("organization.contract_projectselector", $action, array("atkfilter" => "projectid=[id]","atktarget" => atkurlencode($return_url)));
 			$this->redirect(session_url($target_url,SESSION_NESTED));
 		}
 	}
 	
 	
 	
 	
 	

	function adminFooter()
	{
	  $res = "" ;
	  if ($this->m_archive!=1)
	  {
	    return text("onlyactiverecordsareshown")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
	                '&archive=1', text('clicktoviewarchive', $this->m_type, $this->m_module))."<br>";
	  }
	  else
	  {
	    return text("youareinthearchive")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
	                '&archive=0', text('clicktoviewactiverecords', $this->m_type, $this->m_module))."<br>";
	  }
	  return $res."<br>";
	}

	function action_admin(&$handler)
	{
	  global $g_sessionManager;
	  $this->m_archive = $g_sessionManager->pageVar("archive");
	
	  if ($this->m_archive==1)
	  {
	    $this->addFilter("contract.status='archived'");
	  }
	  else
	  {
	    $this->addFilter("contract.status='active'");
	  }
	
	  return $handler->action_admin();
	}

    
  /**
   * Updates contract_id in table project
   *
   * @param type name description
   * @return type description
  */
  function action_edit(&$handler)
  {
    global $g_sessionManager;			//post values will be stored in this variable until overwritten with another value.
    
    if ($g_sessionManager->pageVar("projectid")!="" && $this->m_postvars["atkselector"] != "")
    {    	
      $record = $this->selectDb($this->m_postvars["atkselector"]);
      $contractid = $record[0]["id"];
    	
    	$db = &atkGetDb();
      $db->query("UPDATE project SET contract_id = $contractid WHERE project.id = ".$this->m_postvars["projectid"]);
    }
    return $handler->action_edit();
  }


  function period_price_edit($rec)
  {
    return $this->m_attribList["period_price"]->edit($rec)." ".atkconfig("currency_symbol");
  }

  function descriptor_def()
  {
    return "[description]";
  }

  function initial_values()
  {
    return array("enddate"=>array("year"=>(date("Y")+1),
                                  "month"=>date("m"),
                                  "day"=>date("d")));
  }
  
 /* function addProjects($rec, $projects)
  {
    $db = &atkGetDb();
    
    // Add the first found match as project.
    for ($i=0, $_i=count($projects); $i<$_i; $i++)
    {
      // Only add projects that are not already linked to another deposit.
      $db->query("UPDATE project SET contract_id = ".$rec["id"]."
                       WHERE 
                         project.id = '".$projects[$i]["id"]."' 
                         AND 
                           (contract_id IS NULL OR contract_id = '')");
    }
  }*/
}

?>