<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage scheduler
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

atkimport("modules.email_template.email_template");

/**
 * Class for managing scheduler email templates
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
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
    return array("title","location","description","fullname","firstname","lastname","email",
                 "date","tofullname","tofirstname","tolastname","startdate","enddate",
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
                   $this->m_data["title"],SESSION_NESTED);
  }

  function _getSchedulerItem($action,$scheduler_id,$senttoowner)
  {
    $scheduler_node = &atkGetNode("scheduler.scheduler");
    $rec = $scheduler_node->selectDb("scheduler_scheduler.id = ".$scheduler_id);

    if($senttoowner)
    {
      $this->setCustomerReceiver($rec[0]["owner"]["email"],$rec[0]["owner"]["firstname"]." ".$rec[0]["owner"]["lastname"]);
    }
    $cyclusnode = atkGetNode("scheduler.scheduler_cyclus");
    return array("title"=>$rec[0]["title"],
                 "location"=>$rec[0]["location"],
                 "description"=>$rec[0]["description"],
                 "startdate"=>$this->getDate($rec[0],"start"),
                 "enddate"=>$this->getDate($rec[0],"end"),
                 "category"=>$rec[0]["category"]["description"],
                 "priority"=>$scheduler_node->getAttribute("priority")->display($rec[0]),
                 "participants"=>$this->getAttendees($rec[0]["attendees"]),
                 "owner"=>$rec[0]["owner"]["firstname"]." ".$rec[0]["owner"]["lastname"],
                 "tofullname"=>$rec[0]["owner"]["firstname"]." ".$rec[0]["owner"]["lastname"],
                 "tofirstname"=>$rec[0]["owner"]["firstname"],
                 "tolastname"=>$rec[0]["owner"]["lastname"],
                 "repetition"=>$cyclusnode->recur_info_display($rec[0]["cyclus"]),
                );


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
    foreach($record as $rec)
    {
      if($attendees!='') $attendees.=", ";
      $attendees.=$rec["person_id"]["firstname"]." ".$rec["person_id"]["lastname"];
      if($rec["person_id"]["role"]!="employee") $attendees.=" (".atktext("contact","scheduler").")";
    }
    return $attendees;
  }

}

?>