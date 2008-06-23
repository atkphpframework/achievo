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
useattrib("atknumberattribute");

/**
 * Class for managing scheduler dates, is only used internal
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_dates extends atkNode
{
  function scheduler_dates()
  {
    $this->atkNode("scheduler_dates");
    $this->add(new atkNumberAttribute("scheduler_id",AF_PRIMARY));
    $this->add(new atkNumberAttribute("startdate",AF_PRIMARY));
    $this->add(new atkNumberAttribute("enddate"));
    $this->setTable("scheduler_dates");
    $this->setSecurityAlias("scheduler.scheduler");
  }
}


?>