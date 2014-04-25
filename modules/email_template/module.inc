<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage email_template
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Module file for the menu items and security
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class mod_email_template extends achievoModule
{

    function getNodes()
    {
        
    }

    function getMenuItems()
    {
        
    }

}

// Add plugins dir for the resource plugin
atkimport("atk.ui.atksmarty");
atkSmarty::addPluginDir(moduleDir("email_template") . "plugins/");
?>
