<?php

// The ability to add contracts per user was sponsored by Tryllian BV, Netherlands.

class usercontracts extends atkNode
{
  function usercontracts()
  {
    $this->atkNode("usercontracts",NF_NO_SECURITY); // this is a support module who's called directly

    $this->addAttribute(new atkAttribute("uc_id"                                , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkManyToOneRelation("uc_userid","employee",AF_READONLY));
    $this->addAttribute(new atkAttribute("uc_descr"                  , AF_OBLIGATORY));
    $this->addAttribute(new atkAttribute("uc_hours"                  , AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("uc_startdate"        ,"F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("uc_enddate"        , "F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->setTable("usercontract", "usercontract");         
    $this->setOrder("uc_id");
  }
    function initial_values()
     {
      return array("uc_hours"=>40);
    }

}
?>
