<?php

 // calendar base class
  include_once "class.calendar.inc";
  include_once "class.base_dayview.inc";
  include_once "class.base_weekview.inc";

  class schedule extends atkNode
  {
   function schedule()
   {
    global $g_sessionManager, $view, $viewdate;

    $aItemBegin = array("08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30","14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00","21:30","22:00","22:30","23:00","23:30","24:00");
    $aItemEnd   = $aItemBegin;
   
    $this->atkNode("schedule", NF_ADD_LINK); 

    $this->addAttribute(new atkAttribute("item_owner"           , AF_READONLY));
    $this->addAttribute(new atkAttribute("item_id"              , AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkAttribute("item_title"           , AF_SEARCHABLE|AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("item_begin_date"  , "F d Y","d F Y", 0, date("Ymd")));
    $this->addAttribute(new atkDateAttribute("item_end_date"    , "F d Y","d F Y", 0, date("Ymd")));
    $this->addAttribute(new atkListAttribute("item_begin_time"  , $aItemBegin, AF_OBLIGATORY));
    $this->addAttribute(new atkListAttribute("item_end_time"    , $aItemEnd, AF_OBLIGATORY));
    $this->addAttribute(new atkBoolAttribute("item_allday"      ));
    $this->addAttribute(new atkBoolAttribute("item_public"      ));
    $this->addAttribute(new atkManyToOneRelation("item_type"    , "schedule_types",AF_OBLIGATORY));

    $view = $g_sessionManager->stackVar("view");

    if (trim($view) == "") { $view = "dayview"; }

    $this->setTable("schedule", "schedule");
    $this->setOrder("item_date DESC");
   }

   function initial_values()
   {
    global $g_user;
    return array("item_owner"=>$g_user["name"]);
   }


   function get_employees($user_id)
   {
    global $g_db;
  
    $sql = "SELECT name,userid
            FROM employee
            WHERE status='active'
            ORDER BY name
           ";

    $records = $g_db->getrows($sql);
    $employee_code='<OPTION VALUE="all">'.text("allusers");
    for($i=0;$i<count($records);$i++)
    { 
      if($user_id==$records[$i]["userid"]) { $sel="SELECTED"; } else { $sel=""; }
      $employee_code.='<OPTION VALUE="'.$records[$i]["userid"].'" '.$sel.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $employee_code;
   }


   function adminPage()
   {
    global $g_sessionManager, $view, $PHP_SELF;
    global $g_layout, $dayview, $g_user;

    if (method_exists($this, $view))
    {
     $result = $this->$view();
    }
    else
    {
     $result = "Niet bestaande view ($view)";
    }

    if (trim($dayview) == "")
    {
     $user = $g_user["name"];
    }
    else
    {   
     $user = $dayview;
    }

    $g_layout->ui_top(text("Achievo scheduler / ".$view));
    $g_layout->output(href($PHP_SELF."?view=weekview&weekview=$user","Weekview") ."&nbsp;". href($PHP_SELF."?view=monthview","Monthview"));
    $g_layout->output($result);
    $g_layout->ui_bottom();    
   }

   function dayview()
   {
    global $g_layout, $viewdate, $g_user, $g_db, $dayview, $PHP_SELF;

    // viewdate is something like 2001-07-23
    $day   = substr($viewdate, 8,2);
    $month = substr($viewdate, 5,2);
    $year  = substr($viewdate, 0,4);

    // set default view
    if (trim($dayview) == "")
    {
     $user = $g_user["name"];
    }
    else
    {
     $user = $dayview;
    }

    // get all users we can view
    $form  = "<FORM method='post' action='$PHP_SELF'>\n";
    $form .= session_form();	
    $form .= "<input type='hidden' name='viewdate' value='$viewdate'>\n";
    $form .= "<SELECT name='dayview'>\n";
    $form .= $this->get_employees($user);
    $form .= "</SELECT>\n";
    $form .= "&nbsp;<input type='submit' name='submit_dayview' value='".text("Refresh")."'>\n";
    $form .= "</FORM>\n";

    // get the rendered matrix
    $dummy = new dayView($day, $month, $year);
    $dummy->viewerWidth = "600";
    $dummy->owner       = $g_user["name"];
    $dummy->showGlobal  = $user;
    $dummy->generate();

    // get the legend
    $legend .= $dummy->legend;

    // assign our output buffer
    $buff  = "<br><br><table width='".$dummy->viewerWidth."'><tr>";
    $buff .= "<td valign='top' align='left'>".$dummy->dayNav()."</td>";
    $buff .= "<td valign='top' align='right'>$form</td>";
    $buff .= "</tr></table>";
    $buff .= $dummy->dumpMatrix();
 
    return $buff;
   }



   function weekview()
   {
    global $g_layout, $viewdate, $g_user, $g_db, $weekview, $PHP_SELF;

    // viewdate is something like 2001-07-23
    $day   = substr($viewdate, 8,2);
    $month = substr($viewdate, 5,2);
    $year  = substr($viewdate, 0,4);

   // set default view
    if (trim($weekview) == "")
    {
     $user = $g_user["name"];
    }
    else
    {   
     $user = $weekview;
    }

    $form  = "<FORM method='post' action='$PHP_SELF'>\n";
    $form .= session_form();    
    $form .= "<input type='hidden' name='viewdate' value='$viewdate'>\n";
    $form .= "<SELECT name='weekview'>\n";
    $form .= $this->get_employees($user);
    $form .= "</SELECT>\n";
    $form .= "&nbsp;<input type='submit' name='submit_dayview' value='".text("Refresh")."'>\n";
    $form .= "</FORM>\n";

    $dummy = new weekView($day, $month, $year);
    $dummy->viewerWidth = "600";
    $dummy->owner       = $g_user["name"];
    $dummy->showGlobal  = $user;
    $dummy->generate();

    $buff  = "<br><br><table width='".$dummy->viewerWidth."'><tr>";
    $buff .= "<td valign='top' align='left'>".$dummy->weekNav()."</td>";
    $buff .= "<td valign='top' align='right'>$form</td>";
    $buff .= "</tr></table>"; 
    $buff .= $dummy->dumpMatrix();

    return $buff;
   }

   

   


  }
?>
