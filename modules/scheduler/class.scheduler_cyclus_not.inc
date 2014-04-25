<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage scheduler
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atknumberattribute");

/**
 * Class for managing cyclus dates that are not part of an cyclus
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage scheduler
 * @since 1.3.0
 */
class scheduler_cyclus_not extends atkNode
{

    function scheduler_cyclus_not()
    {
        $this->atkNode("scheduler_cyclus_not", NF_NO_ADD | NF_NO_EDIT | NF_NO_VIEW | NF_MRA);
        $this->add(new atkNumberAttribute("scheduler_id", AF_PRIMARY));
        $this->add(new atkNumberAttribute("date", AF_PRIMARY));
        $this->setTable("scheduler_cyclus_not");
        $this->setSecurityAlias("scheduler.scheduler");
    }

    function date_display($rec)
    {
        return date("d-m-Y H:i", $rec["date"]);
    }

    function descriptor_def()
    {
        return "[date]";
    }

}

?>