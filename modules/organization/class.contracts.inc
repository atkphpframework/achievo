<?php

userelation("atkmanytoonerelation");
useattrib("atklistattribute");
useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");

class contracts extends atkNode
{
  function contracts()
  {
    $this->atkNode("contracts",NF_ADD_LINK);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("contracttype","organization.contracttype",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->add(new atkManyToOneRelation("customer","organization.organization",AF_SEARCHABLE|AF_OBLIGATORY|AF_RELATION_AUTOLINK));
    $this->add(new atkListAttribute("billing_period",array("monthly","quarterly","yearly", "per_incident"),"",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("period_price",AF_OBLIGATORY|AF_HIDE_LIST, 13, 5));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y", 0, 0, AF_OBLIGATORY));
    $this->add(new atkDateAttribute("enddate","F d Y","d F Y", 0, 0, AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description", TEXT_SMALL, AF_OBLIGATORY));

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
