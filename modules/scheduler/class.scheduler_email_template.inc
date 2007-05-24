<?php

atkimport("modules.email_template.email_template");
class scheduler_email_template extends email_template
{
  function scheduler_email_template()
  {
    $this->m_module = "scheduler";
    $this->m_from_email = "scheduler@achievo.org";
    $this->m_from_name = "Achievo Scheduler";

    $this->email_template("scheduler_email_template");
    $this->addFilter("email_template.module='scheduler'");
    $this->setModule('scheduler');
    $this->setFromEmail("scheduler@achievo.org");
    $this->setFromName("Achievo Scheduler");
  }


  function getValidCodes()
  {
    return array("title","location","description","fullname","firstname","lastname","e-mail",
                 "date","to-fullname","to-firstname","to-lastname","startdate","enddate",
                 "priority","participants","category","olddate","owner","repetition","action",
                 "link");
  }

  function loadData($action,$scheduler_id,$date,$trigger_action="",$senttoowner=false)
  {
    $user = atkGetUser();
    if($trigger_action=="") $trigger_action=$action;
    $this->m_data = $this->_getSchedulerItem($action,$scheduler_id,$senttoowner);
    $this->m_data["fullname"]=$user["firstname"]." ".$user["lastname"];
    $this->m_data["firstname"]=$user["firstname"];
    $this->m_data["lastname"]=$user["lastname"];
    $this->m_data["email"]=$user["email"];
    $this->m_data["date"]=date("d-m-Y");
    $this->m_data["action"]=atktext($trigger_action,"scheduler");
    $this->m_data["link"]=href(dispatch_url("scheduler.scheduler","view",array("atkselector"=>"scheduler_scheduler.id=".$scheduler_id,"scheduler_id"=>$scheduler_id,"itemdate"=>$date)),
                   $item["title"],SESSION_NESTED);
  }

  function _getSchedulerItem($action,$scheduler_id,$senttoowner)
  {
    $scheduler_node = &atkGetNode("scheduler.scheduler");
    $rec = $scheduler_node->selectDb("scheduler_scheduler.id = ".$scheduler_id);
    
/*
    
{$olddate}: Old startdate - scheduler
{$action}: Action that caused the notify: Added, Accepted, Canceled,.... - scheduler
{$link}: Link to view the event - scheduler
*/   
    if($senttoowner)
    {
      $this->setCustomerReceiver($rec[0]["owner"]["email"],$rec[0]["owner"]["firstname"]." ".$rec[0]["owner"]["lastname"]);
    }
    return array("title"=>$rec[0]["title"],
                 "location"=>$rec[0]["location"],
                 "description"=>$rec[0]["description"],
                 "startdate"=>$this->getDate($rec[0],"start"),
                 "endddate"=>$this->getDate($rec[0],"end"),
                 "category"=>$rec[0]["category"]["description"],
                 "priority"=>$rec[0]["priority"],
                 "participants"=>$this->getAttendees($rec[0]["attendees"]),
                 "owner"=>$rec[0]["owner"]["firstname"]." ".$rec[0]["owner"]["lastname"],
                 "to-fullname"=>$rec[0]["owner"]["firstname"]." ".$rec[0]["owner"]["lastname"],
                 "to-firstname"=>$rec[0]["owner"]["firstname"],
                 "to-lastname"=>$rec[0]["owner"]["lastname"],
                 "repetition"=>$this->getRecurringInfo($rec[0]),
                );


  }

  function getRecurringInfo($rec)
  {
    if($rec["recur"]=="once") return '';
    if($rec["recur"]=="daily")
    {

    }
    elseif($rec["recur"]["weekly"])
    {

    }

  }

  function getDate($rec,$type)
  {
    if($rec["allday"]==1)
    {
      $time = mktime(12,0,0,$rec[$type."date"]["month"],$rec[$type."date"]["day"],$rec[$type."date"]["year"]);
    }
    else
    {
      $time = mktime($rec[$type."time"]["hours"],$rec[$type."time"]["minutes"],0,$rec[$type."date"]["month"],$rec[$type."date"]["day"],$rec[$type."date"]["year"]);
    }
    $date = date("d-m-Y H:i",$time);
    return $date;
  }

  function getAttendees($record)
  {
    $attendees="";
    foreach($record as $key=>$rec)
    {
      if($attendees!='') $attendees.=", ";
      $attendees.=$rec["person_id"]["firstname"]." ".$rec["person_id"]["lastname"];
      if($rec["person_id"]["role"]!="employee") $attendees.=" (".atktext("contact","scheduler").")";
    }
    return $attendees;
  }

}
/*
array(1) {
  [0]=>
  array(25) {
    ["id"]=>
    string(5) "54870"
    ["attendees"]=>
    ["title"]=>
    string(9) "Recurring"
    ["location"]=>
    string(10) "Vlissingen"
    ["description"]=>
    string(16) "Nice appointment"
    ["recur"]=>
    string(5) "daily"
    ["startdate"]=>
    array(3) {
      ["year"]=>
      string(4) "2007"
      ["month"]=>
      string(2) "02"
      ["day"]=>
      string(2) "18"
    }
    ["enddate"]=>
    array(3) {
      ["year"]=>
      string(4) "2007"
      ["month"]=>
      string(2) "02"
      ["day"]=>
      string(2) "18"
    }
    ["allday"]=>
    string(1) "0"
    ["starttime"]=>
    array(3) {
      ["hours"]=>
      string(2) "03"
      ["minutes"]=>
      string(2) "00"
      ["seconds"]=>
      string(2) "00"
    }
    ["endtime"]=>
    array(3) {
      ["hours"]=>
      string(2) "04"
      ["minutes"]=>
      string(2) "00"
      ["seconds"]=>
      string(2) "00"
    }
    ["cyclus"]=>
    array(11) {
      ["scheduler_id"]=>
      string(5) "54870"
      ["cyclus_enddate"]=>
      array(3) {
        ["year"]=>
        string(4) "2007"
        ["month"]=>
        string(2) "02"
        ["day"]=>
        string(2) "23"
      }
      ["startday"]=>
      string(1) "1"
      ["endday"]=>
      string(1) "1"
      ["startmonth"]=>
      string(1) "1"
      ["endmonth"]=>
      string(1) "1"
      ["every"]=>
      string(1) "1"
      ["month_time"]=>
      string(1) "1"
      ["weekday"]=>
      array(0) {
      }
      ["exceptions"]=>
      array(1) {
        [0]=>
        array(3) {
          ["scheduler_id"]=>
          string(5) "54870"
          ["date"]=>
          string(10) "1171850400"
          ["atkprimkey"]=>
          string(84) "scheduler_cyclus_not.scheduler_id='54870' AND scheduler_cyclus_not.date='1171850400'"
        }
      }
      ["atkprimkey"]=>
      string(37) "scheduler_cyclus.scheduler_id='54870'"
    }
    ["category"]=>
    array(5) {
      ["id"]=>
      string(3) "164"
      ["description"]=>
      string(49) "Bijeenkomst met externe relatie (hier/op locatie)"
      ["bgcolor"]=>
      string(7) "#ffcc33"
      ["fgcolor"]=>
      string(7) "#444444"
      ["example"]=>
      NULL
    }
    ["private"]=>
    string(1) "0"
    ["all_users"]=>
    string(1) "0"
    ["priority"]=>
    string(1) "0"
    ["nonblocking"]=>
    string(1) "0"
    ["notes"]=>
    array(0) {
    }
    ["owner"]=>
    array(31) {
      ["id"]=>
      string(3) "366"
      ["title_id"]=>
      NULL
      ["userid"]=>
      string(5) "sandy"
      ["lastname"]=>
      string(6) "Pleyte"
      ["firstname"]=>
      string(5) "Sandy"
      ["initials"]=>
      string(0) ""
      ["address"]=>
      NULL
      ["zipcode"]=>
      NULL
      ["city"]=>
      NULL
      ["state"]=>
      NULL
      ["country"]=>
      NULL
      ["phone"]=>
      NULL
      ["cellular"]=>
      string(14) "06 42 59 40 69"
      ["fax"]=>
      string(0) ""
      ["email"]=>
      NULL
      ["function"]=>
      NULL
      ["remark"]=>
      NULL
      ["role"]=>
      NULL
      ["vcard"]=>
      NULL
      ["birthdate"]=>
      string(0) ""
      ["bankaccount"]=>
      NULL
      ["socialsecuritynumber"]=>
      NULL
      ["functionlevel"]=>
      NULL
      ["department"]=>
      NULL
      ["creditor_id"]=>
      NULL
      ["profiles"]=>
      NULL
      ["supervisor"]=>
      NULL
      ["status"]=>
      string(6) "active"
      ["usercontract"]=>
      NULL
      ["passwordexplanation"]=>
      NULL
      ["password"]=>
      array(1) {
        ["hash"]=>
        NULL
      }
    }
    ["owner_attendee"]=>
    string(1) "0"
    ["lastdate"]=>
    string(10) "1172199600"
    ["atkprimkey"]=>
    string(30) "scheduler_scheduler.id='54870'"
  }
}
*/

/**
$$Fullname$$: name of the user, eg. "[] admin admin"
$$Firstname$$: first name of the user, eg. "admin"
$$Lastname$$: last name of the user, eg. "admin"
$$Domain$$: domain name for mail-address, eg. ""
$$E-Mail$$: email-address of the user, eg. "admin@"
$$Date$$: todays date, eg. "2006/09/22"
$$to-fullname$$: Fullname of person to notify
$$to-firstname$$: Firstname of person to notify
$$to-lastname$$: Lastname of person to notify

$$Title$$: Title of the event
$$Description$$: Description
$$startdate$$: Start Date/Time
$$Enddate$$: End Date/Time
$$olddate$$: Old Startdate
$$Category$$: Category
$$Location$$: Location
$$Priority$$: Priority
$$Participants$$: Participants
$$Owner$$: Owner
$$Repetition$$: Repetitiondetails (or empty)
$$Action$$: Action that caused the notify: Added, Canceled, Accepted, Rejected, ...
$$link$$: Link to view the event
$$Disinvited$$: Participants disinvited from an event

 */
?>