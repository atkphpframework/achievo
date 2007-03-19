<?php
  userelation("atkonetomanyrelation");
  useattrib("atkweekdayattribute");

  class scheduler_cyclus extends atkNode
  {
     function scheduler_cyclus()
     {
       atkimport("modules.utils.dateutil");
       $month_array = dateutil::monthlist();
       $month_arr_value = array_keys($month_array);
       $month_arr_disp = array_values($month_array);

       $day_arr = array();
       for($i=1;$i<32;$i++) $day_arr[]=$i;

       $this->atkNode("scheduler_cyclus");
       $this->add(new atkAttribute("scheduler_id",AF_PRIMARY|AF_HIDE));

       $this->add(new atkDateAttribute("cyclus_enddate"    , "F d Y","d F Y", 0, 0),"cyclus");
       $this->add(new atkListAttribute("startday",$day_arr,$day_arr,AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");
       $this->add(new atkListAttribute("endday",$day_arr,$day_arr,AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");
       $this->add(new atkListAttribute("startmonth",$month_arr_disp,$month_arr_value,AF_OBLIGATORY),"cyclus");
       $this->add(new atkListAttribute("endmonth",$month_arr_disp,$month_arr_value,AF_OBLIGATORY),"cyclus");

       $this->add(new atkListAttribute("every",array("none",2,3,4,5,6,7,8,9,10),array(1,2,3,4,5,6,7,8,9,10),AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");

       $this->add(new atkListAttribute("month_time",array("first","second","third","fourth"),array(1,2,3,4),AF_OBLIGATORY),"cyclus");
       $this->add(new atkWeekdayAttribute("weekday"),"cyclus");
       $this->add(new atkOneToManyRelation("exceptions","scheduler.scheduler_cyclus_not","scheduler_id"),"cyclus");

       $this->setTable("scheduler_cyclus");
     }

     function every_edit($record="", $fieldprefix="",$mode)
     {
      $attrib = $this->getAttribute("every");
      $ret = $attrib->edit($record,$fieldprefix,$mode);
      $ret.= '&nbsp;<span id="every_day">days</span><span id="every_week">weeks</span><span id="every_month">months</span><span id="every_year">years</span>';

      return $ret;
     }

     function initial_values()
     {
      $initial=array();
      $initial["recur"]="once";
      $initial["startday"]=1;
      $initial["endday"]=1;
      $initial["startmonth"]=1;
      $initial["endmonth"]=1;
      $initial["every"]=1;
      $initial["month_time"]=1;
      $initial["weekday"]=0;

      return $initial;
     }
  }

?>