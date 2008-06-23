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
 * Class for managing organization rates
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class rate extends atkNode
{

  /**
   * Constructor
   *
   * @return void
   */
  function rate()
  {
    $this->atkNode("rate",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("rate_name",AF_OBLIGATORY));

    $this->setSecurityAlias("organization.organization");
    $this->setOrder("rate_name");
    $this->setTable("crm_rate");
    $this->setDescriptorTemplate('[rate_name]');
  }
}



?>