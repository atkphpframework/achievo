<?php

useattrib("atkdateattribute");

class holidays extends atkNode
{
  function holidays()
  {
    $this->atkNode("holidays"); 

    $this->add(new atkDateAttribute("holiday_date", "F d Y","d F Y",0,0, AF_PRIMARY));
    $this->add(new atkAttribute("description", AF_OBLIGATORY));
    $this->setTable("holidays");         
    $this->setOrder("holiday_date");
  }
    
}
?>