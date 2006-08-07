<?php

class schedulertools
{
  /**
   * Convert weekday number to flag
   *
   * @param int $weekday_number Weekday number (0 sunday, 6 saturday)
   * @return int Weekday flag
   */
  function weekdaynumbertoflag($weekday_number)
  {
    if (($weekday_number >=0) && ($weekday_number <= 6))
      $weekday = pow(2,$weekday_number); //2^$weekday_number;
    else
      $weekday = 0;

    return $weekday;
  }


  /**
   * Calculate dates for an item
   * @param array $item An scheduler item
   * @param string $rangeStartDate Startdate of the range we want to check
   * @param string $rangeEndDate Enddate of the range we want to check
   * @return array Array with all dates
   */
  function getDates($item,$rangeStartDate=NULL,$rangeEndDate=NULL)
  {
    atk_var_dump($item,"ITEM TO GETDATES");
    $dates = array();
    $datecounter=0;
    if($item["recur"]=="once")
    {
      $startdate = date("Y-m-d",mktime(12,0,0,$item["startdate"]["month"],$item["startdate"]["day"],$item["startdate"]["year"]));
      $enddate = date("Y-m-d",mktime(12,0,0,$item["enddate"]["month"],$item["enddate"]["day"],$item["enddate"]["year"]));
      atkdebug("Once: $enddate");
    }
    else
    {
      $startdate = date("Y-m-d",mktime(12,0,0,$item["startdate"]["month"],$item["startdate"]["day"],$item["startdate"]["year"]));
      //$startdate = date("Y-m-d",mktime(12,0,0,$item["cyclus"]["cyclus_startdate"]["month"],$item["cyclus"]["cyclus_startdate"]["day"],$item["cyclus"]["cyclus_startdate"]["year"]));
      // Check if we have an enddate
      if($item["cyclus"]["cyclus_enddate"]["year"]!="" && $item["cyclus"]["cyclus_enddate"]["month"]!=0 && $item["cyclus"]["cyclus_enddate"]["day"]!=0)
      {
        $enddate = date("Y-m-d",mktime(12,0,0,$item["cyclus"]["cyclus_enddate"]["month"],$item["cyclus"]["cyclus_enddate"]["day"],$item["cyclus"]["cyclus_enddate"]["year"]));
      }
      else 
      {
        $enddate = mktime(12,0,0,12,31,2037);
      }
      
    }
    // Check if getDates has a range start/end date
    if($rangeStartDate!=NULL && $rangeEndDate!=NULL)
    {
      if($rangeStartDate>$enddate) return array();
      if($rangeStartDate>$startdate && $rangeStartDate<$enddate) $startdate = $rangeStartDate;
      if($rangeEndDate<$enddate && $rangeEndDate>$startdate) $enddate = $rangeEndDate;
    }

    $endtime = mktime(12,0,0,substr($enddate,5,2),substr($enddate,8,2),substr($enddate,0,4));
    atkdebug("LOOP FROM $startdate till $enddate");
    //return array();

    while(true)
    {

        $starttime = mktime(12,0,0,substr($startdate,5,2),substr($startdate,8,2)+$datecounter,substr($startdate,0,4));
        $unix_date = $starttime;
        atkdebug("Check: ".date("Ymd",$starttime)." > ".date("Ymd",$endtime));
        if(date("Ymd",$starttime)>date("Ymd",$endtime)) break;
        // Skip all months that we don't need for the yearly day/date events
        if(($item["recur"]=="yearly_day" && date("m",$starttime)!=$item["cyclus"]["startmonth"]) ||
           ($item["recur"]=="yearly_date" && !(date("m",$starttime)>= $item["cyclus"]["startmonth"] && date("m",$starttime)<=$item["cyclus"]["endmonth"]))
          )
        {
          $current_day = (date("d",$starttime)-1);
          $number_of_days = date("t",$starttime);
          $datecounter+=($number_of_days-$current_day);
          continue;
        }

        //echo date("Ymd",$starttime)."\n";
        $month_time = ceil(date("d",$starttime)/7);
        $day = date("j",$starttime);
        $month = date("n",$starttime);
        $year = date("Y",$starttime);
        $weekday_number = date("w",mktime(12,0,0,$month,$day,$year));
        //atkdebug("WEEK DAY NUMBER: $weekday_number");
        $weekday = schedulertools::weekdaynumbertoflag($weekday_number);
        //echo "Weekday $weekday_number is flag: ".$weekday."\n";

        // for everydate we need to loop true the recs
        // else we can't see if a rec is valid or not.
        $show_item = true;
          // Check weekday / monthtime / day / day+month
          switch($item["recur"])
          {
            case 'once':
              if(date("Y-m-d",$starttime)>=$startdate && date("Y-m-d",$starttime)<=$enddate) { $show_item = true; } else { $show_item = false; }
              break;
            case 'weekly':
              if(hasFlag(array_sum($item["cyclus"]["weekday"]),$weekday)) { $show_item = true; } else { $show_item = false; }
            break;
            case 'monthly_date':
              atkdebug("Day = $day | ".$item["cyclus"]["startday"]. " - ".$item["cyclus"]["endday"]);
              if($item["cyclus"]["startday"]<=$day && $item["cyclus"]["endday"]>=$day)  { $show_item = true; atkdebug("Show"); } else { $show_item = false; }
            break;
            case 'monthly_day':
              if(hasFlag(array_sum($item["cyclus"]["weekday"]),$weekday) && $item["cyclus"]["month_time"]==$month_time) { $show_item = true; } else { $show_item = false; }
            break;
            case 'yearly_date':
              if(($item["cyclus"]["startday"]<=$day && $item["cyclus"]["endday"]>=$day) && ($item["cyclus"]["startmonth"]<=$month && $item["cyclus"]["endmonth"]>=$month))  { $show_item = true; } else { $show_item = false; }
            break;
            case 'yearly_day':
              if($item["cyclus"]["startmonth"]==$month && hasFlag(array_sum($item["cyclus"]["weekday"]),$weekday) && $item["cyclus"]["month_time"]==$month_time) { $show_item = true; } else { $show_item = false; }
            break;
          }
          // Check the every ... (days,weeks,months,years)
          if($item["recur"]!="once" && isset($item["cyclus"]["every"]) && $item["cyclus"]["every"]>=1 && $show_item==true)
          {
       			//$cyclus_startdate_array = explode('-',$item["cyclus"]["cyclus_startdate"]);
      			//$cyclus_startdate_time = mktime(12,0,0,intval($item["cyclus"]["cyclus_startdate"]["month"]),intval($item["cyclus"]["cyclus_startdate"]["day"]),intval($item["cyclus"]["cyclus_startdate"]["year"]));
            $cyclus_startdate = mktime(12,0,0,$item["startdate"]["month"],$item["startdate"]["day"],$item["startdate"]["year"]);
            switch($item["recur"])
            {
              case 'daily':
                $interval = $unix_date - $cyclus_startdate_time;
  				    	$interval_days = floor($interval/(60*60*24));
  				    	$devided = $interval_days/$item["cyclus"]["every"];
  					    $match = (int) $devided;
  				    	if ($devided != $match)
  				    	{
  					    	$show_item = false;
  					    }
  				      break;
  				    case 'weekly':
                $interval = $unix_date - $cyclus_startdate_time;
  				    	$interval_weeks = floor($interval/(60*60*24*7));
  				    	$devided = $interval_weeks/$item["cyclus"]["every"];

  					    $match = (int) $devided;
  				    	if ($devided != $match)
  				    	{
  					    	$show_item = false;
  					    }
  				      break;
  				    case 'monthly_date':
  				    case 'monthly_day':
      					//$start_month = $item["cyclus"]["cyclus_startdate"]["month"];
      					$start_month = $item["startdate"]["month"];
      					$this_month = strftime('%m', $unix_date);
      					$interval = $this_month - $start_month;
      					$devided = $interval/$item["cyclus"]["every"];
      					$match = (int) $devided;
      					if ($devided != $match)
      					{
      						$show_item= false;
      					}
      				  break;

      				case 'yearly_date':
      				case 'yearly_day':
      				  //$start_year = $item["cyclus"]["cyclus_startdate"]["year"];
      					$start_year = $item["startdate"]["year"];
      					$this_year = strftime('%Y', $unix_date);
      					$interval = $this_year - $start_year;
      					$devided = $interval/$item["cyclus"]["every"];
      					$match = (int) $devided;
      					if ($devided != $match)
      					{
      						$show_item= false;
      					}
      				  break;
            }
          }
          //atkdebug("Startdate item (".$item["startdate"].") == ".$this->m_viewdate);
          if($show_item)
          {
            $dates[] = date("Y-m-d",$starttime);
          }
          $datecounter++;
          if($item["recur"]=="once") break;
        }
        return $dates;
      }





}

?>