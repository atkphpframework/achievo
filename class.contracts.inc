<?php

userelation("atkmanytoonerelation");
useattrib("atklistattribute");
useattrib("atkdateattribute");
useattrib("atktextattribute");

class contracts extends atkNode
{
  function contracts()
  {
    $this->atkNode("contracts",NF_ADD_LINK);

    $this->addAttribute(new atkAttribute("id",AF_AUTOKEY));
    $this->addAttribute(new atkManyToOneRelation("contracttype","contracttype",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));    
    $this->addAttribute(new atkManyToOneRelation("customer","customer",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));    
    $this->addAttribute(new atkListAttribute("billing_period",array(text("monthly"),text("quarterly"),text("yearly"),text("per_incident")),array(),AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("period_price",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y", 0, 0,AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("enddate","F d Y","d F Y", 0, 0,AF_OBLIGATORY));    
    $this->addAttribute(new atkTextAttribute("description",AF_OBLIGATORY));    
    
    $this->setTable("contract","contract");
    $this->setOrder("startdate");
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
  
}

?>