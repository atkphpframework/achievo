<?php
  // class to generate a weekview
   //
   // typical usage:
   //
   // $dummy = new weekview(25,7,2001);     // show July 25th 2001
   // $dummy->showGlobal  = "wim";          // for user wim
   // $dummy->owner = $g_user["name"];     // remember who is currently logged in
   // $dummy->generate();                  // generate the matrix
   // $dummy->dumpMatrix();                // show it to the audience ....
   //
   //

   class weekView
   {
    var $day          = 0;
    var $month        = 0;
    var $year         = 0;
    var $appointments = array();
    var $weekdates    = array();
    var $title        = "";
    var $showGlobal;
    var $owner;
    var $viewerWidth  = "100%";
    var $title        = "";

  /**
   * constructor
   * @param $day   (if 0, then current day)
   * @param $month (if 0, then current month)
   * @param $year  (if 0, then current year)
   */
    function weekView($day=0, $month=0, $year=0)
    {
      $this->year   = ($year  == 0) ? date("Y") : $year;
      $this->month  = ($month == 0) ? date("m") : $month;
      $this->day    = ($day   == 0) ? date("d") : $day;
    }

   /**
   * wrapper around the gathering functions
   * fills several info arrays
   */
    function generate()
    {
     $viewdate = $this->year."-".$this->month."-".$this->day;
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
        $this->weekdates[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
        $this->weekdates[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
        $this->weekdates[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
      }

      $this->appointments = $this->getAppointments($this->showGlobal, $this->weekdates[1]['date'], $this->weekdates[7]['date']);
    }

  /**
   * get the items for the specified week
   * fills an array with the items
   */
    function getAppointments($user, $begindate, $enddate)
    {
      // crosscomparison to match all appointments in range.
      global $g_db;

      $date = $this->year."-".$this->month."-".$this->day;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();
      $query->addTable('schedule');
      $query->addJoin('schedule_attendee', '', 'schedule_attendee.schedule_id=schedule.id', TRUE);
      $query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);
      $query->addJoin('person', 'owner', 'owner.id=schedule.owner', TRUE);

      $query->addField('id', ' ', 'schedule');
      $query->addField('startdate', ' ', 'schedule');
      $query->addField('enddate', ' ', 'schedule');
      $query->addField('starttime', ' ', 'schedule');
      $query->addField('endtime', ' ', 'schedule');
      $query->addField('title', ' ', 'schedule');
      $query->addField('description', ' ', 'schedule');
      $query->addField('location', ' ', 'schedule');
      $query->addField('allday', ' ', 'schedule');
      $query->addField('publicitem', ' ', 'schedule');
      $query->addField('owner', ' ', 'schedule');
      $query->addField('userid', '', 'owner', 'owneruser_AE_');
      $query->addField('status', ' ', 'schedule');
      $query->addField('scheduletype', ' ', 'schedule');
      $query->addField('all_users', ' ', 'schedule');

      $query->addCondition("schedule.startdate <='$enddate' AND schedule.enddate >= '$begindate'");

      if ($this->showGlobal != "all")
      {
        $query->addCondition("(schedule.owner='".$this->showGlobal."' OR person.userid='".$this->showGlobal."' OR schedule.all_users = 1)");
      }

      $query->addOrderBy('starttime');
      $querystring = $query->buildSelect(TRUE);
      $recs = $g_db->getrows($querystring);                    

      // now add them to a nice array
      for ($i=0;$i<count($recs);$i++)
      {
        $query->deAlias($recs[$i]);         
        atkDataDecode($recs[$i]);        
      
        $year = substr($recs[$i]["startdate"],0,4);
        $month = substr($recs[$i]["startdate"],5,2);
        $day = substr($recs[$i]["startdate"],8,2);
        $date = $year."-".$month."-".$day;

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

  /**
   * daypart
   * determine whether a time is in the morning, in the afternoon of in the evening
   * @returns morning, afternoon or evening
   */
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

  /**
   * weekNav
   * navigate through weeks using prev/next week link
   * @returns html code containing links for previous week and next week
   */
   function weekNav()
   {
    global $PHP_SELF;

    $prevTime = $this->weekdates[0]['date'];
    $prevUrl  = $PHP_SELF."?view=weekview&viewdate=$prevTime&user=".$this->showGlobal;

    $nextTime = $this->weekdates[8]['date'];
    $nextUrl  = $PHP_SELF."?view=weekview&viewdate=$nextTime&user=".$this->showGlobal;

    $date     = $this->year."-".$this->month."-".$this->day;
    $this->title = "<font size='-1'><b>".text("week")."&nbsp;".strftime("%V", mktime(0,0,0,substr($date,5,2), substr($date,8,2), substr($date,0,4)))."</b></font>";

    return "<font size='-1'>".href($prevUrl, text('previousweek'))."</font>"."&nbsp;"."<font size='-1'>".href($nextUrl, text('nextweek'))."</font>";
   }

  /**
   * dumpMatrix
   * the actual displaying function
   */
    function dumpMatrix()
    {
     global $g_layout, $g_user;

      $buff   = $g_layout->initGUI();
      $buff .= $g_layout->data_top();

      $buff .= $g_layout->tr_top();
      $buff .= $g_layout->ret_td_datatitle();

      for ($i=1;$i<=7;$i++)
      {
        $text  = text($this->weekdates[$i]["day"])."<br>(".substr($this->weekdates[$i]["date"],8,2)."-".substr($this->weekdates[$i]["date"],5,2).")";
        $href  = $_SERVER["PHP_SELF"]."?view=dayview&viewdate=".$this->weekdates[$i]["date"]."&dayview=".$this->m_showGlobal;
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
          $dayappointments = $this->appointments[$this->weekdates[$i]["date"]][$daypart_name];
          $str="";
          for ($j=0;$j<count($dayappointments);$j++)
          {
            $attendees = $this->get_attendees($dayappointments[$j]["id"]);
            $found = 0;
            for($k=0;$k<count($attendees);$k++)
            {
              if ($attendees[$k]["userid"] == $g_user["id"]) $found = 1;
            }
            if ($g_user["id"]==$dayappointments[$j]["owner"] || (is_allowed("calendar.schedule", "edit_attendee") && $found == 1))
            {
              // This is your own record, you may edit it.
              $link = href($_SERVER["PHP_SELF"]."?atkaction=edit&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
            }
            else
            {
              // this is somebody elses appointment. you may only view it if it is a public item.
              if ($dayappointments[$j]["publicitem"]==1)
              {
                $link = href($_SERVER["PHP_SELF"]."?atkaction=view&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
              }
              else
              {
                // not a public item. Show private appointment
                $link = text("private_appointment");
              }
            }

            if ($daypart_name!="allday")
            {
              $str.= "[".substr($dayappointments[$j]["starttime"],0,2).":".substr($dayappointments[$j]["starttime"],3,2);
              $str.= "-".substr($dayappointments[$j]["endtime"],0,2).":".substr($dayappointments[$j]["endtime"],3,2);
              $str.="] ";
            }
            $str.= $link;
            if ($this->showGlobal!=$dayappointments[$j]["owner"]) $str.=" (".$dayappointments[$j]["owneruser"]["userid"].")";

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
        $href  = $_SERVER["PHP_SELF"]."?atkaction=add&viewdate=".$this->weekdates[$i]["date"];
        $buff .= $g_layout->ret_td_datatitle(href($href, $text, SESSION_NESTED), 'align="center"');
      }
      $buff .= $g_layout->tr_bottom();

      $buff .= $g_layout->data_bottom();
      $buff .= "<br><br>";

      return $buff;
    }

    function get_attendees($scheduleid)
    {
      global $g_db;

     $name = "atk".atkconfig("database")."query";
     $query = new $name();
     $query->addTable('schedule_attendee');
     $query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);

     $query->addField('userid', ' ', 'person');
     $query->addField('lastname', ' ', 'person');
     $query->addField('firstname', ' ', 'person');
     $query->addField('role', ' ', 'person');

     $query->addCondition("schedule_attendee.schedule_id = $scheduleid");

     $querystring = $query->buildSelect(TRUE);
     return $g_db->getRows($querystring);
    }

   }

?>
