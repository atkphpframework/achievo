<?php

 // calendar base class
  include_once "class.calendar.inc";
  include_once "class.base_dayview.inc";

  class schedule extends atkNode
  {
   function schedule()
   {
    global $g_sessionManager, $view, $viewdate;

    $aItemBegin = array("08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30","14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00","21:30","22:00","22:30","23:00","23:30","24:00");
    $aItemEnd   = $aItemBegin;
   
    $this->atkNode("schedule", NF_ADD_LINK); 

    $this->addAttribute(new atkAttribute("owner"            , AF_READONLY));
    $this->addAttribute(new atkAttribute("item_id"          , AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkAttribute("item_title"       , AF_SEARCHABLE|AF_OBLIGATORY));
    $this->addAttribute(new atkDateAttribute("item_date"    , "F d Y","d F Y", 0, date("Ymd")));
    $this->addAttribute(new atkListAttribute("item_begin"   , $aItemBegin, AF_OBLIGATORY));
    $this->addAttribute(new atkListAttribute("item_end"     , $aItemEnd, AF_OBLIGATORY));
    $this->addAttribute(new atkBoolAttribute("item_allday"  , $aItemEnd, AF_OBLIGATORY));
    $this->addAttribute(new atkBoolAttribute("item_public"  , $aItemEnd, AF_OBLIGATORY));
    $this->addAttribute(new atkTextAttribute(""  , $aItemEnd, AF_OBLIGATORY));

    $view = $g_sessionManager->stackVar("view");

    if (trim($view) == "") { $view = "dayview"; }

    $this->setTable("schedule", "schedule");
    $this->setOrder("item_date DESC");
   }

   function initial_values()
   {
    global $g_user;
    return array("owner"=>$g_user["name"]);
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
    global $g_layout;

    if (method_exists($this, $view))
    {
     $result = $this->$view();
    }
    else
    {
     $result = "Niet bestaande view ($view)";
    }

    $g_layout->ui_top(text("Achievo scheduler / ".$view));
    $g_layout->output(href($PHP_SELF."?view=weekview","Weekview") ."&nbsp;". href($PHP_SELF."?view=monthview","Monthview"));
    $g_layout->output($result);
    $g_layout->ui_bottom();    
   }

   function dayview()
   {
    global $g_layout, $viewdate, $g_user, $g_db, $dayview;

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
    $form  = "<FORM method='post'>\n";
    $form .= "<SELECT name='dayview'>\n";
    $form .= $this->get_employees($user);
    $form .= "</SELECT>\n";
    $form .= "<br><br><input type='submit' name='submit_dayview' value='".text("Change view")."'>\n";
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
    $buff  = "<table width='".$dummy->viewerWidth."'><tr>";
    $buff .= "<td valign='top' align='left'>$legend</td>";
    $buff .= "<td valign='top' align='right'>$form</td>";
    $buff .= "</tr></table><br>";
    $buff .= $dummy->dumpMatrix();
 
    return $buff;
   }

   function monthview()
   {
    global $g_layout, $PHP_SELF;

    // create a new instance of our calendar
     $cal    = new achievoCalendar($month, $year, $day);

    // calculate the days within this week
     $items  = $cal->monthItems();

    $buff  = "<br>".$g_layout->data_top();
    $buff .= "<tr>";
    $buff .= $g_layout->ret_td_datatitle(text("Sun","width='15%'"));
    $buff .= $g_layout->ret_td_datatitle(text("Mon","width='15%'"));
    $buff .= $g_layout->ret_td_datatitle(text("Tue","width='15%'"));
    $buff .= $g_layout->ret_td_datatitle(text("Wed","width='15%'"));
    $buff .= $g_layout->ret_td_datatitle(text("Thu","width='15%'"));
    $buff .= $g_layout->ret_td_datatitle(text("Fri","width='15%'"));
    $buff .= $g_layout->ret_td_datatitle(text("Sat","width='15%'"));
    $buff .= "</tr>";
   
    $buff .= "<tr>";
    $offs  = 0;

    for ($i=1;$i<=count($items);$i++)
    {
     if ($items[$i] != "&nbsp;")
     {
      $href = $PHP_SELF."?view=dayview&viewdate=".$cal->displayYear."-".$cal->displayMonth."-".$items[$i];
      if ($cal->displayMonth == date("m") && $cal->displayYear == date("Y") && $items[$i] == date("d"))
      {
       $buff .= "<td valign='top' align='left' bgcolor='#FF0000'>".href($href,$items[$i])."</td>";
      }
      else
      {
       $buff .= $g_layout->ret_td_datatitle(href($href,$items[$i]));
      }
     }
     else
     {
      $buff .= $g_layout->ret_td_datatitle($items[$i]);
     }
     $offs++;

     if ($offs == 7)
     {
      $buff .= "</tr>";  
      $offs = 0;
     }
    }


    $buff .= $g_layout->data_bottom();
    return $buff;
   }


   function weekview()
   {
    global $g_layout, $PHP_SELF;

    // create a new instance of our calendar
     $cal    = new achievoCalendar($month, $year, $day);

    // calculate the days within this week
     $items  = $cal->weekItems();

    // get appointitems in this week
     $filter = "item_date >= '".$items[1]['date']."' AND item_date <= '".$items[7]['date']."'";
     $app    = $this->selectDb($filter,"item_date","",$this->m_listExcludes);

    $buff  = "<br>".$g_layout->data_top();
    $buff .= "<tr>";
    for ($i=1;$i<=7;$i++)
    {
     $title = $items[$i]['day']."<br>".substr($items[$i]['date'],5);
     $buff .= $g_layout->ret_td_datatitle(href($PHP_SELF."?view=dayview&viewdate=".$items[$i]['date'],text($title)));    
    }
    $buff .= "</tr>";

    $buff .= "<tr>"; 
    for ($i=1;$i<=7;$i++)
    {
     $buff .= $g_layout->ret_td_datatitle("<br><br><br>");    
    }
    $buff .= "</tr>";

    $buff .= "<tr>"; 
    for ($i=1;$i<=7;$i++)
    {
     $buff .= $g_layout->ret_td_datatitle(href($PHP_SELF."?atkaction=add",text("add"),SESSION_NESTED));    
    }
    $buff .= "</tr>";

    $buff .= $g_layout->data_bottom();
    
    return $buff;
   }

   

   


  }
?>
