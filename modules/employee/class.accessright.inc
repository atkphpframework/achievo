<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage notes
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atknumberattribute");

/**
 * This is a 'dummy' node which can only be used for retrieval
 * of access information and installing the database tables.
 * It's not intended as an editor for rights. Use the profile
 * editor if you want to edit rights.
 * The readonly flag is set to prevent accidental editing through
 * this class.
 *
 * @package achievo
 * @subpackage employee
 */
class accessright extends atkNode
{

    /**
     * Constuctor
     */
    function accessright()
    {
        $this->atkNode("accessright", NF_READONLY);

        $this->add(new atkAttribute("node", AF_PRIMARY, 100));
        $this->add(new atkAttribute("action", AF_PRIMARY, 25));
        $this->add(new atkNumberAttribute("role_id", AF_PRIMARY, 10));

        $this->setTable("accessright");
    }

}

?>