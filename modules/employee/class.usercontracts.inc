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
    $this->add(new atkManyToOneRelation("userid","employee.employee",AF_READONLY|AF_OBLIGATORY));
    $this->add(new atkNumberAttribute("uc_hours", AF_OBLIGATORY, 3));
    $this->add(new atkDateAttribute("startdate","F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->add(new atkDateAttribute("enddate", "F d Y","d F Y",0,0, AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description", TEXT_SMALL, 0));
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
  
  /**
   * We override the hours edit, because it is the first field in the edit screen.
   * This override is then used to set some values for various fields, depending
   * on the previous contract of that employee (if any):
   * - uc_hours is set to the uc_hours of the previous contract
   * - workingdays is set to the workingdays of the previous contract
   * - startdate is set to the enddate of the previous contract
   * - enddate is set to startdate + 1 year.
   */
  function uc_hours_edit(&$record, $prefix, $mode)
  {
    if ($mode=="add")
    {
      if ($record["userid"]["id"]!="")
      {        
       $previous = $this->selectDb("userid=".$record["userid"]["id"], "enddate DESC", 1, "", array("enddate", "workingdays", "uc_hours"));
       
        if (count($previous))
        {    
        
          if (is_array($previous[0]["enddate"]))
          {            
            $year = $previous[0]["enddate"]["year"];
            $month = $previous[0]["enddate"]["month"];
            $day = $previous[0]["enddate"]["day"];
            $record["startdate"] = array("year"=>$year,
                                         "month"=>$month,
                                         "day"=>$day);
                                         
            // We also set enddate now, to startdate+1 year.
            $endstamp = mktime(12, 0, 0, $month, $day, $year+1);
            $record["enddate"] = array("year"=>date("Y", $endstamp),
                                       "month"=>date("m", $endstamp),
                                       "day"=>date("d", $endstamp)); 
          }
          // and while we're at it, we should also set the workingdays
          // to the workingdays of the previous contract.
          if (!empty($previous[0]["workingdays"]))
          {
            $record["workingdays"] = $previous[0]["workingdays"];
          }
          
          // and also the uc_hours.
          if (!empty($previous[0]["uc_hours"]))
          {
            $record["uc_hours"] = $previous[0]["uc_hours"];
          }
        }        
      }
    }
    // We have only implemented this method to set some values. We should
    // call the regular edit method now.
    return $this->m_attribList["uc_hours"]->edit($record, $prefix, $mode);
  }

}

?>
