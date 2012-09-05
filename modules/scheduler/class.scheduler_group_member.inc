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
userelation("atkmanytoonerelation");

/**
 * Class for managing scheduler group members
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_group_member extends atkNode
{
  function scheduler_group_member()
  {
    $this->atkNode("scheduler_group_member",NF_NO_SECURITY);
    $this->add(new atkmanytoonerelation("groupid","scheduler.scheduler_group",AF_PRIMARY));
    $this->add(new atkmanytoonerelation("userid","employee.employee",AF_PRIMARY));
    
    $this->setTable("scheduler_group_member");
    $this->setSecurityAlias("scheduler.userpreferences");
  }
}

?>