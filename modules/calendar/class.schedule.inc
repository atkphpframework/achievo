<?php

/**
 * Module Calendar
 * Class schedule
 *
 * This class makes it possible to schedule a appointment.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

 // calendar base class
  include_once (moduleDir("calendar")."class.calendar.inc");
  include_once (moduleDir("calendar")."class.base_dayview.inc");
  include_once (moduleDir("calendar")."class.base_monthview.inc");
  include_once (moduleDir("calendar")."class.base_weekview.inc");
  include_once (moduleDir("calendar")."class.base_employeeview.inc");

  useattrib("atkdateattribute");
  useattrib("atktextattribute");
  useattrib("atkboolattribute");
  useattrib("atktimeattribute");
  useattrib("atklistattribute");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");

  class schedule extends atkNode
  {
    var $m_viewdate; 
    var $m_user;
    var $m_view;
  
    function schedule()
    {
      global $g_user, $g_sessionManager;

      $time = $g_sessionManager->stackVar("time");
      
      if ($time!="")
      {
        $default_start_time = $time; // passed from the url.

        // endtime is half an hour later than starttime by default
        $default_end_time = strftime("%H:%M", mktime(substr($time,0,2), substr($time,3,2)+30));
      }
      else
      {
        $default_start_time = strftime("%H:%M");
        $default_end_time = strftime("%H:%M", mktime(substr($default_start_time,0,2),substr($default_end_time,3,2)+30));
      }

      $aItemBegin = array("08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30","14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00","21:30","22:00","22:30","23:00","23:30","24:00");
      $aItemEnd   = $aItemBegin;

      $this->atkNode("schedule", NF_ADD_LINK|NF_EDITAFTERADD);

      $this->add(new atkAttribute("id"              , AF_AUTOKEY));
      $this->add(new atkDateAttribute("startdate"  , "F d Y","d F Y", 0, 0 ,AF_OBLIGATORY));
      $this->add(new atkDateAttribute("enddate"    , "F d Y","d F Y", 0, 0 ,AF_OBLIGATORY));
      $this->add(new atkTimeAttribute("starttime"  , 8, 24, array("00","30"), $default_start_time, 0));
      $this->add(new atkTimeAttribute("endtime"    , 8, 24, array("00","30"), $default_end_time, 0));
      $this->add(new atkBoolAttribute("allday"));
      $this->add(new atkAttribute("title"          , AF_SEARCHABLE|AF_OBLIGATORY, 50));
      $this->add(new atkTextAttribute("description"));
      $this->add(new atkAttribute("location"        , AF_SEARCHABLE, 50));
      $this->add(new atkBoolAttribute("publicitem"));
      $this->add(new atkBoolAttribute("all_users"));
      $this->add(new atkListAttribute("status", array("confirmed", "unconfirmed", "cancelled")));
      $this->add(new atkManyToOneRelation("scheduletype", "calendar.schedule_types",AF_OBLIGATORY|AF_RELATION_AUTOLINK));
      $this->add(new atkOneToManyRelation("attendees", "calendar.schedule_attendee", "schedule_id", AF_HIDE_LIST|AF_CASCADE_DELETE));
      $this->add(new atkOneToManyRelation("notes", "calendar.schedule_notes", "schedule_id", AF_HIDE_LIST|AF_CASCADE_DELETE));
      $this->add(new atkAttribute("owner"           , AF_READONLY|AF_HIDE_ADD, 15));

      $this->m_view     = $g_sessionManager->stackVar("view");
      if (trim($this->m_view) == "") { $this->m_view = "dayview"; }
      
      $this->m_user     = $g_sessionManager->stackVar("user");
      if ($this->m_user=="") $this->m_user = $g_user["name"];
      
      $this->m_viewdate = $g_sessionManager->stackVar("viewdate");      
      if ($this->m_viewdate=="") $this->m_viewdate = strftime("%Y-%m-%d");

      $this->addAllowedAction("view");

      $this->setTable("schedule", "schedule");
      if ($this->m_view == "dayview")
      {
        $this->setOrder("startdate DESC");
      }
      else
      {
        $this->setOrder("startdate DESC, starttime");
      }
    }


    function initial_values()
    {
      global $g_user;

      $initial["owner"] = $g_user["name"];
      $initial["publicitem"] = 1;

      if ($this->m_viewdate!="")
      {
        $arr_date = array("year"=>substr($this->m_viewdate, 0, 4),
                          "month"=>substr($this->m_viewdate, 5, 2),
                          "day"=>substr($this->m_viewdate, 8, 2));
        $initial["startdate"] = $arr_date;
        $initial["enddate"] = $arr_date;
      }
      return $initial;
    }

    function enddate_validate(&$record, $mode)
    {
      if ($record["enddate"]["year"] < $record["startdate"]["year"])
      {
        triggerError($record, "enddate", "error_date");
      }
      elseif ($record["enddate"]["year"] == $record["startdate"]["year"])
      {
        if ($record["enddate"]["month"] < $record["startdate"]["month"])
        {
          triggerError($record, "enddate", "error_date");
        }
        elseif ($record["enddate"]["month"] == $record["startdate"]["month"])
        {
          if ($record["enddate"]["day"] < $record["startdate"]["day"])
          {
            triggerError($record, "enddate", "error_date");
          }
        }
      }
    }

    function endtime_validate(&$record, $mode)
    {
      if ($record["endtime"]["hours"] < $record["starttime"]["hours"])
      {
        triggerError($record, "endtime", "error_time");
      }
      elseif ($record["endtime"]["hours"] == $record["starttime"]["hours"])
      {
        if ($record["endtime"]["minutes"] <= $record["starttime"]["minutes"])
        {
          triggerError($record, "endtime", "error_time");
        }
      }
    }

    function get_employees($user_id)
    {
      global $g_db;

      $sql = "SELECT lastname,firstname,userid
              FROM person
              WHERE status='active' AND role='employee'
              ORDER BY lastname
             ";

      $records = $g_db->getrows($sql);
      $employee_code='<OPTION VALUE="all">'.text("allusers");
      for($i=0;$i<count($records);$i++)
      {
        if($user_id==$records[$i]["userid"]) { $sel="SELECTED"; } else { $sel=""; }
        $employee_code.='<OPTION VALUE="'.$records[$i]["userid"].'" '.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      return $employee_code;
    }

    function get_employees2()
    {
      global $g_db, $selemployee;

      $sql = "SELECT lastname,firstname,userid
              FROM person
              WHERE status='active' AND role='employee'
              ORDER BY lastname
             ";
      $records = $g_db->getrows($sql);

      if ($selemployee["all"] == "all") { $sel="CHECKED"; } else { $sel=""; }
      $employee_code='<INPUT TYPE="checkbox" VALUE="all" NAME="employee[]" '.$sel.'><font size="-1">'.text("allusers")."</font><br>";
      for($i=0;$i<count($records);$i++)
      {
        if($selemployee[$records[$i]["userid"]]==$records[$i]["userid"]) { $sel="CHECKED"; } else { $sel=""; }
        $employee_code.='<INPUT TYPE="checkbox" VALUE="'.$records[$i]["userid"].'"  NAME="employee[]" '.$sel.'><font size="-1">'.$records[$i]["lastname"].', '.$records[$i]["firstname"]."(".$records[$i]["userid"].")</font><br>";
      }
      return $employee_code;
    }

    function adminPage()
    {
      global $g_layout;

      if (method_exists($this, $this->m_view))
      {
        $funcname = $this->m_view;
        $result = $this->$funcname($this->m_viewdate);
      }
      else
      {
        $result = "Niet bestaande view (".$this->m_view.")";
      }

      $g_layout->initGUI();
      $g_layout->ui_top(text("Achievo scheduler / ".$this->m_view));
      $g_layout->output($this->statusbar());
      $g_layout->output($result);
      $g_layout->ui_bottom();
    }

    function monthview($viewdate)
    {
      global $g_user, $g_db;

      // viewdate is something like 2001-07-23
      $month = substr($viewdate, 5,2);
      $year  = substr($viewdate, 0,4);

      // get all users we can view
      $form  = "<FORM method='post' action='".$_SERVER["PHP_SELF"]."'>\n";
      $form .= session_form();
      $form .= "<input type='hidden' name='viewdate' value='$viewdate'>\n";
      $form .= "<SELECT name='user'>\n";
      $form .= $this->get_employees($this->m_user);
      $form .= "</SELECT>\n";
      $form .= "&nbsp;<input type='submit' value='".text("refresh")."'>\n";
      $form .= "</FORM>\n";

      //get the rendered matrix
      $dummy = new monthView($month, $year);
      $dummy->viewerWidth = "600";
      $dummy->owner       = $g_user["name"];
      $dummy->showGlobal  = $this->m_user;
      $dummy->generate();

      // assign our output buffer
      $buff .= "<br><br><table width='".$dummy->viewerWidth."'><tr>";
      $buff .= "<td valign='top' align='left'>".$dummy->monthNav()."</td>";
      $buff .= "<td valign='top' align='right'>$form</td>";
      $buff .= "</tr><tr><td valign='top' align='left'><b>".$dummy->title."</b></td>";
      $buff .= "</tr></table>";
      $buff .= $dummy->dumpMatrix();

      // build the legend
      $legend  = "<TABLE border='0' cellPadding='1'><TBODY><TR><TD bgColor='#000000'><TABLE width='100%' border='0' cellPadding='2' cellSpacing='0'><TBODY>";

      $tmpLegend  = "<TR bgcolor='#FFFFFF'><TD>\n";
      $tmpLegend .= "<table border='0' cellpadding='0' cellspacing='0'><tr><td bgcolor='#FFFFFF'><img src='images/trans.gif' border='1' width='10' height='10'></td></tr></table>";
      $tmpLegend .= "</TD><TD><font color='#000000' face='verdana' size='-1'>".text("no_appointments")."</font></TD>";
      $tmpLegend .= "</TR>";
      $tmpLegend .= "<TR bgcolor='#FFFFFF'><TD>\n";
      $tmpLegend .= "<table border='0' cellpadding='0' cellspacing='0'><tr><td bgcolor='#FFFF00'><img src='images/trans.gif' border='1' width='10' height='10'></td></tr></table>";
      $tmpLegend .= "</TD><TD><font color='#000000' face='verdana' size='-1'>".text("appointments")."</font></TD>";
      $tmpLegend .= "</TR>";
      $tmpLegend .= "<TR bgcolor='#FFFFFF'><TD>\n";
      $tmpLegend .= "<table border='0' cellpadding='0' cellspacing='0'><tr><td bgcolor='#FFCC33'><img src='images/trans.gif' border='1' width='10' height='10'></td></tr></table>";
      $tmpLegend .= "</TD><TD><font color='#000000' face='verdana' size='-1'>".text("today")."</font></TD>";
      $tmpLegend .= "</TR>";
      $tmpLegend .= "<TR bgcolor='#FFFFFF'><TD>\n";
      $tmpLegend .= "<table border='0' cellpadding='0' cellspacing='0'><tr><td bgcolor='#FF0000'><img src='images/trans.gif' border='1' width='10' height='10'></td></tr></table>";
      $tmpLegend .= "</TD><TD><font color='#000000' face='verdana' size='-1'>".text("today_and_appointments")."</font></TD>";
      $tmpLegend .= "</TR>";
      $legend    .= $tmpLegend;

      $buff .= $legend."</TBODY></TABLE></TD></TR></TBODY></TABLE>";

      return $buff;
    }

    function employeeview($viewdate)
    {
      global $g_user, $g_db;
      
      $selemployee = $_REQUEST["selemployee"];

      if (!is_array($viewdate))
      {
        // viewdate is something like 2001-07-23
        $day   = substr($viewdate, 8,2);
        $month = substr($viewdate, 5,2);
        $year  = substr($viewdate, 0,4);
      }
      else
      {
        $day   = $viewdate["day"];
        $month = $viewdate["month"];
        $year  = $viewdate["year"];
      }

      $selemployee = Array();
      for ($i=0;$i<count($this->m_postvars["employee"]);$i++)
      {
        $selemployee[$this->m_postvars["employee"][$i]] = $this->m_postvars["employee"][$i];
      }

      // get all users we can view
      $form  = "<FORM name='frmemployeeview' method='post' action='".$_SERVER["PHP_SELF"]."'>\n";
      $form .= session_form();
      $form .= "<input type='hidden' name='viewdate' value='$viewdate'>\n";
      $form .= $this->get_employees2();
      $form .= "<br>";

      $recs["viewdate"]["day"] = $day;
      $recs["viewdate"]["month"] = $month;
      $recs["viewdate"]["year"] = $year;
      $dateattrib = new atkDateAttribute("viewdate"  , "F d Y","d F Y", 0, 0);
      $form .= $dateattrib->edit($recs,"","");
      $form .= "<br>";
      $form .= "&nbsp;<br><input type='submit' value='".text("refresh")."'>\n";
      $form .= "</FORM>\n";

      $dummy = new employeeView($day, $month, $year);
      $dummy->viewerWidth = "600";

      $buff  = href($_SERVER["PHP_SELF"]."?view=dayview&viewdate=$viewdate",text("dayview"))."<br><br>";
      $buff .= "<font size='-1'>".text("seluserdate")."</font><br><br>";

      // assign our output buffer
      $buff .= "<table width='".$dummy->viewerWidth."'><tr>";
      $buff .= "<td valign='top' align='left'>$form</td>";
      $buff .= "</tr></table><br>";
      $buff .= $dummy->dumpMatrix($this->m_postvars["employee"]);

      return $buff;
    }

    function dayview($viewdate)
    {
      global $g_user, $g_db;

      // viewdate is something like 2001-07-23
      $day   = substr($viewdate, 8,2);
      $month = substr($viewdate, 5,2);
      $year  = substr($viewdate, 0,4);

      // get all users we can view
      $form  = "<FORM method='post' action='".$_SERVER["PHP_SELF"]."'>\n";
      $form .= session_form();
      $form .= "<input type='hidden' name='viewdate' value='$viewdate'>\n";
      $form .= "<SELECT name='user'>\n";
      $form .= $this->get_employees($this->m_user);
      $form .= "</SELECT>\n";
      $form .= "&nbsp;<input type='submit' value='".text("refresh")."'>\n";
      $form .= "</FORM>\n";

      // get the rendered matrix
      $dummy = new dayView($day, $month, $year);
      $dummy->viewerWidth = "600";
      $dummy->owner       = $g_user["name"];
      $dummy->showGlobal  = $this->m_user;
      $dummy->generate();

      // get the legend
      $legend .= $dummy->legend;

      // link to weekview and monthview and employeeview
      $buff = href($_SERVER["PHP_SELF"]."?view=weekview&viewdate=$viewdate",text("weekview")) ."&nbsp;". href($_SERVER["PHP_SELF"]."?view=monthview&viewdate=$viewdate", text("monthview"))."&nbsp;".href($_SERVER["PHP_SELF"]."?view=employeeview&viewdate=$viewdate", text("employeeview"));

      // assign our output buffer
      $buff .= "<br><br><table width='".$dummy->viewerWidth."'><tr>";
      $buff .= "<td valign='top' align='left'>".$dummy->dayNav()."</td>";
      $buff .= "<td valign='top' align='right'>$form</td>";
      $buff .= "</tr><tr><td valign='top' align='left'><b>".$dummy->title."</b></td>";
      $buff .= "</tr></table>";
      $buff .= $dummy->dumpMatrix();
      $buff .= $legend."<br>";

      return $buff;
    }

    function weekview($viewdate)
    {
      global $g_user, $g_db;

      // viewdate is something like 2001-07-23
      $day   = substr($viewdate, 8,2);
      $month = substr($viewdate, 5,2);
      $year  = substr($viewdate, 0,4);

      // get all users we can view
      $form  = "<FORM method='post' action='".$_SERVER["PHP_SELF"]."'>\n";
      $form .= session_form();
      $form .= "<input type='hidden' name='viewdate' value='$viewdate'>\n";
      $form .= "<SELECT name='user'>\n";
      $form .= $this->get_employees($this->m_user);
      $form .= "</SELECT>\n";
      $form .= "&nbsp;<input type='submit' value='".text("refresh")."'>\n";
      $form .= "</FORM>\n";

      // get the rendered matrix
      $dummy = new weekView($day, $month, $year);
      $dummy->viewerWidth = "600";
      $dummy->owner       = $g_user["name"];
      $dummy->showGlobal  = $this->m_user;
      $dummy->generate();

      // assign our output buffer
      $buff .= "<br><br><table width='".$dummy->viewerWidth."'><tr>";
      $buff .= "<td valign='top' align='left'>".$dummy->weekNav()."</td>";
      $buff .= "<td valign='top' align='right'>$form</td>";
      $buff .= "</tr><tr><td valign='top' align='left'><b>".$dummy->title."</b></td>";
      $buff .= "</tr></table>";
      $buff .= $dummy->dumpMatrix();

      return $buff;
    }
  }

?>