<?php

useattrib("atkdummyattribute");

class currency extends atkNode
{
  function currency()
  {
    $this->atkNode("currency");

    $this->add(new atkAttribute("symbol",AF_PRIMARY|AF_OBLIGATORY));
    $this->add(new atkAttribute("name" ,AF_UNIQUE));
		$this->add(new atkDummyAttribute("label","1 ".atkconfig("currency_symbol")." ".text("isworth").":",AF_HIDE_LIST));
    $this->add(new atkAttribute("value",AF_OBLIGATORY));

    $this->setTable("currency");
    $this->setOrder("name");
  }

  function descriptor_def()
  {
    return "[name] ([symbol])";
  }

}

?>