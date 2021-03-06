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
atkimport("atk.utils.atkactionlistener");

/**
 * Event listener that registers all actions and saves them into
 * the database
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class eventListener extends atkActionListener
{

    /**
     * This method handles the storage of the action in the database.
     *
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    function actionPerformed($action, $record)
    {
        $valid_actions = array("edit", "view");
        if (!in_array($action, $valid_actions))
            return true;
        $user = &getUser();
        $userid = $user[atkconfig("auth_userpk")];
        if ($userid == "")
            $userid = 0; // probably administrator
        $node = $this->m_node->atkNodeType();
        $db = &$this->m_node->getDb();
        $primarykey = $this->m_node->primaryKey($record);
        $summary = $this->m_node->descriptor($record);

        $record = array("user_id" => $userid,
            "stamp" => $db->func_now(),
            "node" => $node,
            "action" => $action,
            "primarykey" => $primarykey,
            "summary" => $summary);

        $eventlog = &atkGetNode("crm.eventlog");

        $recordlist = $eventlog->selectDb("node='$node' AND primarykey='" . $db->escapeSQL($primarykey) . "'");
        if (count($recordlist) == 1) {
            $record = $recordlist[0];

            $record["stamp"] = date("Y-m-d H:i:s");
            $record["action"] = $action;
            $record["summary"] = $summary;
            $eventlog->updateDb($record, true, array(), array("stamp", "action", "summary"));
        } else {
            $eventlog->addDb($record);
        }
        return true;
    }

}

?>