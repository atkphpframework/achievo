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
 * Class for managing activities
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
function smarty_function_crmlastviewed($params, &$smarty)
{
    $eventlog = &atkGetNode("crm.eventlog");
    $userid = atkGetUserId();
    $items = $eventlog->getRecentlyViewed($userid);

    $theme = &atkinstance("atk.ui.atktheme");
    $tpl = $theme->tplPath("crm_lastviewed.tpl");
    if ($theme->tplPath("crm_lastviewed.tpl")) {
        $ui = &atkinstance("atk.ui.atkui");
        return $ui->render('crm_lastviewed.tpl', array("items" => $items));
    }
    return "no template ?";
}

?>