<?php

// The ability to add contracts per user was sponsored by Tryllian BV, Netherlands.

class usercontracts extends atkNode
{
  function usercontracts()
  {
    $this->atkNode("usercontracts"); // this is a support module who's called directly

    $this->addAttribute(new atkAttribute("id"                                , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkManyToOneRelation("userid","employee",AF_READONLY));
    $this->addAttribute(new atkAttribute("uc_hours"                  , AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("startdate"        ,"F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("enddate"        , "F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->addAttribute(new atkTextAttribute("description"));
    $this->setTable("usercontract", "usercontract");         
    $this->setOrder("id");
  }

    function initial_values()
     {
      return array("uc_hours"=>40);
    }

}
?>
