<?php

class currency extends atkNode
{
  function currency()
  {
    $this->atkNode("currency");

    $this->addAttribute(new atkAttribute("symbol",AF_PRIMARY|AF_OBLIGATORY));
    $this->addAttribute(new atkAttribute("name" ,AF_UNIQUE));
		$this->addAttribute(new atkDummyAttribute("label","1 ".atkconfig("currency_symbol")." ".text("isworth").":",AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("value",AF_OBLIGATORY));

    $this->setTable("currency");
    $this->setOrder("name");
    
  }


  function descriptor_def()
  {
    return "[name] ([symbol])";
  }

}
?>
