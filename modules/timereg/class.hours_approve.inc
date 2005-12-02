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
	    $this->hours("hours_approve");

	    // Get a reference to the global sessionmanager
	  	global $g_sessionManager;

	  	// Make sure the year session variable is available at all times
  	  $g_sessionManager->pageVar("year");
  	  $g_sessionManager->pageVar("viewdate");
  	  $g_sessionManager->pageVar("weekid");
  	  $g_sessionManager->pageVar("viewuser");

//$hourslocknode = &getNode("timereg.hours_lock");
//$users=array(367,794);
//atk_var_dump($users);
//foreach($users as $userid)
//{
//  atkdebug("<h1>CHECK</h1>");
//  atk_var_dump($userid);
//  $min=rand(1, 30);
//  $max=rand($min, 40);
//  for($i=$min; $i<=$max; $i++)
//  {
//    $record = $hourslocknode->initial_values();
//    $record["week"] = "2005" . $i;
//    $record["userid"] = array("id"=>$userid);
//    $record["approved"] = (rand(0,1)==1)?1:null;
//    $hourslocknode->addDb($record);
//  }
//}
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
      $employeenode = &getNode("employee.employee");
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
	    $weekid = $this->m_postvars["weekid"];

	    // If the weekid isnt numeric, then throw an error, no action can be taken so no buttons are added to the footer.
	    if (!is_numeric($weekid))
	    {
	      atkerror("Posted weekid isn't numeric");
	      return "";
	    }

	    // Retrieve the hours_lock record for the specified weekid
	    $hourslocknode = &getNode("timereg.hours_lock");
      $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$weekid."' AND hours_lock.userid='".$this->m_user."'");

      // Don't display any buttons if there are no hourlocks found
      if (count($hourlocks)==0)
      {
        return "";
      }

      // Determine the approval status
      $hourlockapproved = ($hourlocks[0]["approved"]==1);


      $output = '<br />' . atktext("hours_approve_thestatusofthisweekis") . ": ";
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
  	    $output.= atkButton(atktext("disapprove"), dispatch_url("timereg.hours_approve","disapprove", array("weekid"=>$weekid)), SESSION_NESTED);
	    else
	    {
	      $output.= atkButton(atktext("approve"), dispatch_url("timereg.hours_approve","approve"));
  	    $output.= "&nbsp;&nbsp;".atkButton(atktext("disapprove"), dispatch_url("timereg.hours_approve","disapprove", array("weekid"=>$weekid)), SESSION_NESTED);
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

	    // If a week is selected for a specific employee, then show the approval screen
	    if ((!is_null($this->m_postvars["viewuser"])) && (!is_null($this->m_postvars["viewdate"])) && (($this->m_postvars["year"] == null) || ($this->m_postvars["year"] == substr($this->m_postvars["viewdate"], 0, 4))))
	    {
	    	// Try to find the hour_lock in de database
	    	$hourslocknode = &getNode("timereg.hours_lock");
	    	$hourlocks = $hourslocknode->selectDb("hours_lock.id='" . $this->m_postvars["weekid"] . "'");

	    	// Only show the weekview if the lock still exists:
	    	if (count($hourlocks) > 0)
	    	{

	    	  // Store the employeeid and viewdate in class vars
    	  	$this->m_user = $this->m_postvars["viewuser"];
  	    	$this->m_viewdate = $this->m_postvars["viewdate"];

  	    	// Add the approve screen to the page
  	    	$page->addContent($this->weekview());
	    	}

	    }

	    // Add the approvedweekview to the page
	    $page->addContent($this->approvedweekview());

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
	  function approvedweekview()
	  {
	    // Get the current user
	    $user = getUser();

	    // If the year is given in postvars, then view the approvedweekview for the specified year
	    if (is_numeric($this->m_postvars["year"]) && strlen($this->m_postvars["year"])==4)
	      $year = trim($this->m_postvars["year"]);

	    // Else use the current year
	    else
	      $year = date('Y');

	    // Get the singleton instance of the ui renderer
	    $ui = &atkui::getInstance();

      // Compose the tplvars containing content and legend
      $tplvars_contentwithlegend = array(
        "content" => $this->getEmployeesTable($user["id"], $year),
        "legenditems" => $this->getLegendData()
      );

      $contentwithlegend = $ui->render("contentwithlegend.tpl", $tplvars_contentwithlegend);

	    // Use a numberattrib to get a display for the year inputbox
	    $yearattrib = new atkNumberAttribute("year",0,6);

      // Display the year input form
      $header = atktext("year")." : ".$yearattrib->edit(array("year"=>$year));
      $header.= '<input type="submit" name="atkbtn1" value="'.atktext("view").'"><br>';
      //$header.= atkbutton(atktext("view"), dispatch_url("timereg.hours_approve", "admin"), SESSION_REPLACE);

      // Add an explanation to the approvedweekview
	    $footer = atktext("remark") . ": ";
      $footer.= (!$this->allowed("any_user")) ? atktext("hours_approve_remarkmanager") . '<br />' : '';
	    $footer.= atktext("hours_approve_remarkweekblock");
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
        "align" => "center"
      );

      $boxcontents = $ui->render("vertical.tpl", $tplvars_vertical);

	    // Add a conversion link to the approvedweekview if datastructurs outdated

	    // Put the result into a box
	    $boxedresult = $ui->renderBox(array("title"=>atktext("title_houradmin_approve"),"content"=>$boxcontents));

	    // Return the boxed result HTML
	    return $boxedresult;
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

    /**
     * Showemployees for
     *
     * Full description of the function
     *
     * @param type name description
     * @return type description
     */
	  function getEmployeesData($userid, $year)
	  {
	    // Load the employee node
	    $employeenode = &getNode("employee.employee");

	    // Do we select only the employees the current user is maanger of, or all employees
	    $where = $this->allowed("any_user") ? "" : "person.supervisor = '$userid'";

	    // Get the employees from the database
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
	    return $users;
	  }

	  function getEmployeesTable($userid, $year)
	  {
	    // Get the employees/weeks data from the database
	    $users = $this->getEmployeesData($userid, $year);

	    // Initialize the tabledata array
	    $tabledata = array();

	    // Add the header columns to the tabledata
	    $tabledata[] = array(atktext("employee"), atktext('weeks'));

	    // Loop through the users array in order to add a row for every user
	    foreach ($users as $user)
	    {
	      // Add all weeks as links into the weeks string
	      $weeks = "";
	      foreach ($user['weeks'] as $weeknumber => $weekdata)
	      {

	        // Prefix weeknumbers below 10 with a padding zero
	        if ($weeknumber<10) $prefix = 0;
	        else $prefix = null;

	        // Determine the color to be used when displaying the weeknumber
	        if ($weekdata["approved"]=="1")
	          $color = "#009900";
	        else
	          $color = "#FF0000";

	        // Add a weeknumberlink to the weeks string
	        $weeks.=href('dispatch.php?viewuser='.$user["id"].'&viewdate='.date("Y-m-d",$this->getdatefromweek($year.$prefix.$weeknumber))."&weekid=".$weekdata["id"],$weeknumber, SESSION_DEFAULT, false, 'style="color: '.$color.';"')."&nbsp;";
	      }

	      if ($weeks == "")
	      {
	        $weeks = atktext("hours_approve_noblockedweeksyet");
	      }

	      // Add a row to the table containing the employeename and the week links
	      $tabledata[] = array($user["name"], $weeks);
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
	    $weekid = $this->m_postvars["weekid"];

	    $hourslocknode = &getNode("timereg.hours_lock");

	    if (is_numeric($weekid))
	    {
	      $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$weekid."'");
	      $hourlock = $hourlocks[0];

	      $hourlock["approved"] = 1;
	      $hourslocknode->updateDb($hourlock);
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

      // Get the weekid
	    $weekid = $this->m_postvars["weekid"];

	    // Throw an error if the weekid is not numeric
	    if (!is_numeric($weekid))
	    {
	      atkerror("Given weekid is not numeric");
  	    return "";
	    }

	    // Get a reference to the hours_lock node
	    $hourslocknode = &getNode("timereg.hours_lock");

	    // Get the lock
	    $hourlocks = $hourslocknode->selectDb("hours_lock.id='".$weekid."'");
	    $hourlock = $hourlocks[0];
      $hoursapproved = $hourlock["approved"];

	    // Check if the lock exists
	    if (count($hourlocks)==0)
	    {
	      atkerror("The given week lock can not be found");
	      return "";
	    }

	    // Get a reference to the global sessionmanager
	  	global $g_sessionManager;

	  	// Make sure the reason session variable is available at all times
	  	$g_sessionManager->pageVar("reason");

      $page = &$this->getPage();
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
        $boxedcontent = $ui->renderbox(array("title"=>$title,"content"=>$content));

        $page->register_style($theme->stylePath("style.css"));
        $page->addContent($boxedcontent);
        return $handler;
      }


      if (!isset($this->m_postvars["atkdodisapprove"]))
      {

        // Show confirmation dialog
        $content = '<form action="'. dispatch_url("timereg.hours_approve", "disapprove", array("weekid"=>$this->m_postvars["weekid"],"reason"=>$this->m_postvars["reason"])) . '" method="post">';
        $content.= session_form(SESSION_BACK);
        $content.= '<table name="entryform" id="entryform" cellspacing="0">';
        $content.= '<tr><td colspan="2" class="fieldlabel"><b>' . atktext("hours_approve_disapproveofapprovedweekconfirmation", "timereg") . '</b></td></tr>';
        $content.= '</table>';
        $content.= '<br>';
        $content.= '<input type="submit" class="btn_save" name="atkdodisapprove" value="'.atktext('disapprove').'">&nbsp;&nbsp;';
        $content.= '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext('cancel').'">';
        $content.= '</form>';
        $content.= '<br>';

        $title = atktext("generate", "docmanager") . " " . $documenttype["name"];

        $ui = $this->getUi();
        $boxedcontent = $ui->renderbox(array("title"=>$title,"content"=>$content));

        $page->register_style($theme->stylePath("style.css"));
        $page->addContent($boxedcontent);
        return $handler;
      }

      // Get the employee
      $employeenode = &getNode("employee.employee");
      $employees = $employeenode->selectDb("person.id = '".$hourlock["userid"]["id"]."'");
      if (count($employees)==0)
      {
        atkerror("Could not find the owner of the hours lock");
        return "";
      }
      $employee = $employees[0];

      // Send an e-mail
      $to = $employee["email"];
      $subject = atktext("hours_approve_registrationdisapprovedsubject");
      $weekno = substr($hourlock["week"], 4);
      $yearno = substr($hourlock["week"], 0, 4);
      $body = sprintf(atktext("hours_approve_registrationdisapprovedbody"), $weekno, $yearno, $this->m_postvars["reason"]);
      usermail($to, $subject, $body);

      // Delete the hours_lock
	    $hourslocknode->deleteDb("hours_lock.id='".$weekid."'");

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

        // Get the locked hours by week
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