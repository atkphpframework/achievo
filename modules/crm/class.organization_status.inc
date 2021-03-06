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
 * Class for managing organization status
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class organization_status extends atkNode
{

    /**
     * Constructor
     *
     * @return void
     */
    function organization_status()
    {
        $this->atkNode("organization_status", NF_MRA | NF_ADD_LINK);
        $this->add(new atkAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("status_name", AF_OBLIGATORY));

        $this->setSecurityAlias("organization.organization");
        $this->setOrder("status_name");
        $this->setTable("crm_organization_status");
        $this->setDescriptorTemplate('[status_name]');
    }

}

?>