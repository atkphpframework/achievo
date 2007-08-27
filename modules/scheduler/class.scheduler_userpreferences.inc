<?php

// TODO: Timezone

  useattrib("atklistattribute");
  useattrib("atktimeattribute");
  useattrib("atkflagattribute");
  useattrib("atkboolattribute");
  useattrib("atknumberattribute");
  useattrib("atkradioattribute");
  useattrib("atkdummyattribute");
  useattrib("atkcolorpickerattribute");
  userelation("atkonetomanyrelation");
  userelation("atkshuttlerelation");
  userelation("atkmanytomanyrelation");
  userelation("atkmanyboolrelation");
  userelation("atkshuttlerelation");

  class scheduler_userpreferences extends atkNode
  {
    function scheduler_userpreferences()
    {
      // Call the parent constructor
      $this->atkNode("scheduler_userpreferences");

      $userviews = $this->getUserViews();
      // Get the possible minute steps
      $m_arr_minute_step = array();
      for($i=0;$i<60;$i+=5) $m_arr_minute_step[] = sprintf("%02d",$i);

      //$datestyles = $this->getDateStyles();
      //$timezones = $this->getTimeZones();

      $this->add(new atkAttribute("userid",AF_PRIMARY|AF_HIDE));
      $this->add(new atkDummyAttribute("group1","<b>".atktext("settings")."</b>:<br>"),"scheduler");
      //$this->add(new atkListAttribute("timezone",array_values($timezones),array_keys($timezones),AF_OBLIGATORY|AF_NO_TRANSLATION));
      //$this->add(new atkListAttribute("dateformat",array_keys($datestyles), array_values($datestyles),AF_OBLIGATORY|AF_NO_TRANSLATION));
      //$this->add(new atkRadioAttribute("timeformat",array(12,24),array("12","24")));
      $this->add(new atkListAttribute("timeschedule",array(5,6,10,15,30,60),array(),AF_OBLIGATORY|AF_NO_TRANSLATION),"scheduler");
      //$this->add(new atkBoolAttribute("showweekendsinweekview"));
      $this->add(new atkBoolAttribute("showweeknumbers"),"scheduler");
      $this->add(new atkBoolAttribute("showtodo"),"scheduler");
      $this->add(new atkBoolAttribute("showemployeebirthdays"),"scheduler");
      $this->add(new atkBoolAttribute("showlunarphases"),"scheduler");
      //$this->add(new atkBoolAttribute("showcustomerbirthdays"));
      $this->add(new atkBoolAttribute("autorefresh"),"scheduler");
      $this->add(new atkNumberAttribute("refresh_interval"),"scheduler");
      $this->add(new atkListAttribute("default_view",array("day","week","month"),array(),AF_OBLIGATORY),"scheduler");
      $this->add(new atkListAttribute("default_userview",array_values($userviews),array_keys($userviews),AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM),"scheduler");
      $this->add(new atkTimeAttribute("default_eventtime", 0, 23, $m_arr_minute_step,"01:00", 0),"scheduler");
      $this->add(new atkManyToOneRelation("default_category", "scheduler.scheduler_category",AF_RELATION_AUTOLINK),"scheduler");
      $this->add(new atkDummyAttribute("group2","<br><b>".$this->text("colors")."</b>:<br>"),"scheduler");
      $this->add(new atkColorPickerAttribute("scheduler_emptycolor"),"scheduler");
      $this->add(new atkColorPickerAttribute("scheduler_emptyworkhourscolor"),"scheduler");
      /*
      $this->add(new atkDummyAttribute("group3","<br><b>".$this->text("email")."</b>:<br>"));
      $this->add(new atkBoolAttribute("email_reminders"));
      $this->add(new atkBoolAttribute("email_addedevents"));
      $this->add(new atkBoolAttribute("email_updatedevents"));
      $this->add(new atkBoolAttribute("email_removedevents"));
      $this->add(new atkBoolAttribute("email_rejectedevents"));
      */
      $this->add(new atkDummyAttribute("group4","<br><b>".$this->text("assistant")."</b>:<br>"),"scheduler");
      $shuttle_rel = &$this->add(new atkShuttleRelation("assistant","scheduler.scheduler_userassistants","employee.employee",AF_NO_LABEL),"scheduler");
      $shuttle_rel->setLocalKey("userid");
      $shuttle_rel->setRemoteKey("employeeid");
      $shuttle_rel->setDestinationFilter("person.status='active'");

      $this->add(new atkDummyAttribute("group5","<br><b>".$this->text("groups")."</b>:<br>"),"scheduler");
      $this->add(new atkOneToManyRelation("groups","scheduler.scheduler_group","userid"),"scheduler");



      $this->setTable("scheduler_userpreferences");
      $this->setSecurityAlias("employee.userprefs");
    }

    function getUserViews()
    {
      $users = array();

      // Get current user information
      $user = getUser();
      $userid = atkArrayNvl($user, "id");

      // If logged in as non-administrator
      if (!empty($userid))
      {
        // Add current user
        $usernode = &getNode("employee.employee");
        $userdescriptor = $usernode->descriptor($user);
        $users[$userid] = $userdescriptor;

        // Add groups
        $groupnode = &getNode("scheduler.scheduler_group");
        $recs = $groupnode->selectDb("`scheduler_group`.`userid`='$userid'");
        if(count($recs)>0)
        {
          foreach($recs as $rec)
          {
            $users["g_".$rec["id"]] = $rec["name"];
          }
        }
      }

      // Add all users
      $users["*"] = "allusers";

      return $users;
    }

    function refresh_interval_display($record="", $mode="")
    {
      $refrintattr = &$this->getAttribute("refresh_interval");
      $result = $refrintattr->display($record, $mode) . ' ' . $this->text("seconds");
      return $result;
    }

    function refresh_interval_edit($record="", $fieldprefix="")
    {
      $refrintattr = &$this->getAttribute("refresh_interval");
      $result = $refrintattr->edit($record, $fieldprefix) . ' ' . $this->text("seconds");
      return $result;
    }


    function initial_values()
    {
      $userid = atkArrayNvl(getUser(), "id");
      return array("timeschedule"=>30,
                   "showweeknumbers"=>false,
                   "showtodo"=>false,
                   "showemployeebirthdays"=>false,
                   "showcustomerbirthdays"=>false,
                   "autorefresh"=>false,
                   "refresh_interval"=>0,
                   "default_view"=>"day",
                   "default_userview"=>$userid,
                   "default_eventtime"=>array('hours'=>'01','minutes'=>'00','seconds'=>'00'),
                   "scheduler_emptycolor"=>"#DDDDDD",
                   "scheduler_emptyworkhourscolor"=>"#FFFFFF",
                   "email_reminders"=>false,
                   "email_addedevents"=>false,
                   "email_updatedevents"=>false,
                   "email_removedevents"=>false,
                   "email_rejectedevents"=>false);
    }

    function getDateStyles()
    {
      $time = time();
      $datestyles = array (
        "F d, Y"=> atktext(strtolower(date("F",$time))) . date(" d, Y",$time),
        "d F, Y"=> date("d ",$time) . atktext(strtolower(date("F",$time))) . date(", Y",$time),
        "d-F-Y"=> date("d-",$time) . atktext(strtolower(date("F",$time))) . date("-Y",$time),
        "d-F-y"=> date("d-",$time) . atktext(strtolower(date("F",$time))) . date("-y",$time),
        "m/d/Y"=> date("m/d/Y",$time),
        "m/d/y"=> date("m/d/y",$time),
        "m-d-Y"=> date("m-d-Y",$time),
        "m-d-y"=> date("m-d-y",$time),
        "Y-m-d"=> date("Y-m-d",$time),
        "y-m-d"=> date("y-m-d",$time),
        "Y/m/d"=> date("Y/m/d",$time),
        "y/m/d"=> date("y/m/d",$time),
        "d/m/Y"=> date("d/m/Y",$time),
        "d/m/y"=> date("d/m/y",$time),
        "d-m-Y"=> date("d-m-Y",$time),
        "d-m-y"=> date("d-m-y",$time)
      );
      return $datestyles;
    }

    function getTimeZones()
    {
      /** @todo: Timezones?!? */
      /*
      global $_DATE_TIMEZONE_DATA;
      $timezone_options = array();
      $timezone_values = array();
      $add_text = atktext("add N hours to server time");
      $sub_text = atktext("substract N hours from server time");

      for ( $i = -12; $i <= 12; $i++ )
      {
        $timezone_values[] = $i;
        if ( $i < 0 )
          $timezone_options[] = str_replace ( "N", -$i, $sub_text)." (".date("H:i",$time+($i*3600)).")";
        else if ( $i == 0 )
          $timezone_options[] = atktext("same as server time (".date("H:i",$time).")");
        else
          $timezone_options[] = str_replace ( "N", $i, $add_text )." (".date("H:i",$time+($i*3600)).")";
      }
      for($i=-12;$i<=14;$i++)
      {
        $timezone_options[]= "GMT ".($i>0?"+$i":$i);
        $timezone_values[] = $i;

      }
      */
      $timezones = array();
      for($i=-12;$i<=14;$i++)
      {
        $timezones[$i]= "GMT ".($i>0?"+$i":$i);
      }
      return $timezones;
    }

    function getUserPreferences()
    {
      static $userPreferences = array();
      static $users = array();

      $userid = atkArrayNvl(getUser(), "id");

      if(!in_array($userid,$users))
      {
        $users[] = $userid;
        $rec = (!empty($userid)) ? $this->selectDb("userid = '$userid'") : null;
        if(is_array($rec) && count($rec)>0)
        {
          $userPreferences[$userid] = $rec[0];
          return $rec[0];
        }
        else
        {
          $userPreferences[$userid] = $this->initial_values();
          return $this->initial_values();
        }
      }
      else
      {
        return $userPreferences[$userid];
      }
    }
  }

?>