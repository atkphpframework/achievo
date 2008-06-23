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
userelation("atkonetomanyrelation");

/**
 * Class for managing scheduler groups
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_group extends atkNode
{
  function scheduler_group()
  {
    $this->atkNode("scheduler_group",NF_NO_SECURITY);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("userid",AF_HIDE));
    $this->add(new atkAttribute("name"));
    $this->add(new atkOneToManyRelation("members","scheduler.scheduler_group_member","groupid",AF_HIDE_LIST));
    
    $this->setTable("scheduler_group");
    $this->setOrder("scheduler_group.name");
  }
}

?>