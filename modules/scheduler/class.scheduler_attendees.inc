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
    $this->atkNode("scheduler_attendees");

    $this->add(new atkManyToOneRelation("schedule_id", "scheduler.scheduler", AF_PRIMARY|AF_HIDE));    
    $this->add(new atkManyToOneRelation("person_id", "person.select_person", AF_PRIMARY|AF_LARGE));
    $this->add(new atkBoolAttribute("sendmail"));

    $this->setTable("scheduler_attendees");
    $this->setSecurityAlias("scheduler.scheduler");
  }
}

?>