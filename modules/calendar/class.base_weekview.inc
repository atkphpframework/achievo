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
        $viewtime = adodb_mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
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
      atkimport("module.utils.dateutil");
      
      // crosscomparison to match all appointments in range.
      $db = &atkGetDb();

      $date = $this->year."-".$this->month."-".$this->day;

      $query = &$db->createQuery();
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
        $user = getUser();
        $query->addCondition("(schedule.owner='".(!isset($this->showGlobal)?0:$user["id"])."' OR person.userid='".$this->showGlobal."' OR schedule.all_users = 1)");
      }

      $query->addOrderBy('starttime');
      $querystring = $query->buildSelect(TRUE);
      $recs = $db->getrows($querystring);

      // now add them to a nice array
      for ($i=0,$_i=count($recs); $i<$_i; $i++)
      {
        $query->deAlias($recs[$i]);
        atkDataDecode($recs[$i]);
                
        $days = array();
        //If the startdate is before the enddate, we have multiple days. We add the appointment
        //for each day. If the startdate en enddate are the same, or startdate > enddate, we only
        //add the appointment on the startdate.
        $startdate = $this->_formatDate($recs[$i]["startdate"]);
        $enddate = $this->_formatDate($recs[$i]["enddate"]);
        
        //If start is beyond end, the difference is negative.        
        if(dateutil::date_diff($startdate,$enddate) > 0)        
        { 
          //returns dates in the format yyyymmdd, we want them to be in the format yyyy-mm-dd.         
          $daysbetween = dateutil::daysBetween($startdate,$enddate);
          
          foreach($daysbetween as $date)          
            $days[] = $this->_unformatDate($date['date']);          
        }
        else
        {
          $days[] = $recs[$i]["startdate"];
        }

        
        foreach($days as $date)
        {        
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
      }

      return $appointments;
    }
    
    /**
     * Reformat a date in the format yyyy-mm-dd to the format yyyymmdd.     
     *
     * @param date In the format yyyy-mm-dd
     * @return date in the format yyyymmdd 
     */
    function _formatDate($yyyy_mm_dd)
    {
      return str_replace("-","",$yyyy_mm_dd);
    }
    /**
     * Reformat a date in the format yyyymmdd to the format yyyy-mm-dd.
     *
     * @param date In the format yyyymmdd 
     * @return date in the format yyyy-mm-dd 
     */
    function _unformatDate($yyyymmdd)
    {
      return substr($yyyymmdd,0,4)."-".substr($yyyymmdd,4,2)."-".substr($yyyymmdd,6,2);      
    }

  /**
   * daypart
   * determine whether a time is in the morning, in the afternoon of in the evening
   * @returns morning, afternoon or evening
   */
    function daypart($time)
    {
     list($hours, $minutes, $seconds) = explode(":", $time);

     if ($hours < 12)
     {
       return "morning";
     }
     else if ($hours >= 12 && $hours <= 17)
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
    $prevTime = $this->weekdates[0]['date'];
    $prevUrl  = atkSelf()."?view=weekview&viewdate=$prevTime&user=".$this->showGlobal;

    $thisTime = date("Y-m-d");
    $thisUrl  = atkSelf()."?view=weekview&viewdate=$thisTime&user=".$this->showGlobal;

    $nextTime = $this->weekdates[8]['date'];
    $nextUrl  = atkSelf()."?view=weekview&viewdate=$nextTime&user=".$this->showGlobal;

    $date     = $this->year."-".$this->month."-".$this->day;
    $this->title = "<font size='-1'><b>".atktext("week")."&nbsp;".strftime("%V", adodb_mktime(0,0,0,substr($date,5,2), substr($date,8,2), substr($date,0,4)))."</b></font>";

    return "<font size='-1'>".href($prevUrl, atktext('previousweek'))."</font>"."&nbsp;"."<font size='-1'>".href($thisUrl, atktext('thisweek'))."</font>"."&nbsp;"."<font size='-1'>".href($nextUrl, atktext('nextweek'))."</font>";
   }

  /**
   * dumpMatrix
   * the actual displaying function
   */
    function dumpMatrix()
    {
      global $g_user;

      $matrix = &atknew("atk.utils.atktablerenderer");

      $cells[0][] = ""; // empty corner cell

      for ($i=1;$i<=7;$i++)
      {
        $text  = atktext($this->weekdates[$i]["day"])."<br>(".substr($this->weekdates[$i]["date"],8,2)."-".substr($this->weekdates[$i]["date"],5,2).")";
        $href  = atkSelf()."?view=dayview&viewdate=".$this->weekdates[$i]["date"]."&dayview=".$this->m_showGlobal;
        $cells[0][$i] = href($href, $text);
        $matrix->setColAlignment($i, TBL_CENTER);
      }

      $dayparts = array("allday","morning","afternoon","evening");

      foreach ($dayparts as $daypart_name)
      {

        // show different color when daypartname = allday.
        $row = array();
        $row[0] = atktext($daypart_name);

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
              $link = href(atkSelf()."?atkaction=edit&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
            }
            else
            {
              // this is somebody elses appointment. you may only view it if it is a public item.
              if ($dayappointments[$j]["publicitem"]==1)
              {
                $link = href(atkSelf()."?atkaction=view&atkselector=".rawurlencode("schedule.id=".$dayappointments[$j]["id"]), $dayappointments[$j]['title'], SESSION_NESTED);
              }
              else
              {
                // not a public item. Show private appointment
                $link = atktext("private_appointment");
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
          $row[$i] = $str;
        }
        $cells[] = $row;
      }

      $row[0] = ""; // empty left bottom corner cell

      for ($i=1;$i<=7;$i++)
      {
        $text  = atktext("add");
        $href  = atkSelf()."?atkaction=add&viewdate=".$this->weekdates[$i]["date"];
        $row[$i] = href($href, $text, SESSION_NESTED);
      }
      $cells[] = $row;

      return $matrix->render($cells, TBL_HEADER|TBL_FOOTER, "recordlist")."<br><br>";
    }

    function get_attendees($scheduleid)
    {
      $db = &atkGetDb();

     $query = &$db->createQuery();
     $query->addTable('schedule_attendee');
     $query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);

     $query->addField('userid', ' ', 'person');
     $query->addField('lastname', ' ', 'person');
     $query->addField('firstname', ' ', 'person');
     $query->addField('role', ' ', 'person');

     $query->addCondition("schedule_attendee.schedule_id = $scheduleid");

     $querystring = $query->buildSelect(TRUE);
     return $db->getRows($querystring);
    }

   }

?>