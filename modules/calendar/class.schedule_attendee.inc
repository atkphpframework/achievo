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

userelation("atkmanytoonerelation");

class schedule_attendee extends atkNode
{
  function schedule_attendee()
  {
    $this->atkNode("schedule_attendee");

    $this->add(new atkManyToOneRelation("schedule_id", "calendar.schedule", AF_PRIMARY));
    $personrel = new atkManyToOneRelation("person_id", "calendar.schedule_person", AF_PRIMARY|AF_LARGE);
    $this->add($personrel);

    $this->setTable("schedule_attendee", "schedule_attendee");
    $this->setSecurityAlias("calendar.schedule");
  }
}

?>