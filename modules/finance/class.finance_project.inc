<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkdummyattribute");
useattrib("atklistattribute");
useattrib("atkdateattribute");

class finance_project extends atkNode
{
  function finance_project()
  {     
    $this->atkNode("finance_project",NF_NO_ADD|NF_NO_DELETE);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkManyToOneRelation("customer","customer",AF_HIDE_SELECT|AF_READONLY));
    $this->add(new atkManyToOneRelation("coordinator","employee",AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkAttribute("description",AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0,AF_READONLY));    
    $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_READONLY));
    $this->add(new atkDummyAttribute("comment",sprintf(text("fixed_price_explain"),atkconfig("currency")).".",AF_HIDE_LIST));
    $this->add(new atkAttribute("fixed_price"));
    $this->add(new atkOneToManyRelation("rates","finance.rates","projectid",AF_HIDE_LIST));
		
    //  $this->addFilter("project.status","active");
    
		
    $this->setTable("project","project");
    $this->setOrder("name");    
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