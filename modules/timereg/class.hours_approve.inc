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

    /**
     * Constructor
     */
	  function hours_approve()
	  {
	    // Call the base class constructor
	    $this->hours();

	    // Get a reference to the global sessionmanager
	  	global $g_sessionManager;

	  	// Make sure the weekid and approved session variables are available at all times
	    $g_sessionManager->pageVar("weekid");
	    $g_sessionManager->pageVar("approved");
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
	    // Add an approve or disapprove button to the weekview
	    if ($this->m_postvars['approved']==1)
  	    $output = "<br />".atkButton(atktext("disapprove"),dispatch_url("timereg.hours_approve","disapprove"));
	    else if ($this->m_postvars['approved']==0)
	      $output = "<br />".atkButton(atktext("approve"),dispatch_url("timereg.hours_approve","approve"));

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
	    // Get a reference to the databeest
	  	$db = &atkGetDb();

	    // Detect the old datastructure
	  	$showtable = $db->getrows("SHOW TABLES LIKE 'hours_approved'");

      // If old datastructures found, then show link to conversion action
	  	if (!empty($showtable[0]))
	    {
	      return href(dispatch_url("timereg.hours_approve","convert_old"), "Converteer oude gegevens"); // @todo translate
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

	    // Start the output using a session form
	    $formbegin = '<form name="entryform" enctype="multipart/form-data" action="dispatch.php" method="post" onsubmit="globalSubmit(this)">';
	    $formbegin.= session_form();
	    $page->addContent($formbegin);

	    // If a week is selected for a specific employee, then show the approval screen
	    if ($this->m_viewdate)
	    {
	    	$page->addContent($this->approvescreen());
	    }

	    // Add the approvedweekview to the page
	    $page->addContent($this->approvedweekview());

	    // Close the form
	    $formend = '</form>';
	    $page->addContent($formend);

	    // Return an empty string (output is handled through the page
	    return "";
	  }

	  /**
	   * Loads the viewdate sessionmanager var and returns the weekview html result
	   *
	   * @return string HTML representation of the weekview
	   */
	  function approvescreen()
	  {
	    // Get a reference to the global sessionmanager
	  	global $g_sessionManager;

	    // Get the viewdate from the sesssion manager and store it in this node
	  	$this->m_viewdate = $g_sessionManager["viewdate"];

	    // Return the weekview result
	  	return $this->weekview();
	  }

	  function approvedweekview()
	  {
	    // Get the current user
	    $user = getUser();

	    if (is_numeric($this->m_postvars['year']) && strlen($this->m_postvars['year'])==4)
	      $year = trim($this->m_postvars['year']);
	    else
	      $year = date('Y');

	    $yearattrib = new atkNumberAttribute("year",0,6);

	    $result = atktext('year')." : ".$yearattrib->edit(array("year"=>$year));
	    $result.= '<input type="submit" name="atkbtn1" value="'.atktext("view").'"><br>';
	    $result.= '<br>';

	    $result.= $this->showEmployees($user['id'], $year);
	    $result.= '<br />' . atktext('hours_approve_color_explain');
	    $result.= "<br /><br />".$this->convertheck();

	    $ui = &atkui::getInstance();
	    $boxedresult = $ui->renderBox(array("title"=>atktext('title_houradmin_approve'),"content"=>$result));

	    return $boxedresult;
	  }

	  function getdatefromweek($yearweek)
	  {
	    $weekEndDay = 0;
	      // 0 = Sunday, 1 = Monday etc.
	    $year = (int) substr($yearweek, 0, 4);
	    $week = (int) substr($yearweek, -2, 2);
	    $dayOfYear = 4 + (($week - 1) * 7);
	    $date = mktime(0, 0, 0, 1, $dayOfYear, $year);
	    $dayOfWeek = date("w", $date);
	    $daysToAdd = ($weekEndDay - $dayOfWeek + 7) % 7;
	    $date += $daysToAdd * 24*60*60;
	    return $date;
	  }


	  function showEmployees($userid, $year)
	  {
	    global $prevnextlinks;

	    $employeenode = new atkNode("tools");
	    $employeenode = &getNode("employee.employee");
	    if ($userid) $where = "person.supervisor = '$userid'";
	    $data = $employeenode->selectDb($where, null, null, null, array('id', 'lastname', 'firstname', 'supervisor'));

	    $users = array();
	    foreach ($data as $key => $user)
	    {
	      $users[$key]["id"]   = $user["id"];
	      $users[$key]["name"] = $user["firstname"].' '.$user["lastname"];
	      $users[$key]["weeks"]=array();

	      $houslocknode = &getNode("timereg.hours_lock");
	      $lockedweeks = $houslocknode->selectDb("hours_lock.userid='".$user["id"]."'");

	      foreach ($lockedweeks as $lockedweek)
	      {
	        if (substr($lockedweek["week"],0,4)==$year)
	        {
	          $users[$key]["weeks"][substr($lockedweek["week"],4)]["id"]= $lockedweek['id'];
	          $users[$key]["weeks"][substr($lockedweek["week"],4)]["approved"] = $lockedweek["approved"];
	        }
	      }
	      ksort($users[$key]["weeks"]);
	    }
	    return $this->makeLinks($users,$year);
	  }

	  function makeLinks($users, $year)
	  {
	    $result.= "<table border=\"0\">";

	    $result.= "<thead><tr><th>".atktext("employee")."</th><th align='center'>".atktext('weeks')."</th></tr></thead>\n";

	    foreach ($users as $user)
	    {
	      $result.= "\n<tr>";
	      $result.= "<td>".$user["name"]."</td>\n";

	      foreach ($user['weeks'] as $weeknumber => $weekdata)
	      {
	        if ($weeknumber<10) $prefix = 0;
	        else $prefix = null;

	        if ($weekdata["approved"]==1)
	          $color = "#009900";
	        else
	          $color = "#FF0000";

	        $weeks.=href('dispatch.php?viewuser='.$user["id"].'&weekview=1&viewdate='.date("Y-m-d",$this->getdatefromweek($year.$prefix.$weeknumber))."&weekid=".$weekdata["id"]."&approved=".$weekdata["approved"],$weeknumber, SESSION_DEFAULT, false, 'style="color: '.$color.';"')."&nbsp;";
	      }
	      $result.= "<td>$weeks</td>\n";
	      $result.= "</tr>";
	      $weeks = "";
	    }
	    $result.= "</table>";
	    return $result;
	  }

	  function action_approve(&$handler)
	  {
	    $this->updateHoursLockApproval("1");
	  }

	  function action_disapprove(&$handler)
	  {
	    $this->updateHoursLockApproval("0");
	  }

	  function updateHoursLockApproval($approval)
	  {
	    global $ATK_VARS, $g_sessionManager;
	    $viewdate = $this->m_postvars['viewdate'];
	    $weekid = $this->m_postvars["weekid"];

	    $hourslocknode = &getNode("timereg.hours_lock");

	    if (is_numeric($weekid))
	    {
	      $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$weekid."'");
	      $hourlock = $hourlocks[0];

	      $hourlock["approved"] = $approval;
	      $hourslocknode->updateDb($hourlock);
	    }

	    $g_sessionManager->pageVar("viewdate","");
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

        // Get the locked hours by week
	    $lockedhoursbyweek = array();
	    foreach ($lockedhours as $lockedhour)
	    {
	      $lockedhoursbyweek[$lockedhour['week']][$lockedhour['userid']] = $lockedhour;
	    }

	    //
	    foreach ($approvedhours as $approvedhour)
	    {
	      if ($lockedhoursbyweek[$approvedhour['week']][$approvedhour['userid']])
	      {
	        $db->query("UPDATE `hours_lock` SET `approved` = '1' WHERE `id` = '".$lockedhoursbyweek[$approvedhour['week']][$approvedhour['userid']]['id']."'");
	      }
	      else
	      {
	        $db->query("INSERT INTO `hours_lock` (`week`,`userid` ,`approved`) VALUES ('".$approvedhour['week']."', '".$approvedhour['userid']."', '1');");
	      }
	    }

	    // Drop the hours_approved table
	    $db->query("DROP TABLE `hours_approved`");

	    // Go back to the previous page
	    $this->redirect();
	  }
	}

?>