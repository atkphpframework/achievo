<?php

 // calendar base class
  include_once "class.calendar.inc";
  include_once "class.base_dayview.inc";
  include_once "class.base_monthview.inc";
  
  useattrib("atkdateattribute");
  useattrib("atktextattribute");
  useattrib("atkboolattribute");
  useattrib("atktimeattribute");
  userelation("atkmanytoonerelation");

  class schedule extends atkNode
  {
    function schedule()
    {
      global $g_user, $g_sessionManager, $view, $viewdate, $user, $time;
    
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

      $this->atkNode("schedule", NF_ADD_LINK); 

      $this->add(new atkAttribute("id"              , AF_AUTOKEY));
      $this->add(new atkAttribute("owner"           , AF_READONLY|AF_HIDE));      
      $this->add(new atkDateAttribute("startdate"  , "F d Y","d F Y", 0, 0 ,AF_OBLIGATORY));
      $this->add(new atkDateAttribute("enddate"    , "F d Y","d F Y", 0, 0 ,AF_OBLIGATORY));
      $this->add(new atkTimeAttribute("starttime"  , 8, 24, array("00","30"), $default_start_time, 0));
      $this->add(new atkTimeAttribute("endtime"    , 8, 24, array("00","30"), $default_end_time, 0));
      $this->add(new atkBoolAttribute("allday"));
      $this->add(new atkAttribute("title"          , AF_SEARCHABLE|AF_OBLIGATORY));    
      $this->add(new atkTextAttribute("description"));
      $this->add(new atkAttribute("location"        , AF_SEARCHABLE));        
      $this->add(new atkBoolAttribute("publicitem"));    
      $this->add(new atkManyToOneRelation("scheduletype", "calendar.schedule_types",AF_OBLIGATORY|AF_RELATION_AUTOLINK));

      $view     = $g_sessionManager->stackVar("view");
      $viewdate = $g_sessionManager->stackVar("viewdate");
      $user     = $g_sessionManager->stackVar("user");
      if ($viewdate=="") $viewdate = strftime("%Y-%m-%d");  
      if (trim($view) == "") { $view = "dayview"; }
      if ($user=="") $user = $g_user["name"];
      
      $this->addAllowedAction("view");

      $this->setTable("schedule", "schedule");
      $this->setOrder("startdate DESC");
    }

 
    function initial_values()
    {
      global $g_user, $viewdate;
    
      $initial["owner"] = $g_user["name"];
      $initial["publicitem"] = 1;
    
      if ($viewdate!="")
      {
        $arr_date = array("year"=>substr($viewdate, 0, 4),
                          "month"=>substr($viewdate, 5, 2),
                          "day"=>substr($viewdate, 8, 2));
        $initial["startdate"] = $arr_date;
        $initial["enddate"] = $arr_date;
      }
      return $initial;
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
      global $view, $g_layout, $viewdate;

      if (method_exists($this, $view))
      {
        $result = $this->$view($viewdate);
      }
      else
      {
        $result = "Niet bestaande view ($view)";
      }
      
      $g_layout->ui_top(text("Achievo scheduler / ".$view));
      $g_layout->output($this->statusbar());    
      $g_layout->output($result);
      $g_layout->ui_bottom();    
    }

    function monthview($viewdate)
    {
      global $g_layout, $g_user, $g_db, $dayview, $PHP_SELF;

      // viewdate is something like 2001-07-23
      $month = substr($viewdate, 5,2);
      $year  = substr($viewdate, 0,4);

      $dummy = new monthView($month, $year);
      $dummy->viewerWidth = "600";
      $dummy->owner       = $g_user["name"];
      $dummy->showGlobal  = $user;

      $buff  = "<br><br>".$dummy->monthNav();
      $buff .= "<br><br><b>".$dummy->title."</b>";
      $buff .= $dummy->dumpMatrix();
     
      return $buff;
    }


    function dayview($viewdate)
    {
      global $g_layout, $g_user, $g_db, $dayview, $PHP_SELF;

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
      $form .= "<SELECT name='user'>\n";
      $form .= $this->get_employees($user);
      $form .= "</SELECT>\n";
      $form .= "&nbsp;<input type='submit' value='".text("Refresh")."'>\n";
      $form .= "</FORM>\n";

      // get the rendered matrix
      $dummy = new dayView($day, $month, $year);
      $dummy->viewerWidth = "600";
      $dummy->owner       = $g_user["name"];
      $dummy->showGlobal  = $user;
      $dummy->generate();

      // get the legend
      $legend .= $dummy->legend;
     
      $buff = href($PHP_SELF."?view=weekview&viewdate=$viewdate","Weekview") ."&nbsp;". href($PHP_SELF."?view=monthview&viewdate=$viewdate","Monthview");
 
      // assign our output buffer
      $buff .= "<br><br><table width='".$dummy->viewerWidth."'><tr>";
      $buff .= "<td valign='top' align='left'>".$dummy->dayNav()."</td>";
      $buff .= "<td valign='top' align='right'>$form</td>";
      $buff .= "</tr></table>";
      $buff .= $dummy->dumpMatrix();
      $buff .= $dummy->legend."<br>";
 
      return $buff;
    }

    function weekview($viewdate)
    {
      global $g_layout, $g_user, $g_db, $PHP_SELF, $user;       
    
      if ($viewdate!="")
      {
        $viewtime = mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
      }
      else
      {
        $viewdate = date("Y-m-d");
        $viewtime = time();
      }
      $weekday  = strftime("%w",$viewtime);
  
      if ($weekday==0) $weekday=7;

      for ($i=0; $i<=8;$i++)
      {
        $weekdates[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
        $weekdates[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
        $weekdates[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
      }
    
      $appointments = $this->getAppointments($user, $weekdates[1]['date'], $weekdates[7]['date']);    
    
      $nav  = '<FORM method="post" action="'.$PHP_SELF.'">';
      $nav .= session_form();    
      $nav .= '<br>'.href('dispatch.php?view=weekview&viewdate='.$weekdates[0]['date'], text('houradmin_previousweek'));
      $nav .= '&nbsp;&nbsp;'.href('dispatch.php?view=weekview&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));            
      $nav .= '&nbsp;&nbsp;<input type="hidden" name="viewdate" value="'.$viewdate.'">';
      $nav .= text('user').': <SELECT name="user">';
      $nav .= $this->get_employees($user);
      $nav .= "</SELECT>\n";
      $nav .= '&nbsp;<input type="submit" value="'.text("refresh").'">';
      $nav .= "</FORM>\n";    
      $nav.='<br><br>';
    
      $buff .= $nav;

      $buff .= $g_layout->data_top();

      $buff .= $g_layout->tr_top();
      $buff .= $g_layout->ret_td_datatitle();

      for ($i=1;$i<=7;$i++)
      {
        $text  = text($weekdates[$i]["day"])."<br>(".substr($weekdates[$i]["date"],8,2)."-".substr($weekdates[$i]["date"],5,2).")";
        $href  = $PHP_SELF."?view=dayview&viewdate=".$weekdates[$i]["date"]."&dayview=".$this->m_showGlobal;
        $buff .= $g_layout->ret_td_datatitle(href($href, $text), 'align="center"');
      }
      $buff .= $g_layout->tr_bottom();
    
      $dayparts = array("allday","morning","afternoon","evening");
    
      foreach ($dayparts as $daypart_name)
      {              
        
        // show different color when daypartname = allday.
        $buff .= $g_layout->tr_top($daypart_name=="allday");                 
        $buff .= $g_layout->ret_td(text($daypart_name));   
 
        for ($i=1;$i<=7;$i++)
        {
          $dayappointments = $appointments[$weekdates[$i]["date"]][$daypart_name];
          $str="";
          for ($j=0;$j<count($dayappointments);$j++)          
          {
            if ($g_user["name"]==$dayappointments[$j]["owner"])
            {
              // This is your own record, you may edit it.              
              $link = href($PHP_SELF."?atkaction=edit&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
            }
            else
            { 
              // this is somebody elses appointment. you may only view it if it is a public item.
              if ($dayappointments[$j]["publicitem"]==1)
              {                
                $link = href($PHP_SELF."?atkaction=view&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
              }
              else
              {
                // not a public item. Show private appointment
                $link = text("private_appointment");
              }
            }
            
            if ($daypart_name!="allday")
            {
              $str = "[".$dayappointments[$j]["starttime"]["hours"].":".$dayappointments[$j]["starttime"]["minutes"];
              $str.= "-".$dayappointments[$j]["endtime"]["hours"].":".$dayappointments[$j]["endtime"]["minutes"];
              $str.="] ";
            }
            $str.= $link;
            if ($user=="all") $str.=" (".$dayappointments[$j]["owner"].")";
            
            if ($j!=count($dayappointments)-1) $str.="<br>";
          }          
          $buff .= $g_layout->ret_td($str);
        }
        $buff .= $g_layout->tr_bottom();
      }
    
      $buff .= $g_layout->tr_top();
      $buff .= $g_layout->ret_td_datatitle();
      for ($i=1;$i<=7;$i++)
      {
        $text  = text("add");
        $href  = $PHP_SELF."?atkaction=add&viewdate=".$weekdates[$i]["date"];
        $buff .= $g_layout->ret_td_datatitle(href($href, $text, SESSION_NESTED), 'align="center"');
      }
      $buff .= $g_layout->tr_bottom();
 
      $buff .= $g_layout->data_bottom();
      $buff .= "<br><br>";

      return $buff;
    }
  
    function getAppointments($user, $begindate, $enddate)
    {
      // crosscomparison to match all appointments in range.
      $filter = "startdate<='$enddate' AND enddate>='$begindate'";
      if ($user!="all")
      {
        $filter.= " AND owner='$user'";
      }
      $recs = $this->selectDb($filter, "", "", "", "", "admin");
      
      // now add them to a nice array
      for ($i=0;$i<count($recs);$i++)
      {
        $date = sprintf("%d-%02d-%02d",$recs[$i]["startdate"]["year"],$recs[$i]["startdate"]["month"],$recs[$i]["startdate"]["day"]);

        if ($recs[$i]["allday"] != 1)
        {
          $daypart = $this->daypart($recs[$i]["starttime"]);
          $appointments[$date][$daypart][] = $recs[$i];
        }
        else
        {
          $appointments[$date]["allday"][] = $recs[$i];
        }
      }

      return $appointments;
   }   
   
   function daypart($time)
   {   
     $hour = (int)$time["hours"];
     if ($hour < 12)
     {
       return "morning";
     }
     else if ($hour >= 12 && $hour < 17) 
     { 
       return "afternoon"; 
     } 
     else
     {
       return "evening";
     }
   }   

  }
  
?>