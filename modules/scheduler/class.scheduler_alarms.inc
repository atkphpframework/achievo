<?php
useattrib("atknumberattribute");
useattrib("atkupdatedbyattribute");
useattrib("atklistattribute");
useattrib("scheduler.schedulerdurationattribute");

class scheduler_alarms extends atkNode
{
  function scheduler_alarms()
  {
    $users = $this->getUsers();
    $this->atkNode("scheduler_alarms");
    $this->add(new atkNumberAttribute("scheduler_id",AF_PRIMARY|AF_HIDE));
    $this->add(new atkNumberAttribute("startdate",AF_PRIMARY|AF_READONLY));
    $this->add(new schedulerDurationAttribute("duration"));
    $this->add(new atkNumberAttribute("senddate",AF_HIDE));
    $this->add(new atkListAttribute("userid", array_values($users),array_keys($users),AF_PRIMARY));
    $this->add(new atkUpdatedByAttribute("owner"));
    $this->setTable("scheduler_alarms");
    $this->setOrder("startdate,duration");
    $this->addFilter("startdate>=".time());
    $this->setSecurityAlias("scheduler.scheduler");
  }
  
  function initial_values()
  {
    $sessionmanager = &atkGetSessionManager();
    $startdate = $sessionmanager->stackVar("itemdate");

    $ret = array("startdate"=>$startdate,
                 "userid"=>-1,
                 "duration"=>60);
    
    return $ret;
  }
  
  function startdate_display($rec)
  {
    return date("d-m-Y H:i",$rec["startdate"]);
  }
  
  function preAdd(&$rec)
  {
    // Pre calculate the senddate
    $rec["senddate"] = ($rec["startdate"]-$rec["duration"]);
  }
  
  function preUpdate(&$rec)
  {
    // Pre calculate the senddate
    $rec["senddate"] = ($rec["startdate"]-$rec["duration"]);
  }
  
  function getUsers()
  {
    $db = &atkGetDb();
    $sql = "SELECT id,lastname,firstname FROM person WHERE role='employee' ORDER BY lastname,firstname";
    $nrows = $db->getRows($sql);
    $users = array("-1"=>atktext("all_invited_employees"));
    
    foreach($nrows as $row)
    {
      $users[$row['id']] = $row['lastname'].", ".$row["firstname"];
    }
    return $users;
  }

}
?>