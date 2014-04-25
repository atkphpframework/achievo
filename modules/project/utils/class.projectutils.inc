<?php

class projectutils
{

    function getAvailableActivityIds($phaseid)
    {
        if (!isset($phaseid))
            return array();

        $phaseactivityNode = &atkGetNode("project.phase_activity");
        $selector = sprintf("phaseid='%s'", $phaseid);
        $recs = $phaseactivityNode->selectDb($selector);

        //get the ids
        $ids = array();
        foreach ($recs as $rec) {
            if (isset($rec["activityid"]["id"]) && $rec["activityid"]["id"] != '' && $rec["activityid"]["id"] != 0)
                $ids[] = $rec["activityid"]["id"];
        }

        return $ids;
    }

    /**
     * Determine the number of available activities
     */
    function getActivityCount()
    {
        $cache = atkcache::getInstance();
        $activityCount = $cache->get('activityCount');
        if (!$activityCount) {
            $db = &atkGetDb();
            $result = $db->getrows("SELECT count(id) as activitycount FROM activity");
            $cache->set('activityCount', $result[0]['activitycount'], DAY);
            $activityCount = $result[0]['activitycount'];
        }
        return $activityCount;
    }

}

?>