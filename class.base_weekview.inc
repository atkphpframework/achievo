<?
  class weekView
  {
   var $day         = 0;
   var $month       = 0;
   var $year        = 0;
   var $items       = array();
   var $viewerWidth = "100%";
   var $fancyBegin  = "<TABLE height='100%' width='100%' border='0' cellPadding='0'><TBODY><TR><TD height='100%' bgColor='#000000'><TABLE width='100%' height='100%' border='0' cellPadding='0' cellSpacing='0'><TBODY><TR height='100%'>";
   var $fancyEnd    = "</TR></TBODY></TABLE></TD></TR></TBODY></TABLE>"; 

   function weekView($day=0, $month=0, $year=0)
   {
    $this->year     = ($year  == 0) ? date("Y") : $year;
    $this->month    = ($month == 0) ? date("m") : $month;
    $this->day      = ($day   == 0) ? date("d") : $day;  
   }

   function generate()
   {
    $viewtime = mktime(0,0,0,$this->month, $this->day, $this->year);
    $weekday  = strftime("%w",$viewtime);
 
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
     $items[$i]['date']  = date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
     $items[$i]['day']   = strtolower(date("D" ,$viewtime-(86400*($weekday-$i))));
     $items[$i]['stamp'] = $viewtime-(86400*($weekday-$i));
    }
    $this->items = $items;
   }

   function dumpMatrix()
   {
    global $PHP_SELF;

    $buff  = $this->fancyBegin."<td><table width='".$this->viewerWidth."' cellpadding='1' cellspacing='1' border='0'>\n";

    $buff .= "<tr bgcolor='#D0D0D0'>\n";
    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $text  = text($this->items[$i]["day"])."<br>".substr($this->items[$i]["date"],8,2)."-".substr($this->items[$i]["date"],5,2);
     $href  = $PHP_SELF."?view=dayview&viewdate=".$this->items[$i]["date"];
     $buff .= "<td>".href($href, $text)."</td>";
    }
    $buff .= "</tr>";

    $buff .= "<tr bgcolor='#FFFFFF'>";
    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $buff .= "<td><br><br><br></td>";
    }
    $buff .= "</tr>";

    $buff .= "<tr bgcolor='#D0D0D0'>\n";
    for ($i=1;$i<=count($this->items)-2;$i++)
    {
     $text  = text("New item");
     $href  = $PHP_SELF."?atkaction=add&viewdate=".$this->items[$i]["date"];
     $buff .= "<td>".href($href, $text)."</td>";
    }
    $buff .= "</tr>";

    $buff .= "</table></td>".$fancyEnd;

    return $buff;
   }
  }
?>   