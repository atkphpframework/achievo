<?php

class holidays extends atkNode
{
  function holidays()
  {
    $this->atkNode("holidays",NF_NO_SECURITY); // this is a support module who's called directly

    $this->addAttribute(new atkDateAttribute("holiday_date", "F d Y","d F Y",0,0, AF_PRIMARY));
    $this->addAttribute(new atkAttribute("holiday_desc"                  , AF_OBLIGATORY));
    $this->addAttribute(new atkBoolAttribute("holiday_work"                  ));
    $this->setTable("holidays");         
    $this->setOrder("holiday_date");
  }
    
}
?>
