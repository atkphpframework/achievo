<?
  class monthView
  {
   var $days        = 0;
   var $month       = 0;
   var $year        = 0;
   var $weeks       = 0;
   var $items       = array();
   var $title       = "";
   var $viewerWidth = "100%";
   var $fancyBegin  = "<TABLE height='100%' width='100%' border='0' cellPadding='0'><TBODY><TR><TD height='100%' bgColor='#000000'><TABLE width='100%' height='100%' border='0' cellPadding='0' cellSpacing='0'><TBODY><TR height='100%'>";
   var $fancyEnd    = "</TR></TBODY></TABLE></TD></TR></TBODY></TABLE>"; 

   function monthView($month=0, $year=0)
   {
    $this->year     = ($year  == 0) ? date("Y") : $year;
    $this->month    = ($month == 0) ? date("m") : $month;
   }

   function monthNav()
   { 
    global $PHP_SELF;

    $prevTime = date("Y-m-d", mktime(0,0,0,$this->month-1, 1, $this->year));
    $prevUrl  = $PHP_SELF."?view=monthview&viewdate=$prevTime&dayview=".$this->showGlobal;

    $thisTime = date("Y-m")."-1";
    $thisUrl  = $PHP_SELF."?view=monthview&viewdate=$thisTime&dayview=".$this->showGlobal;

    $nextTime = date("Y-m-d", mktime(0,0,0,$this->month+1, 1, $this->year));
    $nextUrl  = $PHP_SELF."?view=monthview&viewdate=$nextTime&dayview=".$this->showGlobal;

    $this->title = date("M", mktime(0,0,0,$this->month,1,$this->year))." ".$this->year;

    return href($prevUrl, text("Previous month"))."&nbsp;".href($thisUrl, text("This month"))."&nbsp;".href($nextUrl, text("Next month"));
   }

   function generate()
   {

    $days = 1;
    while (checkdate($this->month, $days, $this->year))
    {
     $days++;
    }
    $days--;

    $weeks = ceil($days/7);
    $begin = 1;
    $n     = 0;

    $this->days  = $days;
    $this->weeks = $weeks;

    for ($j=0;$j<=$weeks;$j++)
    {
     $viewtime = mktime(0,0,0,$this->month, $begin, $this->year);
     $weekday  = strftime("%w",$viewtime);
 
     if ($weekday==0) $weekday=7;

     for ($i=1; $i<=7;$i++)
     {
      $items[$n]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $items[$n]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
      $items[$n]['stamp'] = $viewtime-(86400*($weekday-$i));
      $n++;
     }
   
     $begin = $begin+7;
    }

    $this->items = $items;
   }

   function dumpMatrix()
   {
    global $PHP_SELF;

    $item     = 0;
    $colwidth = floor($this->viewerWidth/7);
    $today    = date("Y-m-d");

    $buff  = "<table width='".$this->viewerWidth."' cellpadding='1' cellspacing='1'>\n";
    $buff .= "<tr>\n";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("Mon")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("Tue")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("Wed")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("Thu")."</font></td>";
    $buff .= "<td bgcolor='#D0D0D0' valign='top' align='left'><font size='-1'>".text("Fri")."</font></td>";
    $buff .= "<td bgcolor='#A0A0A0' valign='top' align='left'><font size='-1'>".text("Sat")."</font></td>";
    $buff .= "<td bgcolor='#A0A0A0' valign='top' align='left'><font size='-1'>".text("Sun")."</font></td>";
    $buff .= "</tr>\n";

    for ($i=0;$i<=$this->weeks;$i++)
    {
     $buff .= "<tr>";
     for ($j=0;$j<=6;$j++)
     {
      $url     = $PHP_SELF."?view=dayview&viewdate=".$this->items[$item]["date"];
      $day     = substr($this->items[$item]["date"],8,2);
      $month   = substr($this->items[$item]["date"],5,2);
      $bgcolor = "#C0C0C0";

      if ($this->month == $month) { $bgcolor = "#FFFFFF"; }
      if ($this->items[$item]["date"] == $today) { $bgcolor = "#FFCC33"; }

      $buff .= "<td width='$colwidth' bgcolor='$bgcolor' valign='top' align='left'><font size='-1'>".href($url, $day)."</font></td>";
      $item++;
     }
     $buff .= "</tr>";
    }

    $buff .= "</table><br><br>";
    return $buff;
   }

  }
?>   