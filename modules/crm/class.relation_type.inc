<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage crm
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Class for managing organization relation types
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class relation_type extends atkNode
{
  /**
   * Constructor
   *
   * @return void
   */
  function relation_type()
  {
    $this->atkNode("relation_type",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("type_name"));

    $this->setSecurityAlias("organization.organization");
    $this->setOrder("type_name");
    $this->setTable("crm_relation_type");
    $this->setDescriptorTemplate('[type_name]');
  }
}



?>