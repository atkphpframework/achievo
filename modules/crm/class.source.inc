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
 * Class for managing sources that are used by leads for example
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class source extends atkNode
{

    /**
     * Constructor
     *
     * @return void
     */
    function source()
    {
        $this->atkNode("source", NF_MRA | NF_ADD_LINK);
        $this->add(new atkAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("source_name", AF_OBLIGATORY));

        $this->setSecurityAlias("organization.organization");
        $this->setOrder("source_name");
        $this->setTable("crm_source");
        $this->setDescriptorTemplate('[source_name]');
    }

}

?>