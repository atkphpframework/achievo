<?php
/**
 * projectstaus  node class file
 *
 * @package achievo
 * @subpackage reports
 *
 * @author ivo <ivo@ibuildings.nl>
 *
 * @copyright (c) 2005 Ibuildings.nl BV
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
 *
 * @version $Revision$
 * $Id$
 */
/**
 * @internal includes
 */
useattrib("atkattribute");
useattrib("atkdateattribute");
useattrib("atklistattribute");

atkimport("module.utils.dateutil");

include_once("achievotools.inc");

/**
 * The node class for project status
 *
 * @author ivo <ivo@ibuildings.nl>
 * @package achievo
 * @subpackage reports
 */
class projectstatus extends atkNode
{

    /**
     * projectstatus constructor
     */
    function projectstatus()
    {
        $this->atkNode("projectstatus");
    }

    function getFilterRecord()
    {
        static $s_record = NULL;
        global $g_sessionManager;

        if ($s_record == NULL) {
            $s_record = array();
            $s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
            $s_record["from"] = $g_sessionManager->pageVar("from");
            $s_record["to"] = $g_sessionManager->pageVar("to");
            $s_record["diff"] = $g_sessionManager->pageVar("diff");

            // convert attribute html values to internal values
            $attrs = &$this->getFilterAttrs();
            foreach (array_keys($attrs) as $attribname) {
                $p_attrib = &$attrs[$attribname];
                $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
            }


            $go = $g_sessionManager->pageVar("go");
            if ($go != 1) {
                // initial loading. We'll put the from/to fields to reasonable defaults.

                $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

                // next month
                $next_month = mktime(12, 0, 0, date("m") + 1, date("d"), date("Y"));
                $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
            }
        }
        return $s_record;
    }

    function &getFilterAttrs()
    {
        useattrib("employee.myemployeesattribute");

        $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "reports.projectstatus.any_user");

        $attrs["from"] = &new atkDateAttribute("from");
        $attrs["to"] = &new atkDateAttribute("to");

        $attrs["diff"] = &new atkListAttribute("diff", array(atktext('all'), atktext('positive'), atktext('negative')), array('all', 'pos', 'neg'), AF_OBLIGATORY);

        return $attrs;
    }

    function getFilterBar()
    {
        $record = $this->getFilterRecord();

        $attrs = &$this->getFilterAttrs();

        $output = '<form action="' . getDispatchFile() . '" method="get">';
        $output.= session_form();
        $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

        if ($this->hasAnyUserPrivilege()) {
            $output.= atktext("coordinator") . ": " . $attrs["coordinator"]->edit($record) . " ";
        }

        $output.= "<br/>";

        $output.= atktext("showonlyphasesfrom", "reports") . " " . $attrs["from"]->edit($record) . " ";
        $output.= atktext("to") . " " . $attrs["to"]->edit($record);

        $output.= "<br/>";

        $output.= atktext('diff_planning') . " " . $attrs['diff']->edit($record) . " ";

        $output.= ' <input type="submit" value="' . atktext("refresh") . '">';
        $output.= '</form>';

        return $output;
    }

    function action_report()
    {
        // If adminPage implemented, call it and store the content result.
        $content = $this->getProjectStatus();

        // Add the given content to the page
        $page = &$this->getPage();
        $page->addContent($this->renderActionPage("admin", $content));
    }

    function diffToHtml($diff)
    {
        $diffStr = time_format($diff, true);

        if ($diff < 0) {
            $diffStr = '<span style="color: ' . COLOR_ERROR . '">' . $diffStr . '</span>';
        }
        return $diffStr;
    }

    function getProjectStatus()
    {
        // Get a reference to the sessionmanager
        global $g_sessionManager;

        $this->addStyle("style.css");

        // Try to get a reference to the User Interface object and raise an error if it fails
        $ui = &$this->getUi();

        $go = $g_sessionManager->pageVar("go");

        // Start composing the output by adding the filterbar as topmost item
        $output = $this->getFilterBar();

        $filterrecord = $this->getFilterRecord();

        // Add a newline (vertical spacing)
        $output.= '<br/>';

        // Only get the planning if the filter was set and submitted
        if ($go == 1) {
            $db = &$this->getDb();
            $strfrom = dateutil::arr2str($filterrecord["from"]);
            $strto = dateutil::arr2str($filterrecord["to"]);

            $empfilter = "";
            if ($filterrecord["coordinator"] != NULL) {
                $empfilter = " AND p.coordinator=" . $filterrecord["coordinator"]["id"];
            } else if (!$this->hasAnyUserPrivilege()) { // only show your own employees
                $user = getUser();
                $empfilter = " AND p.coordinator=" . $user["id"];
            }

            $rows = $db->getrows("SELECT
                                p.name as project,
                                ph.name as phase,
                                ph.initial_planning,
                                ph.current_planning,
                                sum(h.time) as minutes
                              FROM
                                project p,
                                phase ph
                              LEFT JOIN hours h ON (h.phaseid = ph.id)
                              WHERE
                                p.id = ph.projectid
                                AND h.phaseid = ph.id
                                AND 
                                (
                                  (ph.startdate < '$strto' AND ph.enddate > '$strfrom')
                                  OR
                                  (ph.startdate < '$strto' AND ph.enddate IS NULL)
                                  OR
                                  (ph.startdate IS NULL AND ph.enddate > '$strfrom')
                                  OR
                                  (ph.startdate IS NULL AND ph.enddate IS NULL)
                                )
                                
                                AND ph.status = 'active'
                                AND p.status = 'active'
                                $empfilter
                              GROUP BY
                                p.name,
                                ph.name,
                                ph.initial_planning,
                                ph.current_planning
                              ORDER BY
                                p.name, ph.name");

            $data[] = array(atktext("project"), atktext("phase"), atktext("initial_planning"), atktext("current_planning"), atktext("booked"), atktext("diff_planning"));

            $total_time = 0;
            $total_current = 0;
            $total_initial = 0;
            $total_diff = 0;

            $previousname = "";

            for ($i = 0, $_i = count($rows); $i < $_i; $i++) {
                $line = array();

                $name = $rows[$i]["project"];
                if ($name != $previousname) {
                    $line[] = $name;

                    $previousname = $name;
                } else {
                    $line[] = "";
                }

                $line[] = $rows[$i]["phase"];
                $line[] = time_format($rows[$i]["initial_planning"]);
                $line[] = time_format($rows[$i]["current_planning"]);

                $booked = time_format($rows[$i]["minutes"]);

                $planning = empty($rows[$i]["current_planning"]) ? $rows[$i]["initial_planning"]
                        : $rows[$i]["current_planning"];
                if ($rows[$i]["minutes"] > $planning) {
                    $booked = '<span style="color: ' . COLOR_ERROR . '">' . $booked . '</span>';
                } else if ($rows[$i]["minutes"] > ($planning * 0.9)) {
                    $booked = '<span style="color: ' . COLOR_WARNING . '">' . $booked . '</span>';
                }

                $line[] = "<b>" . $booked . "</b>";

                $diff = $planning - $rows[$i]["minutes"];

                if (($filterrecord['diff'] == 'neg' && $diff >= 0) || ($filterrecord['diff'] == 'pos' && $diff < 0))
                    continue;

                $line[] = $this->diffToHtml($diff);

                $total_time += $rows[$i]["minutes"];
                $total_current += $rows[$i]["current_planning"];
                $total_initial += $rows[$i]["initial_planning"];
                $total_diff += $diff;

                $data[] = $line;
            }

            $data[] = array(atktext("total"), "", time_format($total_initial), time_format($total_current), time_format($total_time), $this->diffToHtml($total_diff));

            $tr = &atknew("atk.utils.atktablerenderer");
            $tr->setColAlignment(2, TBL_RIGHT);
            $tr->setColAlignment(3, TBL_RIGHT);
            $tr->setColAlignment(4, TBL_RIGHT);
            $tr->setColAlignment(5, TBL_RIGHT);
            $output.= "<br>" . $tr->render($data, TBL_DATA | TBL_FOOTER, "recordlist") . "<br/>";
        }

        // Render a box around the generated output
        $boxedoutput = $ui->renderBox(array("title" => $this->actionTitle('report'), "content" => $output));

        // Return the generated boxed output
        return $boxedoutput;
    }

    function hasAnyUserPrivilege()
    {
        return $this->allowed("any_user");
    }

}

?>