<?

   // class to generate a dayview
   //
   // typical usage:
   //
   // $dummy = new dayview(25,7,2001);     // show July 25th 2001
   // $dummy->showGlobal = 0;              // only personal items (1 for global & personal items)
   // $dummy->owner      = "wim";          // for user wim
   // $dummy->generate();                  // generate the matrix
   // $dummy->dumpMatrix();                // show it to the audience ....
   //
   //

   class dayView
   {
    var $year         = 0;
    var $month        = 0;
    var $day          = 0;    
    var $maxCols      = 15;
    var $allItems     = array();
    var $allDayItems  = array();
    var $appointItems = array();
    var $itemMatrix   = array();
    var $matrixInfo   = array();
    var $viewerWidth  = "100%";
    var $workHours    = array("08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30","14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00","21:30","22:00","22:30","23:00","23:30","24:00");
    var $allDayHours  = array("00:30","01:00","01:30","02:00","02:30","03:00","03:30","04:00","04:30","05:00","05:30","06:00","06:30","07:00","07:30","08:00","08:30","09:00","09:30","10:00","10:30","11:00","11:30","12:00","12:30","13:00","13:30","14:00","14:30","15:00","15:30","16:00","16:30","17:00","17:30","18:00","18:30","19:00","19:30","20:00","20:30","21:00","21:30","22:00","22:30","23:00","23:30","24:00");
    var $owner;
    var $emptyColor   = "#FFFFFF";
    var $fancy        = 1;
    var $fancyBegin   = "<TABLE height='100%' width='100%' border='0' cellPadding='2'><TBODY><TR><TD height='100%' bgColor='#000000'><TABLE width='100%' height='100%' border='0' cellPadding='2' cellSpacing='0'><TBODY><TR height='100%'>";
    var $fancyEnd     = "</TR></TBODY></TABLE></TD></TR></TBODY></TABLE>"; 
    var $showGlobal;
    var $busyBgColor  = "#D0D0D0";
    var $busyFgColor  = "#000000";
    var $busyTitle    = "Occupied";


  /**
   * constructor
   * @param $day   (if 0, then current day)
   * @param $month (if 0, then current month)
   * @param $year  (if 0, then current year)
   */
    function dayView($day=0, $month=0, $year=0)
    {
     $this->year     = ($year  == 0) ? date("Y") : $year;
     $this->month    = ($month == 0) ? date("m") : $month;
     $this->day      = ($day   == 0) ? date("d") : $day;    
    }

  /**
   * wrapper around the gathering functions
   * fills several info arrays
   */
    function generate()
    {
     // get all items for today
     $this->allItems = $this->getItems();

     // filter on allday events / other appointment events
     $this->filterItems();

     // get scheduler types and their colors
     $this->colors = $this->getColors();
    }

  /**
   * dayNav
   * navigate through days using prev/next day link
   * @returns html code containing links for previous day, today and next day
   */
   function dayNav()
   {
    global $PHP_SELF;

    $prevTime = date("Y-m-d", mktime(0,0,0,$this->month, $this->day-1, $this->year));
    $prevUrl  = $PHP_SELF."?view=dayview&viewdate=$prevTime&dayview=".$this->showGlobal;

    $thisTime = date("Y-m-d");
    $thisUrl  = $PHP_SELF."?view=dayview&viewdate=$thisTime&dayview=".$this->showGlobal;

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->month, $this->day+1, $this->year));
    $nextUrl  = $PHP_SELF."?view=dayview&viewdate=$nextTime&dayview=".$this->showGlobal;

    return href($prevUrl, text("Previous day"))."&nbsp;".href($thisUrl, text("Today"))."&nbsp;".href($nextUrl, text("Next day"));
   }

  /**
   * get the items for the specified date
   * fills an array with the items
   */
    function getItems()
    {
     global $g_db;

     // $showGlobal = "all"               -> all public items for all users
     // $showGlobal = $owner              -> all items (public/personal items) for current user
     // $showGlobal != "all" && != $owner -> all public items for specified user + all none-public items as grey 'busy' boxes

     $date = $this->year."-".$this->month."-".$this->day;
     $q    = "SELECT * FROM schedule WHERE item_begin_date <='$date' AND item_end_date >= '$date' ";

     if ($this->showGlobal == "all")
     {
//      $q .= "AND (item_public=1 OR item_owner='".$this->owner."')";
      $q .= "AND item_public=1";
     }
     else
     {
      $q .= "AND item_owner='".$this->showGlobal."'";
     }

     $q .= " ORDER BY item_begin_time";
     return $g_db->getRows($q);
    }


  /**
   * getColors
   * @return array containing description, fore- and background for each scheduler type
   */
   function getColors()
   {
    global $g_db;

    // get the defined colors for each scheduler-type   
    $q = "SELECT * FROM schedule_types ORDER BY id";
    $r = $g_db->getRows($q);

    $colors  = array();
    $legend  = "<TABLE border='0' cellPadding='1'><TBODY><TR><TD bgColor='#000000'><TABLE width='100%' border='0' cellPadding='2' cellSpacing='0'><TBODY>";

    for ($i=0;$i<=count($r)-1;$i++)
    {
     $colors[$r[$i]["id"]]["desc"] = $r[$i]["item_description"];
     $colors[$r[$i]["id"]]["fore"] = $r[$i]["item_foreground"];
     $colors[$r[$i]["id"]]["back"] = $r[$i]["item_background"];

     $tmpLegend                    = "<TR bgcolor='#FFFFFF'><TD>\n";
     $tmpLegend                   .= "<table border='0' cellpadding='0' cellspacing='0'><tr><td bgcolor='".$r[$i]["item_background"]."'><img src='images/trans.gif' border='1' width='10' height='10'></td></tr></table>";
     $tmpLegend                   .= "</TD><TD><font color='#000000' face='verdana' size='-1'>".$r[$i]["item_description"]."</font></TD>";
     $tmpLegend                   .= "</TR>"; 
     $legend                      .= $tmpLegend;
    }

    $this->legend .= $legend."</TBODY></TABLE></TD></TR></TBODY></TABLE>";   
    return $colors;
   }


  /**
   * findInArray
   * $param $what  (look for what
   * @param $where (1d array to look in)
   * @return index within the array if found, else -1
   */
    function findInArray($what, $where)
    {
     $res = -1;
     for ($i=0;$i<=count($where)-1;$i++)
     {
      if ($what == $where[$i])
      {
       $res = $i;
       break;
      }
     }
     return $res;
    }

  /**
   * filterItems
   * seperates the items for allday events and normal appointments
   * fills two arrays
   */
    function filterItems()
    {
     for ($i=0;$i<=count($this->allItems)-1;$i++)
     {
      if ($this->allItems[$i]["item_allday"] == 1)
      {
       $this->allDayItems[] = $this->allItems[$i];
      }
      else
      {
       $this->appointItems[] = $this->allItems[$i];
       $this->setMatrix($this->allItems[$i]);
      }
     }
    }


  /**
   * duration
   * $param $begin (begin time, should be something like 09:00 or 10:30)
   * @param $eind  (end time, should be something like 09:00 or 10:30)
   * @return array containing all half hour items between begin and end (eg 09:00, 09:30, 10:00)
   */
    function duration($begin, $eind)
    {
     $a = $this->findInArray($begin, $this->allDayHours);
     $b = $this->findInArray($eind, $this->allDayHours);

     for ($i=$a;$i<=$b-1;$i++)                      // a half our is now one bar, for two bars remove -1 after <=$b
     {
      $items[] = $this->allDayHours[$i];
     }
    
     return $items;
    }

  /**
   * isColFree
   * $param $duration  (array with duration items, see duration() function)
   * @param $col       ($collumn to check)
   * @return 0 if all cols for $col are free for the items in $duration, else > 0
   */
    function isColFree($duration, $col)
    {
     $used = 0;
     for ($i=0;$i<=count($duration)-1;$i++)
     {
      if (trim($this->itemMatrix[$duration[$i]][$col]) != "") { $used++; }
     }
     return $used;
    }


  /**
   * setMatrix
   * Adds an appointment to a free timeslot (duration/col)
   * $param $item  (full record containing all info about the specified appointment)
   * @return piece of html code with a checkbox
   */
    function setMatrix($item)
    {
     $begin    = substr($item["item_begin_time"],0,5);
     $einde    = substr($item["item_end_time"],0,5);     
     $duration = $this->duration($begin, $einde);
     $appoints = count($this->itemMatrix[$begin]);

     // find the first free collumn in which this item fits
     if ($appoints == 0)
     {
      $col = 0;
     }
     else
     {
      for ($col=0;$col<=$this->maxCols;$col++)
      {
       if ($this->isColFree($duration, $col) == 0)
       {
        break;
       }
      }       
     }

     // add the duration (used in the display part for rowspan)
     $this->matrixInfo[$duration[0]][$col]["duration"] = count($duration); 
    
     //fill the matrix
     for ($i=0;$i<=count($duration)-1;$i++)
     {
      $this->itemMatrix[$duration[$i]][$col]         = $item["item_title"];
      $this->matrixInfo[$duration[$i]][$col]["info"] = $item;
     }

    }


  /**
   * getCols
   * get number of collumns (<TD>'s)
   * @return integer containing number of <TD>'s needed within the display mode
   */
    function getCols()
    {
     $cols = 0;
     while (list($key, $val) = each($this->itemMatrix))
     {
      $a = count($val);
      if ($cols < $a) { $cols = $a; }
     }
     return $cols;
    }


  /**
   * dumpMatrix
   * the actual displaying function 
   */
    function dumpMatrix()
    {
     global $PHP_SELF;

     $buff   = "";
     $cols   = $this->getCols();
     $colors = $this->colors;

     if ($cols == 0)
     {
      $cWidth = "95%";
      $aCols  = 2;
     }
     else
     {
      $cWidth = floor(95/$cols)."%";
      $aCols  = $cols+1;
     }

     $buff .= "<font size='-1'><b>".text(date("l", mktime(0, 0, 0, $this->month, $this->day, $this->year)))." ".$this->day."-".$this->month."-".$this->year."</b></font>";
     $buff .= "<table border='0' cellpadding='1' cellspacing='1' width='".$this->viewerWidth."'>\n";

     for ($i=0;$i<=count($this->allDayItems)-1;$i++)
     {
      $type    = $this->allDayItems[$i]["item_type"];
      $title   = $this->allDayItems[$i]["item_title"];
      $bgcolor = $colors[$type]["back"];
      $fgcolor = $colors[$type]["fore"];

      $buff .= "<tr>\n";
      $buff .= "<td colspan='$aCols' bgcolor='$bgcolor' valign='top' align='left'>";
      $buff .= "<font face='verdana' size='-1' color='$fgcolor'><b>".$title."</b></font>";     
      $buff .= "</td>";
      $buff .= "</tr>";
     } 

     for ($i=0;$i<=count($this->workHours)-1;$i++)
     {
      $time    = $this->workHours[$i];
      $items   = $this->itemMatrix[$time]; 
      $now     = $this->year."-".$this->month."-".$this->day;              
      $addurl  = $PHP_SELF."?atkaction=add&viewdate=$now&hour=$time";
 
      $buff .= "<tr>\n";
      $buff .= "<td bgcolor='#666666' width='5%'><font face='verdana' size='-1' color='#FFFFFF'><b>".href($addurl,$time)."</b></font></td>\n"; 

      if ($cols == 0)
      {
       $buff .= "<td width='$cWidth' bgcolor='".$this->emptyColor."'>&nbsp;</td>\n";
      }
      else
      {
       for ($j=0;$j<=$cols-1;$j++)
       {
        $item    = $items[$j];
        $timeLap = $this->matrixInfo[$time][$j]["duration"];
        $info    = $this->matrixInfo[$time][$j]["info"];
 
        if (trim($item) != "")
        { 
         if ($timeLap != 0)
         {
 
          if ($timeLap == 1)
          {
           $rowspan = "";
          }
          else
          {
           $rowspan = "rowspan='$timeLap'";
          }

          if ($info["item_global"] == 0 && $info["item_owner"] != $this->owner)
          {
           $title   = $this->busyTitle;
           $bgcolor = $this->busyBgColor;
           $fgcolor = $this->busyFgColor;
           $descr   = "";
          }
          else
          {
           $title   = $item;
           $bgcolor = $colors[$info["item_type"]]["back"];
           $fgcolor = $colors[$info["item_type"]]["fore"];
           $descr   = $info["item_description"];
          }
 
          if ($this->fancy != "1")
          {
           $buff .= "<td $rowspan height='100%' valign='top' width='$cWidth' bgcolor='".$bgcolor."'><font color='$fgcolor'><center>".$title."</center></font></td>\n";
          }
          else
          {
           $buff .= "<td height='100%' $rowspan valign='top' width='$cWidth'>";
           $buff .= $this->fancyBegin;
           $buff .= "<td height='100%' bgcolor='$bgcolor' valign='top' align='left'>";
           $buff .= "<font face='verdana' size='-1' color='$fgcolor'><b>".$title."</b></font>";
 
           if ($timeLap > 1)
           {
            $buff .= "<br><font face='verdana' size='-2' color='$fgcolor'>".$descr."</font>"; 	
           }
 
           $buff .= "</td>\n";
           $buff .= $this->fancyEnd;
           $buff .= "</td>";
          }
         }
        }
        else
        {
         $buff .= "<td width='$cWidth' bgcolor='".$this->emptyColor."'>&nbsp;</td>\n";
        }
       }
      }

      $buff .= "</tr>\n";
     }
     $buff .= "</table><br><br>\n";
     return $buff;
    }

   }

?>