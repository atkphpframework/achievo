<?php

class finance_project extends atkNode
{
  function finance_project()
  {
   
    
    $this->atkNode("finance_project",NF_NO_ADD);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE|AF_READONLY));
    $this->addAttribute(new atkManyToOneRelation("customer","customer",AF_HIDE_SELECT|AF_HIDE_ADD|AF_READONLY));
    $this->addAttribute(new atkManyToOneRelation("coordinator","employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_READONLY));
    $this->addAttribute(new atkTextAttribute("description",AF_HIDE_LIST|AF_HIDE_ADD|AF_READONLY));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_HIDE_ADD|AF_READONLY));    
    $this->addAttribute(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_READONLY));
    $this->addAttribute(new atkDummyAttribute("comment","Fixed price is in ".atkconfig("currency").", when no fixed price is defined, hour rates will apply.",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("fixed_price"));
    $this->addAttribute(new atkOneToManyRelation("Ratings","finance.rates","project_id",AF_HIDE_LIST));
		
    //  $this->addFilter("project.status","active");
    
		
    $this->setTable("project","project");
    $this->setOrder("name");

    atkdebug("finance_project::finance_project()");
  }

  function fixed_price_display($rec)
	{
	  if(is_null($rec["fixed_price"])||$rec["fixed_price"]==0)  { return "Hour Rates"; } else { return $this->m_attribList["fixed_price"]->display($rec); } 
	}


  function descriptor_def()
  {
    return "[name]";
  }
  
}
?>