<?php

// The ability to add contracts per user was sponsored by Tryllian BV, Netherlands.

userelation("atkmanytoonerelation");
useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkmultiselectattribute");

class usercontracts extends atkNode
{
  function usercontracts()
  {
    $this->atkNode("usercontracts"); // this is a support module who's called directly

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("userid","employee.employee",AF_READONLY));
    $this->add(new atkNumberAttribute("uc_hours", AF_OBLIGATORY, 3));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->add(new atkDateAttribute("enddate", "F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"));
    $this->add(new atkMultiSelectAttribute("workingdays",array("mon", "tue","wed","thu","fri","sat","sun"),array(1,2,3,4,5,6,0),AF_HIDE_LIST));
    $this->setTable("usercontract");
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
