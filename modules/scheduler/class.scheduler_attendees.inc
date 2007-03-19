<?php

/**
 * Module Calendar
 * Class schedule_attendee
 *
 * This class makes it possible to add attendees to a schedule. These attendees can be employees and contacts.
 * That is why the relation is linked to person.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

useattrib("atkboolattribute");
userelation("atkmanytoonerelation");

class scheduler_attendees extends atkNode
{
  function scheduler_attendees()
  {
    $this->atkNode("scheduler_attendees",NF_MRA);

    $this->add(new atkManyToOneRelation("schedule_id", "scheduler.scheduler", AF_PRIMARY|AF_HIDE));
    $this->add(new atkManyToOneRelation("person_id", "person.select_person", AF_PRIMARY|AF_LARGE));

    $this->setTable("scheduler_attendees");
    $this->setSecurityAlias("scheduler.scheduler");
  }

  function descriptor_def()
  {
    return "[person_id.lastname], [person_id.firstname]";
  }

  function postAdd($record)
  {
    // Only for employees
    if($record["person_id"]["role"]=="employee")
      $this->sendEmail("add_event",$record["schedule_id"]["id"],$record["person_id"]["id"],"date");

    return true;
  }

  function sendEmail($action,$scheduler_id,$person_id,$date)
  {
    $template = &atknew("modules.scheduler.scheduler_email_template");
    atkdebug("TEMPLATE MODULE IS: ".$template->m_module);
    $action = "add_event";
    $template->initialize($action,$person_id);
    atkdebug("TEMPLATE MODULE IS: ".$template->m_module);
    $template->loadData($action,$scheduler_id,$date);
    atkdebug("TEMPLATE MODULE IS: ".$template->m_module);
    $template->send();
  }
}

?>