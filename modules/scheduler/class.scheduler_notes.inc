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

/**
 * This class makes it possible to add notes to a schedule.
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */

useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");

class scheduler_notes extends atkNode
{
  function scheduler_notes()
  {
    $this->atkNode("scheduler_notes");

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_HIDE_ADD|AF_READONLY|AF_OBLIGATORY));
    $this->add(new atkNumberAttribute("scheduler_id", AF_HIDE|AF_OBLIGATORY|AF_FORCE_LOAD));
    $this->add(new atkDateAttribute("entrydate", "F d Y","d F Y", date("Ymd"), 0, AF_HIDE_EDIT|AF_HIDE_ADD|AF_OBLIGATORY));
    $this->add(new atkAttribute("title", AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description", TEXT_LARGE, AF_OBLIGATORY));

    $this->setTable("scheduler_notes", "scheduler_notes");
    $this->setOrder("id");
    $this->setSecurityAlias("scheduler.scheduler");
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