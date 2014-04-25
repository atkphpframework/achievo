<?php
atkimport("modules.timereg.hours");

class movehours extends hours
{

    function movehours($name = "movehours", $flags = 0)
    {
        // Use the normal hours node constructor
        $this->hours($name, $flags);

        // Hide anything but the projectid, phaseid and activityid relations
        foreach (array_keys($this->m_attribList) as $attribname) {
            if (!in_array($attribname, array("projectid", "phaseid", "activityid", "javascript"))) {
                $this->m_attribList[$attribname]->addFlag(AF_HIDE);
                $this->m_attribList[$attribname]->removeFlag(AF_OBLIGATORY);
            }
        }
    }

    function allowed($action, $record = "")
    {
        return atkGetSecurityManager()->allowed("timereg.hours", "edit");
    }

    function validate($record, $mode)
    {
        // When moving hours we do not want the validation for the remark-required field.
        $this->handleValidation($record, $mode);
        return parent::validate($record, $mode);
    }

    function addDb()
    {
        // Determine the other record parameters
        $phase = decodeKeyValuePair(atkArrayNvl($this->m_postvars, "phaseid"));
        $activity = decodeKeyValuePair(atkArrayNvl($this->m_postvars, "activityid"));
        $phaseid = intval($phase["phase.id"]);
        $activityid = intval($activity["activity.id"]);
        $selector = atkArrayNvl($this->m_postvars, "atkselector", array());

        // Error if empty phaseid or activityid
        if (empty($phaseid) || empty($activityid))
            halt("No phase or activity chosen to move hours to.", "critical");

        // Error if no hours selected
        if (empty($selector))
            halt("No hours selected to move.", "critical");

        // Get a list of hours id's
        $ids = array();
        foreach ($selector as $selectoritem)
            $ids[] = intval(atkArrayNvl(array_values(decodeKeyValuePair($selectoritem)), 0));
        $idslist = implode(",", $ids);

        // Change the time registrations using the chosen phase id and activity id.
        //@Yury: hoursbase.virtual_time must be equal phase.virtual_time
        return $this->getDb()->query("
        UPDATE 
          hoursbase h, phase f 
        SET 
          h.phaseid='$phaseid', 
          h.activityid='$activityid',
          h.virtual_time = f.virtual_time
        WHERE 
          h.id IN ($idslist) 
          AND f.id='$phaseid'");
    }

    function editArray($mode = "add", $record = NULL, $forceList = "", $suppressList = "", $fieldprefix = "")
    {
        $result = parent::editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
        $selector = atkArrayNvl($this->m_postvars, "atkselector", array());
        foreach ($selector as $selectoritem)
            $result["hide"][] = '<input type="hidden" name="atkselector[]" value="' . urldecode($selectoritem) . '">';
        return $result;
    }

}

?>
