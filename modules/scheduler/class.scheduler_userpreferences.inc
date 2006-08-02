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

  class scheduler_userpreferences extends atkNode
  {
    function scheduler_userpreferences()
    {
      $usernode = &getNode("employee.employee");
      $user = getUser();

      $m_arr_minute_step = array();
      for($i=0;$i<60;$i+=5) $m_arr_minute_step[] = sprintf("%02d",$i);

      //$datestyles = $this->getDateStyles();
      //$timezones = $this->getTimeZones();

      $this->atkNode("scheduler_userpreferences");
      $this->add(new atkAttribute("userid",AF_PRIMARY|AF_HIDE));
      $this->add(new atkDummyAttribute("group1","<b>".atktext("settings")."</b>:<br>"));

      //$this->add(new atkListAttribute("timezone",array_values($timezones),array_keys($timezones),AF_OBLIGATORY|AF_NO_TRANSLATION));
      //$this->add(new atkListAttribute("dateformat",array_keys($datestyles), array_values($datestyles),AF_OBLIGATORY|AF_NO_TRANSLATION));
      //$this->add(new atkRadioAttribute("timeformat",array(12,24),array("12","24")));
      $this->add(new atkListAttribute("timeschedule",array(5,6,10,15,30,60),array(),AF_OBLIGATORY|AF_NO_TRANSLATION));

      //$this->add(new atkBoolAttribute("showweekendsinweekview"));
      $this->add(new atkBoolAttribute("showweeknumbers"));
      $this->add(new atkBoolAttribute("showtodo"));
      $this->add(new atkBoolAttribute("showemployeebirthdays"));
      $this->add(new atkBoolAttribute("showcustomerbirthdays"));

      $this->add(new atkBoolAttribute("autorefresh"));
      $this->add(new atkNumberAttribute("refresh_interval"));


      $this->add(new atkListAttribute("default_view",array("day","week","month","year","employee"),array(),AF_OBLIGATORY));
      $this->add(new atkListAttribute("default_userview",array($usernode->descriptor($user),"allusers"),array($user["id"],"*"),AF_OBLIGATORY));

      //$this->add(new atkListAttribute("startday",array("sunday","monday","saturday"),array(0,1,6),AF_OBLIGATORY));
      $this->add(new atkTimeAttribute("default_eventtime", 0, 23, $m_arr_minute_step,"01:00", 0));
      $this->add(new atkManyToOneRelation("default_category", "scheduler.scheduler_category",AF_OBLIGATORY|AF_RELATION_AUTOLINK));

      $this->add(new atkDummyAttribute("group2","<br><b>".$this->text("colors")."</b>:<br>"));
      $this->add(new atkColorPickerAttribute("scheduler_emptycolor"));
      $this->add(new atkColorPickerAttribute("scheduler_emptyworkhourscolor"));

      $this->add(new atkDummyAttribute("group3","<br><b>".$this->text("email")."</b>:<br>"));
      $this->add(new atkBoolAttribute("email_reminders"));
      $this->add(new atkBoolAttribute("email_addedevents"));
      $this->add(new atkBoolAttribute("email_updatedevents"));
      $this->add(new atkBoolAttribute("email_removedevents"));
      $this->add(new atkBoolAttribute("email_rejectedevents"));

      $this->add(new atkDummyAttribute("group4","<br><b>".$this->text("assistant")."</b>:<br>"));
      $this->add(new atkOneToManyRelation("assistant", "scheduler.scheduler_userassistants", "userid"));

      $this->setTable("scheduler_userpreferences");
      $this->setSecurityAlias("employee.userprefs");
    }

    function initial_values()
    {
      $user = getUser();
      return array("timeschedule"=>30,
                   "showweeknumbers"=>false,
                   "showtodo"=>false,
                   "showemployeebirthdays"=>false,
                   "showcustomerbirthdays"=>false,
                   "autorefresh"=>false,
                   "refresh_interval"=>0,
                   "default_view"=>"day",
                   "default_userview"=>$user["id"],
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


      $user = getUser();
      $userid = atkArrayNvl($user, "id");

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