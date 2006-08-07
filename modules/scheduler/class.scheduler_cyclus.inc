<?php
  userelation("atkonetomanyrelation");

  class scheduler_cyclus extends atkNode
  {
     function scheduler_cyclus()
     {
       $this->atkNode("scheduler_cyclus");
       $this->add(new atkAttribute("scheduler_id",AF_PRIMARY|AF_HIDE));
       //$tmp_recur = &new atkRadioAttribute("recur",array("once","daily","weekly","monthly_date","monthly_day","yearly_date","yearly_day"),array());
       //$tmp_recur->addOnChangeHandler("change_recur(newvalue);");
       //$this->add($tmp_recur,"cyclus");


       //$this->add(new atkDateAttribute("cyclus_startdate"  , "F d Y","d F Y", 0, 0),"cyclus");
       $this->add(new atkDateAttribute("cyclus_enddate"    , "F d Y","d F Y", 0, 0),"cyclus");
       $this->add(new atkListAttribute("startday",$day_arr,$day_arr,AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");
       $this->add(new atkListAttribute("endday",$day_arr,$day_arr,AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");
       $this->add(new atkListAttribute("startmonth",$month_arr_disp,$month_arr_value,AF_OBLIGATORY),"cyclus");
       $this->add(new atkListAttribute("endmonth",$month_arr_disp,$month_arr_value,AF_OBLIGATORY),"cyclus");

       $this->add(new atkListAttribute("every",array(1,2,3,4,5,6,7,8,9,10),array(1,2,3,4,5,6,7,8,9,10),AF_OBLIGATORY|AF_NO_TRANSLATION),"cyclus");

       $this->add(new atkListAttribute("month_time",array("first","second","third","fourth"),array(1,2,3,4),AF_OBLIGATORY),"cyclus");
       $this->add(new atkFlagAttribute("weekday",array("sunday","monday","thuesday","wednedsday","thursday","friday","saturday"),array(1,2,4,8,16,32,64)),"cyclus");
       $this->add(new atkOneToManyRelation("exceptions","scheduler.scheduler_cyclus_not","scheduler_id"));

       $this->setTable("scheduler_cyclus");
     }

     function every_edit($record="", $fieldprefix="",$mode)
     {
      $attrib = $this->getAttribute("every");
      $ret = $attrib->edit($record,$fieldprefix,$mode);
      $ret.= '&nbsp;<span id="every_day">day(s)</span><span id="every_week">week(s)</span><span id="every_month">month(s)</span><span id="every_year">year(s)</span>';

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