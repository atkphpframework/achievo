<?
  class achievoCalendar 
  { 
   var $displayDay     = 0;          // displayed day
   var $displayMonth   = 0;          // displayed month
   var $displayYear    = 0;          // displayed year
   var $calRows        = array();
   var $totalDays      = 0;

   // constructor function 
   function achievoCalendar($month=0, $year=0, $day = 0)
   {
    $this->displayMonth = ($month == 0) ? date("m") : $month;
    $this->displayYear  = ($year  == 0) ? date("Y") : $year;
    $this->displayDay   = ($day   == 0) ? date("d") : $day;  // only used in week views
   }

   
   // determine number of days in a month
   function daysInMonth()
   {
    $totalDays = 0; 
    while (checkdate($this->displayMonth, $totalDays+1, $this->displayYear))
    {
     $totalDays++; 
    }
    return $totalDays;
   }


   // small helper function
   // input is a month and a year
   // and it returns the next month / year
   function nextMonth($month, $year)
   {
    if ($month == 12)
    {
     return array(1, $year+1);
    }
    else
    {
     return array($month+1, $year);
    }   
   }


   // calculate the days in a specified month
   // handles indent so a 1st day of a month can be on a wednesday
   function monthItems()
   {
    $item          = 1;
    $totalDays     = $this->daysInMonth();
    $this->calRows = array();

    $emptyOffset = date("w", mktime( 0, 0, 0, $this->displayMonth, 1, $this->displayYear));
    for ($i = 0;$i<$emptyOffset;$i++)
    { 
     $this->calRows[$item] = "&nbsp;";
     $item++;
    }

    for ($dag = 1;$dag<=$totalDays; $dag++ )
    {
     $this->calRows[$item] = $dag;
     $emptyOffset++; 

     if ($emptyOffset > 6)
     { 
      $emptyOffset = 0;
     }

     $item++; 
    } 
    
    if ($emptyOffset > 0)
    {
     $emptyOffset = 7 - $emptyOffset; 
    }

    for ( $i = 0; $i <$emptyOffset; $i++ )
    {
     $this->calRows[$item] = "&nbsp;";
     $item++;
    }

    return $this->calRows;
   }


   // calculate the items for the specified quarter
   // starting with $this->displayMonth
   // just a wrapper around the monthItems()
   function quarterItems()
   {
    $items    = array();
    $items[1] = $this->monthItems();

    $tmp = $this->nextMonth($this->displayMonth+1, $this->displayYear);
    $this->displayMonth = $tmp[0];
    $this->displayYear  = $tmp[1];
    $items[2]           = $this->monthItems();

    $tmp = $this->nextMonth($this->displayMonth+1, $this->displayYear);
    $this->displayMonth = $tmp[0];
    $this->displayYear  = $tmp[1];
    $items[3]           = $this->monthItems();
    
    return $items;
   }


   // calculate the items for the specified year
   // also another wrapper around the monthItems()
   function yearItems()
   {
    $items = array();

    for ($i=1;$i<=12;$i++)
    {
     $this->displayMonth = $i;
     $items[$i]          = $this->monthItems();     
    }
    
    return $items;
   }


  // calculate the items for this week
  // takes a date and it returns the complete
  // week this date is in.
  // just a copy from the hours class weekView()
  function weekItems()
  {
   $viewtime = mktime(0,0,0,$this->displayMonth, $this->displayDay, $this->displayYear);
   $weekday  = strftime("%w",$viewtime);
 
   if ($weekday==0) $weekday=7;

   for ($i=0; $i<=8;$i++)
   {
    $items[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
    $items[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
    $items[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
   }
   return $items;
  } 

 }
?>