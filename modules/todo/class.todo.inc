<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage todo
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
require_once("achievotools.inc");
userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atkupdatestampattribute");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkboolattribute");

/**
 * Class for managing the todo's
 *
 * @package achievo
 * @subpackage todo
 */
class todo extends atkNode
{
    var $prioId = array(1, 2, 3, 4, 5);
    var $prioName = array("highest", "high", "normal", "low", "lowest");

    /**
     * Constructor
     *
     * @param string $name Node name, default todo
     * @return void
     */
    function todo($name = "todo")
    {
        $this->atkNode($name, NF_TRACK_CHANGES | NF_MRA | NF_NO_EXTENDED_SEARCH);

        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_READONLY | AF_HIDE | AF_OBLIGATORY));
        $this->add(new atkManyToOneRelation("projectid", "project.project", AF_SEARCHABLE | AF_LARGE));
        $this->add(new atkManyToOneRelation("contactid", "organization.contact", AF_SEARCHABLE | AF_LARGE));
        $this->add(new atkAttribute("title", AF_OBLIGATORY, 50));
        $p_attr = &$this->add(new atkManyToOneRelation("assigned_to", "employee.employee", AF_SEARCHABLE));
        $p_attr->setDestinationFilter("(status='active' OR userid='[assigned_to.id]')");
        $this->add(new atkDateAttribute("entrydate", "", "", 0, 0, AF_READONLY_EDIT | AF_HIDE_ADD | AF_OBLIGATORY));
        $this->add(new atkDateAttribute("duedate", "", "", 0, 0, AF_OBLIGATORY));
        $this->add(new atkDateAttribute("closedate", "", "", 0, 0));
        $this->add(new atkUpdateStampAttribute("updated", AF_HIDE | AF_FORCE_LOAD)); // force_load is necessary, because we need the value when copying the todo to the history table.
        $this->add(new atkListAttribute("priority", $this->prioName, $this->prioId, AF_OBLIGATORY));
        $this->add(new atkListAttribute("completed", array("0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%"), array(0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100), AF_NO_TRANSLATION | AF_LIST_NO_NULL_ITEM));
        $this->add(new atkBoolAttribute("private"));
        $this->add(new atkTextAttribute("description", TEXT_LARGE, AF_HIDE_LIST));
        $this->add(new atkListAttribute("status", array("todo_new", "todo_rejected", "todo_in_progress", "todo_on_hold", "todo_completed"), array(1, 2, 3, 4, 5), AF_SEARCHABLE | AF_OBLIGATORY));
        if ($name == "todo")
            $this->add(new atkOneToManyRelation("history", "todo.todo_history", "todoid", AF_HIDE_LIST | AF_CASCADE_DELETE), 'history');

        $userid = atkArrayNvl(getUser(), "id");
        if ($name == "todo_history") {
            $this->addFilter("([table].private=0 OR ([table].private=1 and [table].owner='" . $userid . "'))");
        } else {
            $this->addFilter("(([table].private=0) OR ([table].private is null) OR ([table].private=1 and ([table].owner='" . $userid . "' OR [table].assigned_to='" . $userid . "')))");
        }
        $this->addSecurityMap(array("complete", "mra_complete", "refresh_todos"), "edit");
        //$this->addSecurityMap('refresh_todos',"admin");
        $this->setTable("todo");
        $this->setOrder("todo.status, todo.duedate, todo.priority");
    }

    public function renderPim()
    {
        $todonode = atkGetNode('todo.todo');
        $todonode->addFilter("todo.status NOT IN (5,2)");
        $todonode2 = clone($todonode);
        $userid = atkGetUserId();

        atkimport('atk.datagrid.atkdatagrid');
        $todonode->addFilter("todo.assigned_to='$userid' AND todo.assigned_to <> 0  AND ((todo.private=0) OR (todo.private is null) OR (todo.private=1 and todo.owner='$userid'))");
        $grid = atkDataGrid::create($todonode, 'todoassignedtome');
        $grid->setEmbedded(true);
        $grid->setBaseUrl(partial_url('todo.todo', 'refresh_todos', 'refresh_todos'));
        $todo_assignedtome = $grid->render();
        $todo_assignedtome.= '<br>' . href(dispatch_url("todo.todo", "add", array("atkfilter" => "assigned_to.id='$userid'")), atktext('add'), SESSION_NESTED);

        $todo_assignedbyme = atktext("pim_assignedbyyou") . ":<br>";

        $todonode2->addFilter("todo.owner='$userid' AND todo.assigned_to<>'$userid'");
        $grid2 = atkDataGrid::create($todonode2, 'todoassignedbyme');
        $grid2->setEmbedded(true);
        $grid2->setBaseUrl(partial_url('todo.todo', 'refresh_todos', 'refresh_todos'));
        $todo_assignedbyme.= $grid2->render();
        $todo_assignedbyme.= '<br>' . href(dispatch_url("todo.todo", "add"), atktext('add'), SESSION_NESTED);

        $res = '<table border="0">';
        $res.= '<tr>';
        $res.= '<td valign="top">';
        $res.= '<form id="' . ($grid->getName() . '_form') . '" name="' . ($grid->getName() . '_form') . '" method="post" action="' . atkSelf() . '">';
        $res.= session_form();
        $res.= '<input type="hidden" name="atknodetype" value="' . $grid->getActionNode() . '">';
        $res.= $todo_assignedtome;
        $res.= '</form></td>';
        $res.= '</tr><tr><td>&nbsp;</td></tr><tr>';
        $res.= '<td valign="top">';
        $res.= '<form id="' . ($grid2->getName() . '_form') . '" name="' . ($grid2->getName() . '_form') . '" method="post" action="' . atkSelf() . '">';
        $res.= session_form();
        $res.= '<input type="hidden" name="atknodetype" value="' . $grid2->getActionNode() . '">';
        $res.= $todo_assignedbyme;
        $res.= '</form></td>';
        $res.= '</tr></table>';

        return $res;
    }

    public function action_refresh_todos()
    {
        atkimport('atk.datagrid.atkdatagrid');
        $this->getPage()->addContent(atkDataGrid::resume($this)->render());
    }

    /**
     * Add complete record action
     *
     * @param array $rec Record
     * @param array $actions Actions
     * @param array $mraactions Multi record actions
     */
    function recordActions($rec, &$actions, &$mraactions)
    {
        if ($rec['status'] <> '5' && $this->getType() != 'todo_history') {
            if ($this->allowed("complete"))
                $actions["complete"] = session_url(dispatch_url($this->atknodetype(), "complete", array("todo_id" => $rec["id"])));
            if ($this->allowed("mra_complete"))
                $mraactions[] = "mra_complete";
        }
    }

    /**
     * Update close date if complete is 100% or status is 5
     *
     * @param array $rec Record
     * @return boolean
     */
    function preUpdate(&$rec)
    {
        if (($rec["completed"] == 100 || $rec["status"] == 5) && $rec["closedate"]["year"] == "" && $rec["closedate"]["month"] == 0 && $rec["closedate"]["day"] == 0) {

            if ($rec["status"] == 5 && $rec["completed"] != 100) {
                $rec["completed"] = 100;
            } elseif ($rec["completed"] == 100 && $rec["status"] != 5) {
                $rec["status"] = 5;
            }

            $rec["closedate"] = array("year" => date("Y"),
                "month" => date("m"),
                "day" => date("d"));
        }
        return true;
    }

    /**
     * When action is complete, update completed, status and closedate
     *
     * @param object $handler Handler
     * @param array $record Record
     * @param string $mode Mode
     * @param string $selector ATK Selected
     */
    function action_complete(&$handler, $record = array(), $mode = "", $selector = NULL)
    {
        if ($selector != NULL) {
            $primkey = $selector;
        } else {
            $primkey = 'todo.id=\'' . $this->m_postvars["todo_id"] . '\'';
        }
        $rows = $this->selectDb($primkey, "", "", "", "", "edit");
        if (count($rows) == 1) {
            // It seems updatedb needs the complete record
            $rec = $rows[0];

            //$rec['atkprimkey'] = $primkey;
            $rec['completed'] = '100';
            $rec['status'] = '5';
            $rec['closedate'] = array('year' => date("Y"),
                'month' => date("m"),
                'day' => date("d"));
            $ret = $this->updateDb($rec);
        }

        $this->redirect();
    }

    /**
     * Complete Multi record action
     *
     */
    function action_mra_complete()
    {
        if (is_array($this->m_postvars['atkselector'])) {
            foreach ($this->m_postvars['atkselector'] as $selector) {
                $handler = "";
                $this->action_complete($handler, array(), "", $selector);
            }
        }
        $this->redirect();
    }

    /**
     * Initial values
     *
     * @return Array
     */
    function initial_values()
    {
        global $g_user;

        $entrydate = array("year" => date("Y"),
            "month" => date("m"),
            "day" => date("d"));
        $nextweek = getdate(time() + 60 * 60 * 24 * 7);
        $duedate = array("year" => $nextweek['year'],
            "month" => $nextweek['mon'],
            "day" => $nextweek['mday']);
        return array("owner" => array("id" => $g_user["id"]),
            "entrydate" => $entrydate,
            "duedate" => $duedate,
            "updated" => $entrydate,
            "completed" => 0,
            "status" => 1,
            "priority" => 3);
    }

    /**
     * Override title display
     *
     * @param array $record Record
     * @param string $mode Mode
     * @return string
     */
    function title_display($record, $mode)
    {
        if (($mode == "plain") || ($mode == "csv"))
            return $record["title"];

        $defaultaction = $this->allowed("edit") ? "edit" : "view";
        return href(dispatch_url("todo.todo", $defaultaction, array("atkselector" => "todo.id='" . $record["id"] . "'")), $record["title"], SESSION_NESTED);
    }

    /**
     * overide display of priority (set high to a red font)
     *
     * @param array $rec Record
     * @return string
     */
    function priority_display($rec)
    {
        $prio = $rec["priority"];
        $org = $this->m_attribList["priority"]->display($rec);
        $result = "";

        switch ($prio) {
            case 1:
                $result = '<font color="' . COLOR_ERROR . '"><b>' . $org . "</b></font>";
                break;
            case 2:
                $result = '<font color="' . COLOR_WARNING . '"><b>' . $org . "</b></font>";
                break;
            case 4:
                $result = '<font color="#666666">' . $org . "</font>";
                break;
            default:
                $result = $org;
                break;
        }
        return $result;
    }

    /**
     * Override completed display to draw a bar
     *
     * @param array $rec Record
     * @return string
     */
    function completed_display($rec)
    {
        $percentage = (isset($rec["completed"]) && is_numeric($rec["completed"])
                    ? $rec["completed"] : 0);
        if ($percentage >= 75) {
            $color = "green";
        } elseif ($percentage > 25 && $percentage < 75) {
            $color = "yellow";
        } else
            $color = "red";
        return '<div title="' . $percentage . '%"  style="height: 8px; width: 50px; border: 1px solid black; padding: 1px; text-align: left;">
              <div style="height: 8px; width: ' . $percentage . '%; background: ' . $color . ';"></div>
            </div>';
    }

    /**
     * Duedate display override
     *
     * @param record $rec Record
     * @return string
     */
    function duedate_display($rec)
    {
        $duedate = sprintf("%d-%02d-%02d", $rec["duedate"]["year"], $rec["duedate"]["month"], $rec["duedate"]["day"]);
        $open = (in_array($rec["status"], array(1, 3, 4)));
        $org = $this->m_attribList["duedate"]->display($rec);

        if (!$open) {
            return $org;
        } else {
            $days_left = $this->daysLeft(date("Y-m-d"), $duedate);

            if ($days_left > 2)
                return $org; // normal
            if ($days_left < 0)
                return '<font color="#FF0000"><b>' . $org . '</b></font>'; // expired
            return '<font color="#ee6c22"><b>' . $org . '</b></font>'; // almost expired
        }
    }

    /**
     * Set descriptor template
     *
     * @return string
     */
    function descriptor_def()
    {
        return "[title]";
    }

    /**
     * support function to add the priority header to a mail message
     * @param int $level priority level
     * @return string
     */
    function prioHeader($level)
    {
        $cr = chr(13) . chr(10);

        switch ($level) {
            case 1:
                $mslevel = "High";
                break;

            case 2:
                $mslevel = "High";
                break;

            case 3:
                $mslevel = "Normal";
                break;

            case 4:
                $mslevel = "Low";
                break;

            case 5:
                $mslevel = "Low";
                break;

            default:
                $level = 3;
                $mslevel = "Normal";
        }

        $header = "X-Priority: " . $level . $cr;
        $header .= "X-MSMail-Priority: " . $mslevel . $cr;
        return $header;
    }

    /**
     * Send an email when a todo is added
     *
     * @param record $rec Record
     * @return boolean
     */
    function postAdd($rec)
    {
        $db = &atkGetDb();
        // obtain some project info
        $assignTo = $rec["assigned_to"]["id"];
        $assigner = $rec["owner"]["id"];
        $pid = $rec["projectid"]["id"];

        // get project name
        $project = "";
        if ($pid != "") {
            $q = "SELECT name FROM project WHERE id='$pid'";
            $r = $db->getRows($q);
            $project = $r[0]["name"];
        }

        // now get her/his email address
        $q = "SELECT email, lastname, firstname, userid, id FROM person WHERE id IN ('$assignTo','" . $assigner . "')";
        $emails = $db->getrows($q);

        // now assemble the mail body
        $subj = $this->text("todoadded") . ": " . $rec["title"];

        $body = $this->text("todo") . ": " . $rec["title"] . "\n";
        $body.= $this->text("description") . ": \n" . $rec["description"] . "\n\n";
        $body.= $this->text("priority") . ": " . $this->prioName[$rec["priority"] - 1] . "\n";
        $body.= $this->text("status") . ": " . $this->m_attribList["status"]->display($rec) . "\n";
        if ($project != "")
            $body.= $this->text("project") . ": " . $project . "\n";
        $body.= $this->text("duedate") . ": " . $this->m_attribList["duedate"]->display($rec) . "\n";

        $body.= "\n";

        for ($i = 0; $i < count($emails); $i++) {
            if ($emails[$i]["id"] == $assigner)
                $body.= $this->text("todo_ownedby") . ": " . $emails[$i]["lastname"] . ", " . $emails[$i]["firstname"] . "\n";
            if ($emails[$i]["id"] == $assignTo)
                $body.= $this->text("todo_assignedto") . ": " . $emails[$i]["lastname"] . ", " . $emails[$i]["firstname"] . "\n";
            $to[] = $emails[$i]["email"];
        }

        // send mail
        $header = $this->prioHeader($rec["priority"]);
        usermail($to, $subj, $body, $header);
        return true;
    }

    /**
     * Check if a todo is changed
     *
     * @param unknown_type $new New
     * @param unknown_type $old Old
     * @return boolean
     */
    function todoChanged($new, $old)
    {
        foreach (array_keys($this->m_attribList) as $attribname) {
            if ($attribname != "updated") { // leave out the updated attrib, for it always
                // changes, but that doesn't mean a change in
                // the record.
                $p_attrib = &$this->m_attribList[$attribname];
                if (!$p_attrib->equal($new, $old)) {
                    atkdebug($attribname . " changed");
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Send mail to todo owner when todo is changed
     *
     * @param array $rec Record
     * @return boolean
     */
    function postUpdate($rec)
    {
        global $g_user;

        if ($this->todoChanged($rec, $rec["atkorgrec"])) {
            $owner = $rec["owner"]["id"];
            $newassign = $rec["assigned_to"]["id"];
            $oldassign = $rec["atkorgrec"]["assigned_to"]["id"];

            // read all relevant emails.
            $query = "SELECT DISTINCT lastname, firstname, email, userid, id
               FROM person
               WHERE id IN('$owner','" . $g_user["id"] . "','$newassign','$oldassign')";
            $db = &atkGetDb();
            $emails = $db->getrows($query);
            $pid = $rec["projectid"]["id"];
            // get project name
            $project = "";

            if ($pid != "") {
                $query = "SELECT name FROM project WHERE id='$pid'";
                $res = $db->getrows($query);
                $project = $res[0]["name"];
            }

            $body = $this->text("todo") . ": " . $rec["title"] . "\n";
            $body.= $this->text("description") . ": \n" . $rec["description"] . "\n\n";
            $body.= $this->text("priority") . ": " . $this->prioName[$rec["priority"] - 1] . "\n";
            $body.= $this->text("status") . ": " . $this->m_attribList["status"]->display($rec) . "\n";
            if ($project != "")
                $body.= $this->text("project") . ": " . $project . "\n";
            $body.= $this->text("duedate") . ": " . $this->m_attribList["duedate"]->display($rec) . "\n";

            $body.= "\n";
            for ($i = 0; $i < count($emails); $i++) {
                if ($emails[$i]["id"] == $owner)
                    $body.= $this->text("todo_ownedby") . ": " . $emails[$i]["lastname"] . ", " . $emails[$i]["firstname"] . "\n";
                if ($emails[$i]["id"] == $oldassign)
                    $body.= $this->text("todo_previouslyassignedto") . ": " . $emails[$i]["lastname"] . ", " . $emails[$i]["firstname"] . "\n";
                if ($emails[$i]["id"] == $newassign)
                    $body.= $this->text("todo_assignedto") . ": " . $emails[$i]["lastname"] . ", " . $emails[$i]["firstname"] . "\n";
                if ($emails[$i]["id"] == $g_user["id"])
                    $body.= $this->text("todo_changedby") . ": " . $emails[$i]["lastname"] . ", " . $emails[$i]["firstname"] . "\n";
                $to[] = $emails[$i]["email"];
            }

            // send mail
            $header = $this->prioHeader($rec["priority"]);
            usermail($to, $this->text("todochanged"), $body, $header);

            // also store the old todo in the history table.
            $historynode = &atkGetNode("todo.todo_history");
            $rec["atkorgrec"]["todoid"]["id"] = $rec["id"]; // the todoid in the historytable
            // points to the current todoid.
            $historynode->addDb($rec["atkorgrec"]);
        }
        return true;
    }

    /** Workaround for bug #194.
     * By setting this method, the owner field is always filled with something, so ownerless
     * todo's will not complain that they can't be saved.
     * Note that this will not actually store the new owner, for that would be even more
     * dangerous.
     * Since this only happens to very few todo's that were converted when a certain
     * conversion bug was still present, this workaround suffices.
     *
     * @param array $record @record
     * @return array
     */
    function edit_values($record)
    {
        if ($record["owner"]["id"] == "") {
            $record["owner"] = getUser();
        }
        return $record;
    }

    /**
     * Calculate days left
     *
     * @param string $start Start date (yyyy-mm-dd)
     * @param string $end End date (yyyy-mm-dd)
     * @return int
     */
    function daysLeft($start, $end)
    {
        if ($start == "" || $end == "")
            return 0;

        $d1 = adodb_mktime(12, 0, 0, substr($start, 5, 2), substr($start, 8, 2), substr($start, 0, 4));
        $d2 = adodb_mktime(12, 0, 0, substr($end, 5, 2), substr($end, 8, 2), substr($end, 0, 4));
        return floor(($d2 - $d1) / (60 * 60 * 24));
    }

}

?>