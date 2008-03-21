<?php

  /**
   * Tests the scheduler tools
   *
   * @access private
   * @author sandy <sandy@achievo.org>
   */
  atkimport("module.scheduler.utils.schedulertools");

  class test_schedulertools extends atkTestCase
  {
    function test_weekdaynumbertoflag()
    {
      $this->assertFalse(schedulertools::weekdaynumbertoflag(10),'Weekday not between 0 and 6');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(0),1,'Weekday Sunday');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(1),2,'Weekday Monday');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(2),4,'Weekday Tuesday');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(3),8,'Weekday Wednesdag');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(4),16,'Weekday Thursday');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(5),32,'Weekday Friday');
      $this->assertEqual(schedulertools::weekdaynumbertoflag(6),64,'Weekday Saturday');
    }

    function test_isLeapYear()
    {
      $this->assertTrue(schedulertools::isLeapYear(2000),' 2000 is a leapyear');
      $this->assertTrue(schedulertools::isLeapYear(2400),' 2400 is a leapyear');
      $this->assertFalse(schedulertools::isLeapYear(2100),'2100 is not a leapyear');
      $this->assertFalse(schedulertools::isLeapYear(2300),'2300 is not a leapyear');
    }

    function test_dayOfWeek()
    {
      $this->assertEqual(schedulertools::dayOfWeek(2,12,2007),1,'Sunday - '.schedulertools::dayOfWeek(2,12,2007));
      $this->assertEqual(schedulertools::dayOfWeek(3,12,2007),2,'Monday - '.schedulertools::dayOfWeek(3,12,2007));
      $this->assertEqual(schedulertools::dayOfWeek(4,12,2007),3,'Tuesday - '.schedulertools::dayOfWeek(4,12,2007));
      $this->assertEqual(schedulertools::dayOfWeek(5,12,2007),4,'Wednesday - '.schedulertools::dayOfWeek(5,12,2007));
      $this->assertEqual(schedulertools::dayOfWeek(6,12,2007),5,'Thursday - '.schedulertools::dayOfWeek(6,12,2007));
      $this->assertEqual(schedulertools::dayOfWeek(7,12,2007),6,'Friday - '.schedulertools::dayOfWeek(7,12,2007));
      $this->assertEqual(schedulertools::dayOfWeek(8,12,2007),7,'Saturday - '.schedulertools::dayOfWeek(8,12,2007));
    }

    function test_nthday()
    {
       $this->assertEqual(schedulertools::nthDay(1,1,4,1997),6,'Daylight Savings Time begins - Sunday 6th April 1997 - '.schedulertools::nthDay(1,1,4,1997));
       $this->assertEqual(schedulertools::nthDay(2,1,5,1997),11,'Mother\'s Day - Sunday 11th May 1997 - '.schedulertools::nthDay(2,1,5,1997));
       $this->assertEqual(schedulertools::nthDay(3,7,5,1997),17,'Armed Forces Day - Saturday 17th May 1997 - '.schedulertools::nthDay(3,7,5,1997));
       $this->assertEqual(schedulertools::nthDay(4,5,11,1997),27,'Thanksgiving Day - Thursday 27th November 1997 - '.schedulertools::nthDay(4,5,11,1997));
       $this->assertEqual(schedulertools::nthDay(5,4,12,1997),31,'5th Wednesday in December 1997 - Wednesday 31st December 1997 - '.schedulertools::nthDay(5,4,12,1997));
       $this->assertEqual(schedulertools::nthDay(-1,1,10,1997),26,'Daylight Savings Time ends - Sunday 26th October 1997 - '.schedulertools::nthDay(-1,1,10,1997));
    }


    function test_getItemStartDate()
    {
      $rec = array('startdate'=>array('day'=>1,'month'=>1,'year'=>2008));
      $date = schedulertools::getItemStartDate($rec);
      $this->assertEqual(1199185200, $date, "Item startdate is 2008-01-01 - $date");
    }

    function test_getItemEndDate()
    {
      $rec = array('recur'=>'once',
                   'enddate'=>array('day'=>1,'month'=>1,'year'=>2009),
                   'cyclus'=>array('cyclus_enddate'=>array('day'=>1,'month'=>1,'year'=>2008),
                                   'end_choice'=>1));
      $date = schedulertools::getItemEndDate($rec);
      $this->assertEqual(1230807600, $date, "Item enddate is 2009-01-01");
      $rec['recur']='monthly';
      $date = schedulertools::getItemEndDate($rec);
      $this->assertEqual(2145870000, $date, "Item has no enddate");
      $rec['cyclus']['end_choice']=2;
      $date = schedulertools::getItemEndDate($rec);
      $this->assertEqual(1199185200, $date, "Item enddate is 2008-01-01");
      $rec['cyclus']['end_choice']=3;
      $date = schedulertools::getItemEndDate($rec);
      $this->assertNull($date, "Item enddate ends after x times");
    }

    function test_showItemOnce()
    {
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "starttime"=>"10:00",
                    "endtime"=>"05:00",
                    "recur"=>"once");
      $startdate = schedulertools::getItemStartDate($item);
      $enddate = schedulertools::getItemEndDate($item);
      $checkdate = $startdate;
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show once item on 1 oct 2006');
      $checkdate = time();
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show once item on 1 oct 2006');
    }

    /**
     *  0 - Every x days
     *  1 - Every workday
     *
     */
    function test_showItemDaily()
    {
       $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("daily_choice"=>1,
                              'end_choice'=>2,
                              "cyclus_enddate"=>array("day"=>5,"month"=>10,"year"=>2006),
                              "daily_every"=>2),
              "recur"=>"daily");
      $startdate = schedulertools::getItemStartDate($item);
      $enddate = schedulertools::getItemEndDate($item);
      $checkdate = mktime(12,0,0,10,2,2006);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show daily item (every x days)');
      $checkdate = mktime(12,0,0,10,3,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show daily item (every x days)');

      $item['cyclus']=array("daily_choice"=>2,
                            "cyclus_enddate"=>array("day"=>5,"month"=>10,"year"=>2006));
      $checkdate = mktime(12,0,0,10,1,2006);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show daily item (every workday)');
      $checkdate = mktime(12,0,0,10,2,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show daily item (every workday)');
    }

    /**
     * - Every x weeks
     * - weekday (flag)
     */
    function test_showItemWeekly()
    {
      //Weekly item, every week on Monday, Tuesday
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2006),
                              'end_choice'=>2,
                              "weekly_every"=>1,
                              "weekly_weekday"=>array(6)),
              "recur"=>"weekly");
      $startdate = schedulertools::getItemStartDate($item);
      $enddate = schedulertools::getItemEndDate($item);
      $checkdate = mktime(0,0,0,10,2,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show weekly item');
      $checkdate = mktime(0,0,0,10,4,2006);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t show weekly item, day is not a monday or tuesday');

      $item["cyclus"]=array("cyclus_enddate"=>array("day"=>31,"month"=>10,"year"=>2006),
                      'end_choice'=>2,
                      "weekly_every"=>2,
                      "weekly_weekday"=>array(6));
      $checkdate = mktime(0,0,0,10,2,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show weekly item');
      $checkdate = mktime(0,0,0,10,10,2006);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t show weekly item');
      $checkdate = mktime(0,0,0,10,17,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show weekly item');
    }

    /**
     * - Day x of every x months
     * - xth weekday of x months
     */
    function test_showItemMonthly()
    {
       $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("monthly_choice"=>1,
                              'end_choice'=>2,
                              "cyclus_enddate"=>array("day"=>10,"month"=>2,"year"=>2007),
                              "monthly_every"=>2,
                              "monthly_day"=>1),
              "recur"=>"monthly");
      $startdate = schedulertools::getItemStartDate($item);
      $enddate = schedulertools::getItemEndDate($item);
      $checkdate = mktime(0,0,0,10,1,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Monthly item');
      $checkdate = mktime(0,0,0,11,1,2006);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show Monthly item');
      $checkdate = mktime(0,0,0,12,1,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Monthly item');


      $item['cyclus']=array("monthly_choice"=>2,
                            "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2007),
                              'end_choice'=>2,
                            "monthly_month_time"=>2,
                            "monthly_weekday_list"=>1,
                            "monthly_every2"=>2);
      $checkdate = mktime(0,0,0,10,8,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Monthly item');
      $checkdate = mktime(0,0,0,12,10,2006);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Monthly item');
      $checkdate = mktime(0,0,0,12,17,2006);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show Monthly item');
    }

    /**
     * - every month, day
     * - xth weekday of month
     */
    function test_showItemYearly()
    {
       $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("yearly_choice"=>1,
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                              'end_choice'=>2,
                              "yearly_day"=>2,
                              "yearly_month"=>1),
              "recur"=>"yearly");
      $checkdate = mktime(0,0,0,1,2,2007);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Yearly item');
      $checkdate = mktime(0,0,0,1,2,2008);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Yearly item');
      $checkdate = mktime(0,0,0,2,2,2008);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show Yearly item');

      $item['cyclus']=array("yearly_choice"=>2,
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                              'end_choice'=>2,
                              "yearly_month_time"=>2,
                              "yearly_weekday_list"=>1,
                              "yearly_month2"=>2);
      $checkdate = mktime(0,0,0,2,11,2007);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Yearly item');
      $checkdate = mktime(0,0,0,2,10,2008);
      $this->assertTrue(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Show Yearly item');
      $checkdate = mktime(0,0,0,2,17,2008);
      $this->assertFalse(schedulertools::showItem($item,$checkdate,$startdate,$enddate),'Don\'t Show Yearly item');

    }


    function test_getDates_once()
    {
       // Once item on the same day
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "starttime"=>"10:00",
                    "endtime"=>"05:00",
                    "recur"=>"once");
      $valid_dates = array('2006-10-01');
      $dates = schedulertools::getDates($item,"2006-10-01","2006-10-01");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertFalse($ret, "Once item on one day");

      // Once Item on several days
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
                    "enddate"=>array("day"=>5,"month"=>10,"year"=>2006),
                    "starttime"=>"10:00",
                    "endtime"=>"05:00",
                    "recur"=>"once");
      $valid_dates = array('2006-10-01');
      $dates = schedulertools::getDates($item,"2006-10-01","2006-10-01");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertFalse($ret, "Once item on multiple days");
    }

    /**
     *  0 - Every x days
     *  1 - Every workday
     */
    function test_getDates_daily()
    {
      // Daily item on the same day
       $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("daily_choice"=>1,
                              'end_choice'=>2,
                              "cyclus_enddate"=>array("day"=>5,"month"=>10,"year"=>2006),
                              "daily_every"=>2),
              "recur"=>"daily");
      $valid_dates = array('2006-10-01','2006-10-03','2006-10-05');
      $dates = schedulertools::getDates($item,"2006-09-15","2006-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Daily item, every x days");

      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2008),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2008),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("daily_choice"=>2,
                              'end_choice'=>2,
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2008)),
              "recur"=>"daily");
      $valid_dates = array('2008-10-01','2008-10-02','2008-10-03','2008-10-06','2008-10-07','2008-10-08','2008-10-09','2008-10-10');
      $dates = schedulertools::getDates($item,"2006-09-15","2006-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Daily item, every workday");
    }

    /**
     * - Every x weeks
     * - weekday (flag)
     */
    function test_getDates_weekly()
    {
      //Weekly item, every week on Monday, Tuesday
      $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2008),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2008),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2008),
                              'end_choice'=>2,
                              "weekly_every"=>1,
                              "weekly_weekday"=>array(6)),
              "recur"=>"weekly");
      $valid_dates = array('2008-10-06','2008-10-07');
      $dates = schedulertools::getDates($item,"2008-09-15","2008-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Weekly item, every week on Monday, Tuesday");

      $item["cyclus"]=array("cyclus_enddate"=>array("day"=>31,"month"=>10,"year"=>2008),
                              'end_choice'=>2,
                              "weekly_every"=>2,
                              "weekly_weekday"=>array(6));
      $valid_dates = array('2008-10-06','2008-10-07','2008-10-20','2008-10-21');
      $dates = schedulertools::getDates($item,"2008-10-01","2008-10-31");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Weekly item, every 2 weeks on Monday, Tuesday");
    }

     /**
     * - Day x of every x months
     * - xth weekday of x months
     */
    function test_getDates_monthly()
    {
       $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2008),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2008),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("monthly_choice"=>1,
                              'end_choice'=>2,
                               "cyclus_enddate"=>array("day"=>10,"month"=>2,"year"=>2009),
                              "monthly_every"=>2,
                              "monthly_day"=>1),
              "recur"=>"monthly");
      $valid_dates = array('2008-10-01','2008-12-01','2009-02-01');
      $dates = schedulertools::getDates($item,"2008-09-15","2009-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Monthly item, day 1 of every 2 months");

      $item['cyclus']=array("monthly_choice"=>2,
                              'end_choice'=>2,
                            "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                            "monthly_month_time"=>2,
                            "monthly_weekday_list"=>1,
                            "monthly_every2"=>2);
      $valid_dates = array('2008-10-12','2008-12-14','2009-02-08','2009-04-12','2009-06-14','2009-08-09');
      $dates = schedulertools::getDates($item,"2008-09-15","2009-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Monthly item, 2nd sunday of every 2 months");

      $item['cyclus']=array("monthly_choice"=>2,
                              'end_choice'=>2,
                            "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                            "monthly_month_time"=>-1,
                            "monthly_weekday_list"=>1,
                            "monthly_every2"=>2);
      $valid_dates = array('2008-10-26','2008-12-28','2009-02-22','2009-04-26','2009-06-28','2009-08-30');
      $dates = schedulertools::getDates($item,"2008-09-15","2009-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Monthly item, last sunday of every 2 months");
    }

     /**
     * - every month, day
     * - xth weekday of month
     */
    function test_getDates_yearly()
    {
       $item = array("startdate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "enddate"=>array("day"=>1,"month"=>10,"year"=>2006),
              "starttime"=>"10:00",
              "endtime"=>"12:00",
              "cyclus"=>array("yearly_choice"=>1,
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                              'end_choice'=>2,
                              "yearly_day"=>2,
                              "yearly_month"=>1),
              "recur"=>"yearly");
      $valid_dates = array('2007-01-02','2008-01-02','2009-01-02');
      $dates = schedulertools::getDates($item,"2006-09-15","2010-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Yearly item, every 2 januari");

      $item['cyclus']=array("yearly_choice"=>2,
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                              'end_choice'=>2,
                              "yearly_month_time"=>3,
                              "yearly_weekday_list"=>1,
                              "yearly_month2"=>2);
      $valid_dates = array('2007-02-18','2008-02-17','2009-02-15');
      $dates = schedulertools::getDates($item,"2006-09-15","2009-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Yearly item, every 3th sunday of februari");


      $item['cyclus']=array("yearly_choice"=>2,
                              "cyclus_enddate"=>array("day"=>10,"month"=>10,"year"=>2009),
                              'end_choice'=>2,
                              "yearly_month_time"=>-1,
                              "yearly_weekday_list"=>1,
                              "yearly_month2"=>2);
      $valid_dates = array('2007-02-25','2008-02-24','2009-02-22');
      $dates = schedulertools::getDates($item,"2006-09-15","2009-10-20");
      $ret = atkArrayCompare($valid_dates,$dates);
      $this->assertEqual(false, $ret, "Yearly item, every last sunday of februari");



    }




  }