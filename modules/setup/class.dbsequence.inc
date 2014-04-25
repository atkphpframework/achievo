<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage setup
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atknumberattribute");

/**
 * Class file for the dbsequence table
 *
 * @package achievo
 * @subpackage setup
 */
class dbsequence extends atkNode
{

    function dbsequence()
    {
        $this->atkNode("dbsequence", NF_READONLY);

        $this->add(new atkAttribute("seq_name", AF_PRIMARY, 50));
        $this->add(new atkNumberAttribute("nextid", AF_OBLIGATORY));

        $this->setTable("db_sequence");
    }

}

?>