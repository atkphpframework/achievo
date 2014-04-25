<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Class for managing activities
 *
 * @package achievo
 * @subpackage project
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license doc/LICENSE Achievo Open Source License
 *
 * @version $revision$
 * $Id$
 */
useattrib("atkboolattribute");
useattrib("atknumberattribute");

class activity extends atkNode
{

    function activity()
    {
        $this->atkNode("activity", NF_MRA);

        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("name", AF_OBLIGATORY | AF_UNIQUE | AF_SEARCHABLE, 50));
        $this->add(new atkAttribute("description", AF_SEARCHABLE, 50));
        $this->add(new atkBoolAttribute("remarkrequired"));
        $this->add(new atkBoolAttribute("overtimecompensation"));

        $this->setTable("activity", "activity");
        $this->setOrder("name");

        atkdebug("activity::activity()");
    }

    function descriptor_def()
    {
        return "[name]";
    }

    function action_stats(&$handler)
    {
        include_once moduleDir("project") . "activitystats.inc";
    }

    function postAdd(&$rec)
    {
        return $this->removeCountCache();
    }

    function postUpdate($rec)
    {
        return $this->removeCountCache();
    }

    function postDelete($rec)
    {
        return $this->removeCountCache();
    }

    /**
     * Remove activityCount key from cache
     * @return boolean
     */
    protected function removeCountCache()
    {
        $cache = atkcache::getInstance();
        return $cache->delete('activityCount');
    }

}

?>