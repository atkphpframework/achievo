<?php

 // calendar base class
  include_once "class.calendar.inc";

  class schedule extends atkNode
  {
   function schedule()
   {
    global $g_sessionManager, $view;
   
    $this->atkNode("schedule", NF_EDITAFTERADD); 

    $this->addAttribute(new atkAttribute("item_id"      , AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkAttribute("item_title"   , AF_SEARCHABLE|AF_OBLIGATORY));
    $this->addAttribute(new atkAttribute("item_date"    , AF_SEARCHABLE|AF_OBLIGATORY));

    $view = $g_sessionManager->stackVar("view");

    if (trim($view) == "") { $view = "weekview"; }

    $this->setTable("schedule", "schedule");
    $this->setOrder("item_date DESC");
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

    $g_layout->ui_top("Kalender / ".$view);
    $g_layout->output( href($PHP_SELF."?view=weekview","Weekview") ."&nbsp;". href($PHP_SELF."?view=monthview","Monthview")."<br>"    );
    $g_layout->output($result);
    $g_layout->ui_bottom();    
   }

   function dayview()
   {
    $buff  = "<br>".$g_layout->data_top();
    $buff .= "<tr>";
    $buff .= $g_layout->ret_td_datatitle(text("Sun","width='15%'"));
    $buff .= "</tr>";
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
