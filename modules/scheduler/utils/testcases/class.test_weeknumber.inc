<?php

/*
Monday is the first day of week.
1 Week containing Jan 7th is the first week of that year.

Monday is the first day of week. (ISO)
2 Week containing Jan 4th is the first week of that year.

Monday is the first day of week.
3 Week containing Jan 1st is the first week of that year.

Wednesday is the first day of week.
4 Week containing Jan 1st is the first week of that year.

Saturday is the first day of week.
5 Week containing Jan 1st is the first week of that year.

Sunday is the first day of week.
6 Week containing Jan 1st is the first week of that year.
*/

 atkimport("module.scheduler.utils.weeknumber");

  class test_weeknumber extends atkTestCase
  {
     function _getStartDay($startday)
     {
       switch ($startday)
       {
         case 0: return "Sunday";
         case 1: return "Monday";
         case 2: return "Tuesday";
         case 3: return "Wednesday";
         case 4: return "Thursday";
         case 5: return "Friday";
         case 6: return "Saturday";
       }
       return "Unknown";
     }
    
     function test_getWeeknumber()
     {
       
       $startday=1; // Monday
       $weekcontains=1; // 1 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,12,30,2006);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=53;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $timestamp = mktime(12,0,0,1,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $timestamp = mktime(12,0,0,2,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=5;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $startday=1; // Monday
       $weekcontains=4; // 4 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,12,30,2006);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=52;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $timestamp = mktime(12,0,0,1,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $timestamp = mktime(12,0,0,2,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=5;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       
       $startday=1; // Monday
       $weekcontains=7; // 7 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,12,30,2006);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=52;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $timestamp = mktime(12,0,0,1,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $timestamp = mktime(12,0,0,2,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=5;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $startday=3; // Wednesday
       $weekcontains=1; // 1 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,12,30,2006);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
        
       $timestamp = mktime(12,0,0,1,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $timestamp = mktime(12,0,0,2,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=6;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $startday=6; // Saturday
       $weekcontains=1; // 1 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,12,30,2006);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $timestamp = mktime(12,0,0,1,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $timestamp = mktime(12,0,0,2,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=5;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $startday=0; // Sunday
       $weekcontains=1; // 1 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,12,30,2006);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=52;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

       $timestamp = mktime(12,0,0,1,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=1;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $timestamp = mktime(12,0,0,2,1,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=5;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));
       
       $startday=2; // Sunday
       $weekcontains=4; // 1 januari
       $msg = " is in weeknumber %d (Startday %s, week contains %d jan) (expected %d)";
       $timestamp = mktime(12,0,0,7,19,2007);
       $weeknumber = weeknumber::getWeeknumber($timestamp,$startday,$weekcontains);
       $expected=29;
       $this->assertEqual($weeknumber,$expected,date("d-m-Y",$timestamp).sprintf($msg,$weeknumber,$this->_getStartDay($startday),$weekcontains,$expected));

        
     }
    
  }