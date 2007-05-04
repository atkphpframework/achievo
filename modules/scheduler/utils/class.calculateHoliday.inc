<?php


class calculateHoliday
{
  var $m_year = 0;

  var $m_mode="";
  var $m_special="";
  var $m_offset=0;
  var $m_weekday=-1;
  var $m_month = 0;
  var $m_day = 0;
  var $m_moveTo=0;
  var $m_length=1;

  /**
   * Constructor
   */
   function calculateHoliday()
   {
   }


  /**
   *
   * @params array $holiday Array with all the info about the holiday
   * @params int $year The year we are going to calculate
   * @return int Timestamp of the calculated date
   *
   */
   function getHolidayDate($holiday,$year)
   {
     $this->m_year=($year!=""?$year:date("Y"));

     $special=($holiday["special"]!=""?$holiday["special"]:"");
     $weekday=($holiday["weekday"]!=""?$holiday["weekday"]:-1);
     $month=($holiday["month"]!=""?$holiday["month"]:0);
     $day=($holiday["day"]!=""?$holiday["day"]:0);
     $offset=($holiday["day_offset"]!=""?$holiday["day_offset"]:0);
     $moveto=($holiday["moveto"]!=""?$holiday["moveto"]:0);
     $length=($holiday["length"]!=""?$holiday["length"]:1);

     if($special!="")
     {
       $this->_seteaster($special,$offset,$length);
     }
     else
     {
       if($weekday==-1)
       {
         $this->_setdate($month,$day,$length);
       }
       elseif($day==0)
       {
         $this->_setwday($weekday,$month,$offset,$length);
       }
       else
       {
         $this->_setdoff($weekday,$month,$day,$offset,$length);
       }
     }
     return $this->_calculateDate();
   }

  /**
   * Parse a date holiday
   *
   * @param string $name Holiday Name
   * @param string $type Holiday type (info, weekend)
   * @param int $month Month
   * @param int $day Day
   * @param int $length Length of the holiday in days
   *
   * @example 1 Januari (1,1)
   */
   function _setdate($month,$day,$length)
   {
     $this->m_mode="Date";
     $this->m_special="";
     $this->m_offset=0;
     $this->m_weekday=0;
     $this->m_month = $month;
     $this->m_day = $day;
     $this->m_moveTo=0;
     $this->m_length = $length;
   }

  /**
   * Parse a easter date holiday
   *
   * @param string $name Holiday Name
   * @param string $type Holiday type (info, weekend)
   * @param int $special For now only Easter
   * @param int $offset Offset
   * @param int $length Length of the holiday in days
   *
   * @example 49 days after easter (easter,49)
   */
   function _seteaster($special,$offset,$length)
   {
     $this->m_mode="Special";
     $this->m_special=$special;
     $this->m_offset=$offset;
     $this->m_weekday=0;
     $this->m_month = 0;
     $this->m_day = 0;
     $this->m_moveTo=0;
     $this->m_length = $length;
   }

  /**
   * Parse a Weekday date holiday
   *
   * @param string $name Holiday Name
   * @param string $type Holiday type (info, weekend)
   * @param int $weekday Weekday
   * @param int $month Month
   * @param int $offset Offset
   * @param int $length Length of the holiday in days
   *
   * @example  Ex. 2nd sunday in may (7,5,2)
   */
   function _setwday($weekday,$month,$offset,$length)
   {
     $this->m_mode="Weekday";
     $this->m_special="";
     $this->m_offset=$offset-1;
     $this->m_weekday=$weekday;
     $this->m_month = $month;
     $this->m_day = 1;
     $this->m_moveTo=0;
     $this->m_length = $length;
   }

  /**
   * Parse a Weekday date holiday
   *
   * @param string $name Holiday Name
   * @param string $type Holiday type (info, weekend)
   * @param int $weekday Weekday
   * @param int $day Day
   * @param int $month Month
   * @param int $offset Offset
   * @param int $length Length of the holiday in days
   *
   * @example  First friday after the 18th day of june. (5,6,18,1)
   */
   function _setdoff($weekday,$month,$day,$offset,$length)
   {
     $this->m_mode="Weekday";
     $this->m_special="";
     $this->m_offset=$offset-1;
     $this->m_weekday=$weekday;
     $this->m_month = $month;
     $this->m_day = $day;
     $this->m_moveTo=0;
     $this->m_length = $length;
   }

 /**
  * Calculate the actual date
  *
  * @return timestamp The timstamp for the date
  */
  function _calculateDate()
  {
    switch ($this->m_mode)
    {
      case "Date":    $date = mktime(12,0,0,$this->m_month,$this->m_day,$this->m_year);
                      break;
      case "Special": if ($this->m_special=="easter")
                        $date = $this->_easter($this->m_offset);
                      break;
      case "Weekday": $date = mktime(12,0,0,$this->m_month,$this->m_day,$this->m_year);
                      if(date("w",$date)<=$this->m_weekday)
                      {
                        $date+=$this->_addDays($this->m_weekday-date("w",$date));
                      }
                      else
                      {
                        $date+=$this->_addDays($this->m_weekday-date("w",$date)+7);
                      }
                      if($this->m_offset>=0)
                      {
                        $date+=$this->_addDays(7*$this->m_offset);
                      }
                      else
                      {
                        while(date("n",$date)==$this->m_month)
                        {
                          $date+=$this->_addDays(7);
                        }
                      }
                      break;
      default:        break;
    }
    if ($this->m_moveTo>0)
    {
      switch ($this->m_moveTo)
      {
        case 1: if (date("w",$date)==6 || date("w",$date)==0)
                  $date+=$this->_addDays(7-date("w",$date)+$this->m_moveTo);
                break;
        case 2: if (date("w",$date)==0)
                  $date+=$this->_addDays(1);
                break;
        case 3: if (date("w",$date)==6)
                  $date+=$this->_addDays(-1);
                if (date("w",$date)==0)
                  $date+=$this->_addDays(1);
                break;
      }
    }
    $ret_dates = array();
    for($i=0;$i<$this->m_length;$i++)
    {
      $ret_dates[] = ($date+$this->_addDays($i));
    }
    return $ret_dates;
  }

 /**
  * Calculate the seconds for x days
  *
  * @param int $days Add x days
  * @return timestamp seconds to add
  */
  function _addDays($days)
  {
    return (86400*$days);
  }

 /**
  * Calculate the easterdate and add the offset
  *
  * @return timestamp The timestamp of the date
  */
  function _easter($offset)
  {

    $easter = easter_date($this->m_year);
    return ($easter+$this->_addDays($this->m_offset));
  }

}

?>