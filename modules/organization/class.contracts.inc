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
    $this->addAttribute(new atkManyToOneRelation("contracttype","organization.contracttype",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->addAttribute(new atkManyToOneRelation("customer","organization.organization",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->addAttribute(new atkListAttribute("billing_period",array("monthly","quarterly","yearly", "per_incident"),"",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->addAttribute(new atkNumberAttribute("period_price",AF_OBLIGATORY|AF_HIDE_LIST, 13, 5));
    $this->addAttribute(new atkDateAttribute("startdate","F d Y","d F Y", 0, 0, AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("enddate","F d Y","d F Y", 0, 0, AF_OBLIGATORY));
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