<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage pim
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
$modifiers[] = "employee.userprefs";

/**
 * Module file for the menu items and security
 *
 * @package achievo
 * @subpackage pim
 */
class mod_pim extends achievoModule
{

    function getNodes()
    {
        
    }

    function getMenuItems()
    {
        
    }

    function pdaNodes()
    {
        return array("pim" => dispatch_url("pim.pim", "pim"));
    }

    function userprefs_modifier(&$node)
    {
        userelation("atkOneToManyRelation");

        $node->add(new atkOneToManyRelation("pimitems", "pim.userpimitems", "userid", AF_FORCE_LOAD), "pim");
    }

}

?>