<?php
  /**
   * mastergantt node class file
   *
   * @package achievo
   * @subpackage project
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

  atkimport("module.utils.dateutil");

  include_once("achievotools.inc");

  /**
   * The node class for mastergantt
   *
   * @author ivo <ivo@ibuildings.nl>
   * @package achievo
   * @subpackage reports
   */
  class mastergantt extends atkNode
  {
    /**
     * mastergantt constructor
     */
    function mastergantt($name="mastergantt")
    {
      $this->atkNode($name);
      $this->setSecurityAlias("project.project");
      $this->m_securityMap["planninggraph"] = "planning";
    }

    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        $s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");
        $s_record["plannedbooked"] = $g_sessionManager->pageVar("plannedbooked");

        // convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs();
        foreach (array_keys($attrs) as $attribname)
        {
          $p_attrib = &$attrs[$attribname];
          $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
        }


        $go = $g_sessionManager->pageVar("go");
        if ($go!=1)
        {
          // initial loading. We'll put the from/to fields to reasonable defaults.

          $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

          // next month
          $next_month = mktime(12,0,0,date("m")+1, date("d"), date("Y"));
          $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
        }
      }
      return $s_record;
    }

    function &getFilterAttrs()
    {
      useattrib("employee.myemployeesattribute");

      $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "project.project.any_project");

      useattrib("atklistattribute");
      $attrs["plannedbooked"] = &new atkListAttribute("plannedbooked", array("planned", "booked"), "", AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM);

      $attrs["from"] = &new atkDateAttribute("from");
      $attrs["to"] = &new atkDateAttribute("to");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="'.getDispatchFile().'" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      if ($this->hasAnyProjectPrivilege())
      {
        $output.= atktext("coordinator").": ".$attrs["coordinator"]->edit($record)."<br/>";
      }

      $output.= atktext("display").": ".$attrs["plannedbooked"]->edit($record)."<br/>";

      $output.= atktext("showonlyphasesfrom", "reports")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to")." ".$attrs["to"]->edit($record)."<br/>";

      $output.= '<input type="submit" value="'.atktext("refresh").'">';
      $output.= '</form>';

      return $output;
    }

    function hasAnyProjectPrivilege()
    {
      return $this->allowed("any_project");
    }

    function action_planning()
    {
      $filter = $this->getFilterRecord();

      $content = $this->getFilterBar();

      if (atkArrayNvl($this->m_postvars,"go",0)==1)
      {
        if (!$this->hasAnyProjectPrivilege())
        {
          $user = getUser();
          $coordinator = $user["id"];
        }
        else
        {
          $coordinator = $filter["coordinator"]["id"];
        }
        $params = array(
          "coordinator" => $coordinator,
          "from" => dateUtil::arr2str($filter["from"]),
          "to" => dateUtil::arr2str($filter["to"]),
          "plannedbooked" => $filter["plannedbooked"]
        );
        $imgurl = session_url(dispatch_url("{$this->m_module}.{$this->m_type}", "planninggraph", $params));

        $content.= '<br/><br/><img src="'.$imgurl.'" border="0"/><br/><br/>';
      }

      $ui = &$this->getUi();
      $box = $ui->renderBox(array("title"=>"Master Gantt", "content"=>$content));
      $actionpage = $this->renderActionPage("stats", array($box));

      $this->addStyle("style.css");
      $page = &$this->getPage();
      $page->addContent($actionpage);
    }

    function dashDate($undasheddate)
    {
      return substr($undasheddate, 0, 4)."-".
             substr($undasheddate, 4, 2)."-".
             substr($undasheddate, 6, 2);
    }

    function getGant($from, $to, $coordinator)
    {
      $db = &atkGetDb();
      $projects = $db->getrows("SELECT
                                  project.id,
                                  project.name,
                                  MIN(phase.startdate) as startdate,
                                  MAX(phase.enddate) as enddate
                                FROM
                                  project,
                                  phase
                                WHERE
                                  phase.status='active'
                                  AND project.status='active'
                                  AND phase.projectid = project.id
                                  AND (phase.enddate IS NULL OR phase.enddate>='$from')
                                  AND (phase.startdate IS NULL OR phase.startdate <= '$to')
                                  ".($coordinator!=""?"AND project.coordinator=$coordinator":"")."
                                GROUP BY
                                  project.id,
                                  project.name");

      //make an gant array, this array contains important information about the phases
      $gant = array();
      for($i=0;$i<count($projects);$i++)
      {
        $gant[($projects[$i]['id'])] = $projects[$i];

        $gant[($projects[$i]['id'])]['booked'] = $this->getBooked($projects[$i]['id']);
        $gant[($projects[$i]['id'])]['planned'] = $this->getPlanned($projects[$i]['id']);
      }
      return $gant;
    }

    function getPlanned($projectid)
    {
      $db = &atkGetDb();
      $rows = $db->getrows("SELECT sum(current_planning) as planned FROM phase
                              WHERE phase.projectid=".$projectid);
      return $rows[0]["planned"];
    }

    function getBooked($projectid)
    {
      $db = &atkGetDb();
      $rows = $db->getrows("SELECT sum(time) as booked FROM hours, phase
                              WHERE hours.phaseid = phase.id AND phase.projectid=".$projectid);
      return $rows[0]["booked"];
    }

    //order the gant array with on the startdate of that phase
    function cmp($a, $b) {
        if ($a['startdate'] == $b['startdate']) return 0;
        return ($a['startdate'] < $b['startdate']) ? -1 : 1;
    }

    function getParams()
    {
      // Load used files
      include_once(atkconfig("atkroot")."atk/utils/adodb-time.inc.php");
      atkimport("module.utils.dateutil");

      // Get a reference to the securitymanager
      global $g_securityManager;

      // Initialize the params array
      $params = array();

      // Load the params
      if ($g_securityManager->allowed("project.project", "any_project"))
      {
        $params["coordinator"] = $_REQUEST["coordinator"];
      }
      else
      {
        $user = getUser();
        $params["coordinator"] = $user["id"];
      }
      $params["from"] = $this->dashDate(dateUtil::startOfWeek($_REQUEST["from"]));
      $params["to"] = $this->dashDate(dateUtil::endOfWeek($_REQUEST["to"]));
      $params["plannedbooked"] = $_REQUEST["plannedbooked"];
      // safeguard
      if (!in_array($params["plannedbooked"], array("planned", "booked")))
      {
        $params["plannedbooked"] = "planned";
      }

      // Return the params
      return $params;
    }

    function drawProject(&$graph, $index, $project, $from, $to, $plannedbooked)
    {
      // Projects that extent the selected period should be cut off.
      if ($project['startdate']<$from) $project['startdate']=$from;
      if ($project['enddate']>$to || $project['enddate']=='') $project['enddate']=$to;

      $caption = "[".time_format($project[$plannedbooked], true)."]";
      $activity = new GanttBar($index, $project['name'], $project['startdate'], $project['enddate'], $caption);

      $colorbase = $project[$plannedbooked];

      $colornode = &atkGetNode("project.mastergantt_colorconfig");
      $color = $colornode->getColor($colorbase/60);
      $activity->SetPattern(BAND_SOLID, $color);
      $activity->SetHeight(10);

      $activity->SetFillColor($color);
      $graph->add($activity);
    }

    function showGraph($gantt, $from, $to, $plannedbooked)
    {
      // Load jpgraph gantt chart library files
      include_once(moduleDir("graph")."jpgraph/jpgraph.php");
      include_once(moduleDir("graph")."jpgraph/jpgraph_gantt.php");

      $graph = new GanttGraph(0,0,"auto");
      $graph->SetBox();
      $graph->SetShadow();

      // Show day, week and month scale
      $graph->ShowHeaders(GANTT_HDAY | GANTT_HWEEK | GANTT_HMONTH);

      // Use the short name of the month together with a 2 digit year
      // on the month scale
      $graph->scale->month->SetStyle(MONTHSTYLE_SHORTNAMEYEAR2);
      $graph->scale->month->SetFontColor("white");
      $graph->scale->month->SetBackgroundColor("blue");

      // 0 % vertical label margin
      $graph->SetLabelVMarginFactor(1);


      $projectids = array_keys($gantt);
      for($i=0,$_i=count($gantt); $i<$_i; $i++)
      {
        $this->drawProject($graph, $i, $gantt[$projectids[$i]], $from, $to, $plannedbooked);
      }

      // Set date range to prevent autoscale errors when no activities were added to the gantt chart
      $graph->SetDateRange($from, $to);

      $graph->Stroke();
      flush();
      die();
    }

    function action_planninggraph(&$handler)
    {
      $params = $this->getParams();

      $gant = $this->getGant($params["from"], $params["to"], $params["coordinator"]);

      usort($gant, array("mastergantt", "cmp"));

      $this->showGraph($gant, $params["from"], $params["to"], $params["plannedbooked"]);
    }


  }
?>