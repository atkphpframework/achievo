<?php

/**
 * Module Calendar
 * Class schedule_notes
 *
 * This class makes it possible to add notes to a schedule.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

useattrib("atkdateattribute");
useattrib("atktextattribute");
userelation("atkmanytoonerelation");

class schedule_notes extends atkNode
{
  function schedule_notes()
  {
    $this->atkNode("schedule_notes");

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_HIDE_ADD|AF_READONLY|AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("schedule_id", "calendar.schedule", AF_HIDE|AF_OBLIGATORY));
    $this->add(new atkDateAttribute("entrydate", "F d Y","d F Y", date("Ymd"), 0, AF_HIDE_EDIT|AF_HIDE_ADD|AF_OBLIGATORY));
    $this->add(new atkAttribute("title", AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkTextAttribute("description", TEXT_LARGE, AF_OBLIGATORY));

    $this->setTable("schedule_note", "schedule_note");
    $this->setOrder("id");
    $this->setSecurityAlias("calendar.schedule");
  }

  function initial_values()
  {
    global $g_user;
    return array("owner"=>array("id"=>$g_user["id"]),
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
  }

  function descriptor_def()
  {
    return "[title]";
  }
}
?>
