<?php

// The ability to add contracts per user was sponsored by Tryllian BV, Netherlands.

userelation("atkmanytoonerelation");
useattrib("atkdateattribute");
useattrib("atktextattribute");

class usercontracts extends atkNode
{
  function usercontracts()
  {
    $this->atkNode("usercontracts"); // this is a support module who's called directly

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("userid","employee",AF_READONLY));
    $this->add(new atkAttribute("uc_hours", AF_OBLIGATORY));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->add(new atkDateAttribute("enddate", "F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"));
    $this->setTable("usercontract", "usercontract");         
    $this->setOrder("id");
  }

  function initial_values()
  {
    return array("uc_hours"=>40,
                 "enddate"=>array("year"=>(date("Y")+1),
                                  "month"=>date("m"),
                                  "day"=>date("d")));
  }

}

?>