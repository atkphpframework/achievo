<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atklistattribute");
useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdummyattribute");
useattrib("atkfuzzysearchattribute");

class contracts extends atkNode
{
  var $m_archive;

  function contracts()
  {
    $this->atkNode("contracts",NF_ADD_LINK);

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
    $this->add(new atkDummyAttribute("projectaddhint", text("fuzzyprojectaddhint", "contracts"), AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW));
    $this->add(new atkFuzzySearchAttribute("projectadd", "project.project", "addProjects", FSA_MULTISELECT));  

    $this->setTable("contract","contract");
    $this->setOrder("startdate");
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
  
  function addProjects($rec, $projects)
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
  }
}

?>