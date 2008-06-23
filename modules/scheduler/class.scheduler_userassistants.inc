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
 * Class for managing scheduler user assistants
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_userassistants extends atkNode
{
   function scheduler_userassistants()
   {
     $this->atkNode("scheduler_userassistants");
     $this->add(new atkManyToOneRelation("userid","scheduler.scheduler_userpreferences",AF_PRIMARY));
     $this->add(new atkManyToOneRelation("employeeid","employee.employee",AF_PRIMARY));
     $this->setSecurityAlias("scheduler.scheduler");

     $this->setTable("scheduler_userassistants");
   }
}

?>