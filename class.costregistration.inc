<?php

include_once "class.projectattrib.inc";  

  class costregistration extends atkNode
  {
    function costregistration()
    {
      global $g_user;

      $this->atkNode("costregistration"); // node() constructor is *not* called automatically!      
      $this->addFilter("userid",strtolower($g_user["name"]));
  
      $this->add(new atkAttribute("id", AF_PRIMARY|AF_UNIQUE|AF_AUTO_INCREMENT|AF_HIDE));
      $this->add(new atkAttribute("userid", AF_READONLY));
      $this->add(new atkDateAttribute("costdate","F d Y","d F Y", 0, date("Ymd"),AF_OBLIGATORY));
      $this->add(new atkProjectAttribute("projectid", AF_OBLIGATORY|AF_SEARCHABLE));
      
      $this->add(new atkManyToOneRelation("currency","finance.currency"));
      $this->add(new atkAttribute("value", AF_OBLIGATORY));
      $this->add(new atkListAttribute("paymethod",array("cash","electronic payment","creditard"), array("cash","electronic payment","creditcard"), AF_OBLIGATORY));
      $this->add(new atkAttribute("description", AF_OBLIGATORY));
      
      $this->setOrder("costdate");
            
      $this->setTable("costregistration", "costregistration");
    }

    function initial_values()
    {
      global $g_user;
      return array("userid"=>strtolower($g_user["name"]));  
    }

    function value_display($rec)
    {
      $value = $rec["value"];
      $value = round($value, 2);
      return "$value";
    }

    function descriptor_def()
    {
      return "[description]";
    }
  }

?>
