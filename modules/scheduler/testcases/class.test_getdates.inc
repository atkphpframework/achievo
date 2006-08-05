<?php

/**
   * Tests the tabs security
   *
   * ATK has now seperate security settings for different tabs within
   * a node. This testcase tests the functionality.
   *
   * @access private
   * @author harrie <harrie@ibuildings.nl>
   */
  atkimport("module.scheduler.utils.schedulertools");

  class test_getdates extends atkTestCase
  {
    function test_onceDates()
    {
      // Once item on the same day
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "starttime"=>"10:00",
                    "endtime"=>"05:00",
                    "recur"=>"once");
      $valid_dates = array('2006-10-01');
      $dates = schedulertools::getDates($item,"2006-10-01","2006-10-01");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Once item on one day");

      // Once Item on several days
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "enddate"=>array("day"=>5,"month"=>10,"year"=>2006),
                    "starttime"=>"10:00",
                    "endtime"=>"05:00",
                    "recur"=>"once");
      $valid_dates = array('2006-10-01');
      $dates = schedulertools::getDates($item,"2006-10-01","2006-10-01");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Once item on multiple days");
    }

    function test_dailyDates()
    {
      // Daily item on the same day
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                              "cyclus_enddate"=>array("day"=>5,"month"=>10,"year"=>2006),
                              "every"=>2),
              "recur"=>"daily");
      $valid_dates = array('2006-10-01','2006-10-03','2006-10-05');
      $dates = schedulertools::getDates($item,"2006-09-15","2006-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Daily item on one day - ".print_r($dates,true));

      // Daily item on the multiple day
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>2,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                              "cyclus_enddate"=>array("day"=>5,"month"=>10,"year"=>2006),
                              "every"=>2),
              "recur"=>"daily");
      $valid_dates = array('2006-10-01','2006-10-03','2006-10-05');
      $dates = schedulertools::getDates($item,"2006-09-15","2006-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Daily item on multiple days - ".print_r($dates,true));

    }

    function test_weeklyDates()
    {
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2006),
                              "every"=>1,
                              "weekday"=>array(6)),
              "recur"=>"weekly");
      $valid_dates = array('2006-10-02','2006-10-03','2006-10-09','2006-10-10');
      $dates = schedulertools::getDates($item,"2006-09-15","2006-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Weekly item on one day - ".print_r($dates,true));


      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"17:00",
              "endtime"=>"18:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>8,"year"=>2006),
                              "cyclus_enddate"=>array("day"=>31,"month"=>10,"year"=>2006),
                              "every"=>2,
                              "weekday"=>array(16)),
              "recur"=>"weekly");
      $valid_dates = array('2006-08-03','2006-08-17','2006-08-31','2006-09-14','2006-09-28','2006-10-12','2006-10-26');
      $dates = schedulertools::getDates($item,"2006-07-15","2006-11-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Weekly item on one day - ".print_r($dates,true));


    }

    function test_monthlyDateDates()
    {
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>5,"year"=>2006),
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2006),
                              "every"=>2,
                              "startday"=>1,
                              "endday"=>4),
              "recur"=>"monthly_date");
      $valid_dates = array('2006-05-01','2006-05-02','2006-05-03','2006-05-04','2006-07-01','2006-07-02','2006-07-03','2006-07-04','2006-09-01','2006-09-02','2006-09-03','2006-09-04');
      $dates = schedulertools::getDates($item,"2006-04-15","2006-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Monthly date dates item on one day - ".print_r($dates,true));
    }

    function test_monthlyDayDates()
    {
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>5,"year"=>2006),
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2006),
                              "month_time"=>2,
                              "weekday"=>array(6)),
              "recur"=>"monthly_day");
      $valid_dates = array('2006-10-09','2006-10-10');
      $dates = schedulertools::getDates($item,"2006-09-15","2006-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Monthly day item on one day - ".print_r($dates,true));
    }

    function test_yearlyDateDates()
    {
     $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>5,"year"=>2004),
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2008),
                              "startday"=>2,
                              "endday"=>4,
                              "startmonth"=>1,
                              "endmonth"=>2,
                              "every"=>1),
              "recur"=>"yearly_date");
      $valid_dates = array('2005-01-02','2005-01-03','2005-01-04','2005-02-02','2005-02-03','2005-02-04','2006-01-02',
                           '2006-01-03','2006-01-04','2006-02-02','2006-02-03','2006-02-04','2007-01-02','2007-01-03',
                           '2007-01-04','2007-02-02','2007-02-03','2007-02-04','2008-01-02','2008-01-03','2008-01-04',
                           '2008-02-02','2008-02-03','2008-02-04');
      $dates = schedulertools::getDates($item,"2004-01-15","2016-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Yearly date item on one day - ".print_r($dates,true));
    }

    function test_yearlyDayDates()
    {
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_startdate"=>array("day"=>1,"month"=>5,"year"=>2001),
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2008),
                              "month_time"=>1,
                              "weekday"=>array(8),
                              "startmonth"=>1),
              "recur"=>"yearly_day");

      $valid_dates = array('2002-01-02','2003-01-01','2004-01-07','2005-01-05','2006-01-04','2007-01-03','2008-01-02');
      $dates = schedulertools::getDates($item,"2001-01-15","2016-10-20");
      $ret = atkArrayCompare($dates,$valid_dates);
      $this->assertEqual(false, $ret, "Yearly day item on one day - ".print_r($dates,true));
    }


  }

?>