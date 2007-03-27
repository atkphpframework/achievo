<?php
	/**
	 * One line description of the file
	 *
	 * Full description of the file
	 *
	 * @package achievo
	 * @subpackage modules
	 *
	 * @author dennis <dennis@ibuildings.nl>
	 *
	 * @copyright (c) 2005 Ibuildings.nl BV
	 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
	 *
	 * @version $Revision$
	 * $Id$
	 */

  /**
   * Import the used classes, nodes, attributes
   */
  include_once("class.hours.inc");
  useattrib("atkNumberAttribute");
  atkimport("atk.ui.atkui");

	/**
	 * One line description of the class
	 *
	 * Full description of the class
	 *
	 * @author dennis <dennis@ibuildings.nl>
	 * @package achievo
	 * @subpackage modules
	 */
  class hours_approve extends hours
	{

    var $m_lockmode;

    /**
     * Constructor
     */
	  function hours_approve()
	  {
	    // Call the base class constructor
	    $this->hours("hours_approve");

	    // Get a reference to the global sessionmanager
	  	global $g_sessionManager;

	  	// Make sure the year session variable is available at all times
  	  $g_sessionManager->pageVar("year");
  	  $g_sessionManager->pageVar("viewdate");
  	  $g_sessionManager->pageVar("periodid");
  	  $g_sessionManager->pageVar("viewuser");

  	  $this->m_lockmode = atkConfig::get("timereg", "lockmode", "week");

	  }

    /**
     * Creates a custom titlebar for the weekview
     *
     * Full description of the function
     *
     * @param type name description
     * @return type description
     */
	  function weekviewTitle($userid, $viewtime)
    {
      $employeenode = &atkGetNode("employee.employee");
      $employees = $employeenode->selectDb("person.id=" . $userid);
      $employeedesc = (count($employees) > 0) ? $employees[0]["firstname"]." ".$employees[0]["lastname"] : "";

      return sprintf(atktext("title_hoursapprove_weekview"), $employeedesc, strftime("%V, %Y",$viewtime));
    }

    /**
     * weekviewHeader override function
     *
     * This function hides the header displayed by the hours base class
     */
    function weekviewHeader()
    {
    }

	  /**
	   * One line description of the function
	   *
	   * @param type name description
	   * @return type description
	   */
	  function weekviewFooter()
	  {
	    // Read the hours_lock id
	    $periodid = $this->m_postvars["periodid"];

	    // If the periodid isnt numeric, then throw an error, no action can be
      // taken so no buttons are added to the footer.
	    if (!is_numeric($periodid))
	    {
	      atkerror("Posted periodid isn't numeric");
	      return "";
	    }

	    // Retrieve the hours_lock record for the specified periodid
	    $hourslocknode = &atkGetNode("timereg.hours_lock");
      $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$periodid."'
                                   AND (hours_lock.userid='".$this->m_user."'
                                        OR hours_lock.userid IS NULL)");

     // Don't display any buttons if there are no hourlocks found
      if (count($hourlocks)==0)
      {
        return "";
      }

      // Determine the approval status
      $hourlockapproved = ($hourlocks[0]["approved"]==1);

      $output = '<br />' . sprintf(atktext("hours_approve_thestatusofthisperiodis"), $this->m_lockmode) . ": ";
      if ($hourlockapproved)
      {
        $output.= '<font color="#009900">' . atktext("approved") . '</font><br /><br />';
      }
      else
      {
        $output.= '<font color="#ff0000">' . atktext("not_approved_yet") . '</font><br /><br />';
      }

      // Add an approve or disapprove button to the weekview
	    if ($hourlockapproved)
  	    $output.= atkButton(atktext("disapprove"),
                            dispatch_url("timereg.hours_approve",
                                         "disapprove",
                                         array("periodid"=>$periodid)),
                            SESSION_NESTED);
	    else
	    {
	      $output.= atkButton(atktext("approve"),
                            dispatch_url("timereg.hours_approve","approve"));

  	    $output.= "&nbsp;&nbsp;".
                  atkButton(atktext("disapprove"),
                            dispatch_url("timereg.hours_approve",
                                         "disapprove",
                                         array("periodid"=>$periodid)),
                            SESSION_NESTED);
	    }

	    // Return the weekview including button
	    return $output;
	  }

    /**
	   * Detects old datastructures and shows a link to the conversionscript if appropriate
	   *
	   * @param type name description
	   * @return type description
	   * @todo Rewrite this function to an installer/patch
	   */
	  function convertheck()
	  {
	    return ""; // method causes fatal error. Could not find a way to make this working.

	    // Get a reference to the databeest
	  	$db = &atkGetDb();

	    // Detect the old datastructure
	  	$showtable = $db->getrows("SHOW TABLES LIKE 'hours_approved'");

      // If old datastructures found, then show link to conversion action
	  	if (!empty($showtable[0]))
	    {
	      return href(dispatch_url("timereg.hours_approve","convert_old"), "Converteer oude gegevens"); // @todo translate
	    }
	    else
	    {
	      return "";
	    }
	  }

	  /**
	   * Handler for the admin action
	   *
	   * @param atkAdminHandler &$handler Default atk action handler for the admin action
	   * @return string atkAdminHandler->action_admin() result
	   */
	  function action_admin(&$handler)
	  {
	    // Get the atkpage singleton instance
	    $page = &atkpage::getInstance();

	    // Add the stylesheet to the page
	    $this->addStyle("style.css");

	    $boxes = array();

	    // If a period is selected for a specific employee, then show the approval screen
	    if ((!is_null($this->m_postvars["viewuser"])) && (!is_null($this->m_postvars["viewdate"])) && (($this->m_postvars["year"] == null) || ($this->m_postvars["year"] == substr($this->m_postvars["viewdate"], 0, 4))))
	    {
	    	// Try to find the hour_lock in de database
	    	$hourslocknode = &atkGetNode("timereg.hours_lock");
	    	$hourlocks = $hourslocknode->selectDb("hours_lock.id='" . $this->m_postvars["periodid"] . "'");

	    	// Only show the weekview if the lock still exists:
	    	if (count($hourlocks) > 0)
	    	{

	    	  // Store the employeeid and viewdate in class vars
    	  	$this->m_user = $this->m_postvars["viewuser"];
  	    	$this->m_viewdate = $this->m_postvars["viewdate"];

  	    	// Add the approve screen to the page
  	    	$boxes[] = $this->weekview(atkConfig::get('timereg','hours_approve_projectteam_members_only'));
	    	}

	    }

	    // Add the approvedperiodview to the page
	    $boxes[] = $this->approvedperiodview();
      $actionpage = $this->renderActionPage("stats", $boxes);
      $page->addContent($actionpage);

	    // Return an empty string (output is handled through the page
	    return "";
	  }

	  /**
	   * One line description of the function
	   *
	   * Full description of the function
	   *
	   * @param type name description
	   * @return type description
	   */
	  function approvedperiodview()
	  {
	    // Get the current user
	    $user = &getUser();

	    // If the year is given in postvars, then view the approvedperiodview
      // for the specified year
	    if (is_numeric($this->m_postvars["year"]) && strlen($this->m_postvars["year"])==4)
	      $year = trim($this->m_postvars["year"]);

	    // Else use the current year
	    else
	      $year = date('Y');

      $selected_functionlevel_id = $this->m_postvars["functionlevelswitch"];

      if (moduleExists('advancedsecurity'))
        $lowerlevels = atkArrayNvl($this->m_postvars, "lowerlevels", 'off');
      else
        $lowerlevels = "off";

	    // Get the singleton instance of the ui renderer
	    $ui = &atkui::getInstance();

      // Compose the tplvars containing content and legend
      $tplvars_contentwithlegend = array(
        "content" => $this->getEmployeesTable($user["id"], $year, $selected_functionlevel_id, $lowerlevels),
        "legenditems" => $this->getLegendData()
      );

      $contentwithlegend = $ui->render("contentwithlegend.tpl", $tplvars_contentwithlegend);

	    // Use a numberattrib to get a display for the year inputbox
	    $yearattrib = new atkNumberAttribute("year",0,6);

	    $func_code = $this->get_functionlevels($selected_functionlevel_id, $lowerlevels);

      // Display the year input form
      $header = "";
      if ($func_code != null) $header.= atktext('functionlevel','project').': '.$func_code.'<br />';
      $header.= atktext("year")." : ".$yearattrib->edit(array("year"=>$year)).'&nbsp;&nbsp;';
      $header.= '<input type="submit" name="atkbtn1" value="'.atktext("view").'">';
      //$header.= atkbutton(atktext("view"), dispatch_url("timereg.hours_approve", "admin"), SESSION_REPLACE);

      // Add an explanation to the approvedperiodview
	    $footer = atktext("remark") . ": <br />";
      $footer.= (!$this->allowed("any_user")) ? atktext("hours_approve_remarkmanager") . '<br />' : '';
      $footer.= (moduleExists('advancedsecurity')) ? atktext("hours_approve_advancedsecurity") . '<br />' : '';
      $footer.= sprintf(atktext("hours_approve_remarkperiodblock"), ucfirst($this->m_lockmode));

      if (atkconfig::get('timereg','hours_approve_only_show_not_approved') == true)
        $footer .= "<br />" . atktext('hours_approve_only_show_not_approved');

       if (atkconfig::get('timereg','hours_approve_projectteam_members_only') == true)
        $footer .= "<br />" . atktext('hours_approve_explain_teammembers_only');

	    $footer.= $this->convertheck();

	    // Start the output using a session form
	    $formstart = '<form name="entryform" enctype="multipart/form-data" action="dispatch.php" method="post" onsubmit="globalSubmit(this)">';
	    $formstart.= session_form();
	    // Close the form
	    $formend = '</form>';

	    // Compose the tplvars containing the
	    $tplvars_vertical = array(
	      "formstart" => $formstart,
	      "formend" => $formend,
	      "blocks" => array(
                      $header,
                      $contentwithlegend,
                      $footer
                    ),
        "align" => "left"
      );

      $boxcontents = $ui->render("vertical.tpl", $tplvars_vertical);

	    // Add a conversion link to the approvedperiodview if datastructurs outdated

	    // Put the result into a box
	    $boxedresult = $ui->renderBox(array("title"=>atktext("title_houradmin_approve"),"content"=>$boxcontents));

	    // Return the boxed result HTML
	    return $boxedresult;
	  }

    function get_functionlevels($selected_level_id, $lowerlevels)
    {
      atkdebug("get_functionlevels | selected=$selected_level_id | lowerlevels=$lowerlevels");
      $params = array("selected_level_id" => $selected_level_id,
                      "lower_levels"      => $lowerlevels);
      $harvest = atkHarvestModules("getFuncLevelDropDown", $params, true);

      $func_code = null;
      if (is_array($harvest) && count($harvest) > 0)
      {
        if (moduleExists('advancedsecurity'))
          $func_code = $harvest['advancedsecurity'];
      }

      return $func_code;
    }

	  /**
	   * Returns the legend HTML code
	   *
	   * @return String HTML code used to display the legend
	   */
	  function getLegendData()
	  {
      // Return the legenditems array
      return array(
        "#22BB22" => atktext("approved"),
        "#FF2222" => atktext("not_approved_yet"),
      );
	  }

	  function getdatefromperiod($yearperiod)
	  {
	    $weekEndDay = 0;
	      // 0 = Sunday, 1 = Monday etc.
	    $year = (int) substr($yearperiod, 0, 4);
	    $period = (int) substr($yearperiod, -2, 2);

      if ($this->m_lockmode == "month")
      {
        // Last day of month
        $date = mktime(0, 0, 0, $period+1, 0, $year);
      }
      else
      {
        // Weekly locking
        $dayOfYear = 4 + (($period - 1) * 7);
        $date = mktime(0, 0, 0, 1, $dayOfYear, $year);
        $dayOfWeek = date("w", $date);
        $daysToAdd = ($weekEndDay - $dayOfWeek + 7) % 7;
        $date += $daysToAdd * 24*60*60;
      }

      return $date;
	  }

    /**
     * Get employee-data
     *
     * @param int $current_user_id
     * @param int $year
     * @param string lowerlevels checkbox can be 'on' or 'off'
     * @return array $users
     */
	  function getEmployeesData($current_user_id, $year, $selected_functionlevel_id, $lowerlevels)
	  {
	    // Load the employee node
	    $employeenode = &atkGetNode("employee.employee");

      $employee_where_clause = $this->getEmployeesWhereClause($current_user_id, $selected_functionlevel_id, $lowerlevels);

	    // Get the employees from the database
	    $data = $employeenode->selectDb($employee_where_clause, null, null, null, array('id', 'lastname', 'firstname', 'supervisor'));

	    $users = array();
	    foreach ($data as $key => $user)
	    {
	      $users[$key]["id"]   = $user["id"];
	      $users[$key]["name"] = $user["firstname"].' '.$user["lastname"];
	      $users[$key]["period"]=array();

	      $hourslocknode = &atkGetNode("timereg.hours_lock");

	      $extra_selector = "";
	      if (moduleExists('timereg') && atkconfig::get('timereg','approve_period_per_coordinator') == true)
	      {
	        $loggedinuser = &getUser();
	        $current_user_id = $loggedinuser['id'];
	        if ($current_user_id != -1 && !is_null($current_user_id))
	         $extra_selector = " AND hours_lock.coordinator='$current_user_id' ";
	      }

        // We sort and limit the query so we get approved entries for this user if there are any.
        // Approved entries will override non-approved ones.
        $lockedperiods = $hourslocknode->selectDb("(hours_lock.userid='".$user["id"]."' OR hours_lock.userid IS NULL)" . $extra_selector, "approved ASC");

	      foreach ($lockedperiods as $lockedperiod)
	      {
	        if (atkconfig::get('timereg','hours_approve_only_show_not_approved') == true && $lockedperiod["approved"] == 1)
	          continue; // only display not-approved periods

	        if (substr($lockedperiod["period"],0,4)==$year)
	        {
	          $users[$key]["period"][substr($lockedperiod["period"],4)]["id"]= $lockedperiod['id'];
	          $users[$key]["period"][substr($lockedperiod["period"],4)]["approved"] = $lockedperiod["approved"];
	        }
	      }
	      ksort($users[$key]["period"]);
	    }

	    return $users;
	  }

	  /**
     * If functionlevel-dropdown is not present, or 'all' functionlevels selected, the default selection will be returned
	   *
	   * @param int $current_user_id
	   * @param int $selected_functionlevel_id
	   * @param string lowerlevels checkbox can be 'on' or 'off'
	   * @return string
	   */
	  function getEmployeesWhereClause($current_user_id, $selected_functionlevel_id, $lowerlevels)
	  {
	    if (empty($selected_functionlevel_id) || $selected_functionlevel_id == 'all' || !moduleExists('advancedsecurity'))
	    {
	      $where_clause = $this->getDefaultEmployeeWhereClaue($current_user_id);
	    }
	    else
	    {
	      $where_clause = $this->getFunctionLevelEmployeeWhereClaue($current_user_id, $selected_functionlevel_id, $lowerlevels);
	    }

	    return $where_clause;
	  }

	  /**
	   * Get where-clause, so all employees with a lower functionlevel than logged-in user will retrieved
	   * Do not check on 'hours_approve_projectteam_members_only' and 'any_user'-right (else filtering won't work)
	   *
	   * @param int $current_user_id
	   * @param int $selected_functionlevel_id
	   * @param string lowerlevels checkbox can be 'on' or 'off'
	   */
	  function getFunctionLevelEmployeeWhereClaue($current_user_id, $selected_functionlevel_id, $lowerlevels)
	  {
      require_once(moduleDir('advancedsecurity').'utils/class.functionlevelsutils.inc');
      $flutils = &atknew('advancedsecurity.utils.functionlevelutils');

      if ($lowerlevels == 'on')
      {
        $levels = $flutils->getFunctionLevelBelow($selected_functionlevel_id, true);

        if (!empty($levels))
        {
          if (count($levels) == 0)
          {
            $ids = "NULL";
          }
          else
          {
            foreach ($levels as $level) $ids .= "'".$level['id']."',";

            $ids = substr($ids, 0, -1); // strip the last comma
          }

          $where = "person.functionlevel IN (".$ids.")";
        }
        else
        {
          $where = "person.functionlevel='$selected_functionlevel_id'";
        }
      }
      else if ($lowerlevels == 'off')
      {
        $where = "person.functionlevel='$selected_functionlevel_id'";
      }
      else
      {
        atkdebug("Unknown value '$lowerlevels' for checkbox", DEBUG_WARNING);
      }

      return $where;
	  }

	  /**
	   * Select the employees that are managed by the current user
	   * Or all employees, if user has this right
	   *
	   * @param int $current_user_id
	   * @return string $where
	   */
	  function getDefaultEmployeeWhereClaue($current_user_id)
	  {
	    $where = ($this->userHasAnyUserRight($current_user_id)) ? "" : "person.supervisor = '$current_user_id'";

	    $add_projectmembers_filter = atkConfig::get('timereg','hours_approve_projectteam_members_only');

	    if ($add_projectmembers_filter == true && $current_user_id != -1 && !is_null($current_user_id))
	    {
        // get employees who are in the projectteams that have current user
        // as project leader.
        $query = "SELECT DISTINCT
                    project_person.personid
                  FROM
                    project_person
                  LEFT JOIN project ON (project.id = project_person.projectid)
                  WHERE
                    project.coordinator = '$current_user_id'
        ";

        $g_db = &atkGetDb();
        $results = $g_db->getRows($query);

        if (is_array($results) && sizeof($results) > 0)
        {
          $in = array();
          foreach($results as $row)
            $in[] = $row['personid'];

          if ($where != "")
            $where .= " AND";
          $where .= " person.id IN ('" . implode("','",$in) . "')";
        }
	    }

	    return $where;
	  }

	  /**
	   * Check if logged-in user has 'any_user' right
	   * @todo since this module does not work when logged-in as administrator, do we need to check on user_id == -1 or empty?
	   * @param int $current_user_id
	   * @return boolean true if user has 'any_user' right
	   */
	  function userHasAnyUserRight($current_user_id)
	  {
	    return $this->allowed("any_user") || $current_user_id == -1 || is_null($current_user_id);
	  }

	  /**
	   * Get employeedata and construct table
	   *
	   * @param int $current_user_id
	   * @param int $year
	   * @param int $selected_functionlevel_id
	   * @param string lowerlevels checkbox can be 'on' or 'off'
	   * @return array $result
	   */
	  function getEmployeesTable($current_user_id, $year, $selected_functionlevel_id, $lowerlevels)
	  {
	    // Get the employees/periods data from the database
	    $users = $this->getEmployeesData($current_user_id, $year, $selected_functionlevel_id, $lowerlevels);

	    // Initialize the tabledata array
	    $tabledata = array();

	    // Add the header columns to the tabledata
	    $tabledata[] = array(atktext('employee'), atktext('periods'));

	    // Loop through the users array in order to add a row for every user
	    foreach ($users as $user)
	    {
	      // Add all periods as links into the periods string
	      $periods = '';
	      foreach ($user['period'] as $periodnumber => $perioddata)
	      {

	        // Prefix periodnumbers below 10 with a padding zero
	        if ($periodnumber<10 && strlen($periodnumber) == 1 ) $prefix = 0;
	        else $prefix = null;

	        // Determine the color to be used when displaying the periodnumber
	        if ($perioddata["approved"]=="1")
	          $color = "#009900";
	        else
	          $color = "#FF0000";

          // Translate month number to month name, if we are using monthly locking
          $monthlist = dateutil::monthlist();
          $periodname = ($this->m_lockmode == "month") ? atktext($monthlist[ltrim($periodnumber, "0")]) : $periodnumber;

	        // Add a periodnumberlink to the periods string
	        $periods.=href('dispatch.php?viewuser='.$user["id"].'&viewdate='.date("Y-m-d",$this->getdatefromperiod($year.$prefix.$periodnumber))."&periodid=".$perioddata["id"],$periodname, SESSION_DEFAULT, false, 'style="color: '.$color.';"')."&nbsp;";

	      }

	      if ($periods == "")
	      {
          $periods = sprintf(atktext('hours_approve_noblockedperiod'), $this->m_lockmode);
	      }

	      // Add a row to the table containing the employeename and the period links
	      $tabledata[] = array($user["name"], $periods);
	    }

	    // Let the tablerenderer do the rendering job
	    $tablerenderer = &atknew("atk.utils.atktablerenderer");
      $result = $tablerenderer->render($tabledata, TBL_DATA, "recordlist");

	    // Return the HTML output form the tablerenderer
      return $result;
	  }

	  function action_approve(&$handler)
	  {
      global $g_sessionManager;
      $viewdate = $this->m_postvars['viewdate'];
      $viewuser = $this->m_postvars['viewuser'];
      $periodid = $this->m_postvars['periodid'];

	    $hourslocknode = &atkGetNode('timereg.hours_lock');

	    if (is_numeric($periodid))
	    {
	      $extra_selector = "";
	      if (moduleExists('timereg') && atkconfig::get('timereg','approve_period_per_coordinator') == true)
	      {
	        $loggedinuser = &getUser();
	        $current_user_id = $loggedinuser['id'];
	        if ($current_user_id != -1 && !is_null($current_user_id))
	         $extra_selector = " AND hours_lock.coordinator='$current_user_id' ";
	    }

	      $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$periodid."'" . $extra_selector);
	      $hourlock = $hourlocks[0];

        $hourlock["approved"] = 1;

        // If the userid of the lock is empty, this is a lock that is valid
        // for all users.
        if (empty($hourlock['userid']['id']))
        {
          // Create copy of lock, valid for only this user.
          // This lock will override the more general one.
          $hourlock['userid']['id'] = $viewuser;
          $hourslocknode->addDb($hourlock);
        }
        else
        {
        $hourslocknode->updateDb($hourlock);
        }
      }

	    $g_sessionManager->pageVar("viewdate","");
	    $this->redirect();
	  }

	  function action_disapprove(&$handler)
	  {
      if (isset($this->m_postvars["atkcancel"]))
      {
        $this->redirect();
        return $handler;
      }

      // Get the periodid
	    $periodid = $this->m_postvars["periodid"];

	    // Throw an error if the periodid is not numeric
	    if (!is_numeric($periodid))
	    {
	      atkerror("Given periodid is not numeric");
  	    return "";
	    }

	    // Get a reference to the hours_lock node
	    $hourslocknode = &atkGetNode("timereg.hours_lock");

	    // Get the lock
	    $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$periodid."'");
	    $hourlock = $hourlocks[0];
      $hoursapproved = $hourlock["approved"];

	    // Check if the lock exists
	    if (count($hourlocks)==0)
	    {
	      atkerror("The given period lock can not be found");
	      return "";
	    }

	    // Get a reference to the global sessionmanager
	  	global $g_sessionManager;

	  	// Make sure the reason session variable is available at all times
	  	$g_sessionManager->pageVar("reason");
      $g_sessionManager->pageVar("atkdodisapprove");

      $page = &$this->getPage();
      $page->register_style($theme->stylePath("style.css"));
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();

      if (!isset($this->m_postvars["reason"]))
      {

        $reqimg = '<img align="top" onMouseOver="javascript:window.status=\''.
                     addslashes(atktext("field_obligatory")).'\';" src="'.$theme->imgPath("required_field.gif").'" border="0" alt="'.
                     atktext("field_obligatory").'">';

        // Show reason input dialog
        $content = '<form action="'. dispatch_url("timereg.hours_approve", "disapprove") . '" method="post">';
        $content.= session_form($hoursapproved ? SESSION_DEFAULT : SESSION_BACK);
        $content.= '<table name="entryform" id="entryform" cellspacing="0">';
        $content.= '<tr><td colspan="2" class="fieldlabel"><b>' . atktext("hours_approve_disapprovegivereason", "timereg") . '</b></td></tr>';
        $content.= '<tr><td colspan="2" class="fieldlabel">&nbsp;</td></tr>';
        $content.= '<tr valign="top">';
        $content.= '<td class="fieldlabel">' . atktext("reason") . ' ' . $reqimg . '</td>';
        $content.= '<td><input id="reason" name="reason" value=""></td>';
        $content.= '</tr>';
        $content.= '</table>';
        $content.= '<br>';
        $content.= '<input type="submit" class="btn_save" name="atk' . ($hoursapproved ? "confirm" : "do") . 'disapprove" value="'.atktext('disapprove').'">&nbsp;&nbsp;';
        $content.= '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext('cancel').'">';
        $content.= '</form>';
        $content.= '<br>';

        $title = atktext("generate", "docmanager") . " " . $documenttype["name"];

        $ui = $this->getUi();
        $box = $ui->renderbox(array("title"=>$title,"content"=>$content));
        $actionpage = $this->renderActionPage("stats", $box);
        $page->addContent($actionpage);

        return $handler;
      }


      if (!isset($this->m_postvars["atkdodisapprove"]))
      {

        // Show confirmation dialog
        $content = '<form action="'. dispatch_url("timereg.hours_approve", "disapprove", array("periodid"=>$this->m_postvars["periodid"],"reason"=>$this->m_postvars["reason"])) . '" method="post">';
        $content.= session_form(SESSION_BACK);
        $content.= '<table name="entryform" id="entryform" cellspacing="0">';
        $content.= '<tr><td colspan="2" class="fieldlabel"><b>' . sprintf(atktext("hours_approve_disapproveofapprovedperiodconfirmation", "timereg"), $this->m_lockmode) . '</b></td></tr>';
        $content.= '</table>';
        $content.= '<br>';
        $content.= '<input type="submit" class="btn_save" name="atkdodisapprove" value="'.atktext('disapprove').'">&nbsp;&nbsp;';
        $content.= '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext('cancel').'">';
        $content.= '</form>';
        $content.= '<br>';

        $title = atktext("generate", "docmanager") . " " . $documenttype["name"];

        $ui = $this->getUi();
        $box = $ui->renderbox(array("title"=>$title,"content"=>$content));
        $actionpage = $this->renderActionPage("stats", $box);
        $page->addContent($actionpage);

        return $handler;
      }


      // Get the employee
      $employeenode = &atkGetNode("employee.employee");
      $employees = $employeenode->selectDb("person.id = '".$hourlock["userid"]["id"]."'");

      if (!empty($hourlock["userid"]["id"]) && count($employees)==0)
      {
        atkerror("Could not find the owner of the hours lock");
        return "";
      }
      $employee = $employees[0];

      // Send an e-mail
      $to = $employee["email"];
      $subject = atktext("hours_approve_registrationdisapprovedsubject");
      $monthlist = dateutil::monthlist();
      $periodno = substr($hourlock["period"], 4);
      $period = ($this->m_lockmode == "month") ? $monthlist[$periodno] : $periodno; //:substr($hourlock["period"], 4);
      $yearno = substr($hourlock["period"], 0, 4);
      $body = sprintf(atktext("hours_approve_registrationdisapprovedbody"), $this->m_lockmode, $period, $yearno, $this->m_postvars["reason"]);
      usermail($to, $subject, $body);

      // Delete the hours_lock
      if (atkconfig::get('timereg','approve_period_per_coordinator'))
      {
        $hourslocknode->deleteDb("hours_lock.period = '{$hourlock["period"]}' AND hours_lock.userid = '{$hourlock["userid"]["id"]}'");
      }
      else
        // Doesn't delete the period if it is a lock that is made for "everyone"
	      $hourslocknode->deleteDb("hours_lock.id='".$periodid."' AND hours_lock.userid IS NOT NULL");

	    // Redirect to the hours_approve view
	    $this->redirect();
	  }

	  /**
	   * Conversion script for old database structure
	   *
	   * @param atkActionHandler &$handler Action handler for the conver_old action
	   */
	  function action_convert_old(&$handler)
	  {
	    // Get the approved and old hours
	    $db = &atkGetDb();
	    $approvedhours = $db->getrows("SELECT * FROM hours_approved");
	    $lockedhours = $db->getrows("SELECT * FROM hours_lock");

        // Get the locked hours by weeks
	    $lockedhoursbyweek = array();
	    foreach ($lockedhours as $lockedhour)
	    {
	      $lockedhoursbyweek[$lockedhour['week']][$lockedhour['userid']] = $lockedhour;
	    }

	    // Convert to the new datastructures
	    foreach ($approvedhours as $approvedhour)
	    {
	      if ($lockedhoursbyweek[$approvedhour['week']][$approvedhour['userid']])
	      {
	        $db->query("UPDATE `hours_lock` SET `approved` = '1' WHERE `id` = '".$lockedhoursbyweek[$approvedhour['week']][$approvedhour['userid']]['id']."'");
	      }
	      else
	      {
	        $db->query("INSERT INTO `hours_lock` (`period`,`userid` ,`approved`) VALUES ('".$approvedhour['week']."', '".$approvedhour['userid']."', '1');");
	      }
	    }

	    // Drop the hours_approved table
	    $db->query("DROP TABLE `hours_approved`");

	    // Go back to the previous page
	    $this->redirect();
	  }
	}

?>
