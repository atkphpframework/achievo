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

userelation("atkmanytoonerelation");

/**
 * Class for managing company former names
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class former_name extends atkNode
{
  /**
   * Constructor
   *
   * @return void
   */
  function former_name()
  {
    $this->atkNode("former_name",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("former_name",AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("account","organization.organization",AF_RELATION_AUTOLINK));

    $this->setSecurityAlias("organization.organization");
    $this->setOrder("former_name");
    $this->setTable("crm_former_name");
    $this->setDescriptorTemplate('[former_name]');
  }

}

?>