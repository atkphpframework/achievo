<?php

class currency extends atkNode
{
  function currency()
  {
    global $config_default_currency;
		
    $this->atkNode("currency");

    $this->addAttribute(new atkAttribute("name" ,AF_UNIQUE));
		$this->addAttribute(new atkDummyAttribute("test","1 ".atkconfig("currency_symbol")." is:",AF_HIDE_LIST));
		$this->addAttribute(new atkAttribute("value",AF_OBLIGATORY));
		$this->addAttribute(new atkAttribute("symbol",AF_PRIMARY|AF_OBLIGATORY));

    $this->setTable("currency");
    $this->setOrder("name");
    
    atkdebug("currency::currency()");    
  }


  function descriptor_def()
  {
    return "[name] ([symbol])";
  }

}
?>
