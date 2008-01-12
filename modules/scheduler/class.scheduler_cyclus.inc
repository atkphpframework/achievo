<?php
  userelation("atkonetomanyrelation");
  useattrib("atkflagattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");

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
       $this->add(new atkDummyAttribute('recur_info','',AF_HIDE_ADD|AF_HIDE_EDIT|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_SELECT),'cyclus');
       $this->add(new atkListAttribute("monthly_day",$day_arr,$day_arr,AF_LIST_NO_NULL_ITEM|AF_NO_TRANSLATION|AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkListAttribute("yearly_day",$day_arr,$day_arr,AF_LIST_NO_NULL_ITEM|AF_NO_TRANSLATION|AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkListAttribute("yearly_month",$month_arr_disp,$month_arr_value,AF_LIST_NO_NULL_ITEM|AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkListAttribute("yearly_month2",$month_arr_disp,$month_arr_value,AF_LIST_NO_NULL_ITEM|AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkNumberAttribute("daily_every",AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkNumberAttribute("weekly_every",AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkNumberAttribute("monthly_every",AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkNumberAttribute("monthly_every2",AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkListAttribute("monthly_month_time",array("first","second","third","fourth"),array(1,2,3,4),AF_LIST_NO_NULL_ITEM|AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkListAttribute("yearly_month_time",array("first","second","third","fourth"),array(1,2,3,4),AF_LIST_NO_NULL_ITEM|AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkFlagAttribute("weekly_weekday",array("sunday","monday","tuesday","wednesday","thursday","friday","saturday"),array(1,2,4,8,16,32,64),'',AF_HIDE),"cyclus")->setManualPaint(true); 	 
       $this->add(new atkListAttribute('monthly_weekday_list',array("sunday","monday","tuesday","wednesday","thursday","friday","saturday"),array(0,1,2,3,4,5,6),AF_LIST_NO_NULL_ITEM|AF_HIDE),'cyclus')->setManualPaint(true);
       $this->add(new atkListAttribute('yearly_weekday_list',array("sunday","monday","tuesday","wednesday","thursday","friday","saturday"),array(0,1,2,3,4,5,6),AF_LIST_NO_NULL_ITEM|AF_HIDE),'cyclus')->setManualPaint(true);
       $this->add(new atkRadioAttribute('daily_choice',array(),array(),AF_DISPLAY_VERT|AF_NO_LABEL|AF_NO_TRANSLATION|AF_HIDE_VIEW),'cyclus');
       $this->add(new atkDummyAttribute('weekly_choice','',AF_NO_LABEL|AF_HIDE_VIEW),'cyclus');
       $this->add(new atkRadioAttribute('monthly_choice',array(),array(),AF_DISPLAY_VERT|AF_NO_LABEL|AF_NO_TRANSLATION|AF_HIDE_VIEW),'cyclus');
       $this->add(new atkRadioAttribute('yearly_choice',array(),array(),AF_DISPLAY_VERT|AF_NO_LABEL|AF_NO_TRANSLATION|AF_HIDE_VIEW),'cyclus');
       $this->add(new atkRadioAttribute('end_choice',array(),array(),AF_DISPLAY_VERT|AF_NO_TRANSLATION|AF_HIDE_VIEW),'cyclus');
       $this->add(new atkDateAttribute("cyclus_enddate", "F d Y","d F Y", 0, 0,AF_HIDE),"cyclus")->setManualPaint(true);
       $this->add(new atkNumberAttribute('cyclus_times',AF_HIDE),'cyclus')->setManualPaint(true);
       $this->add(new atkOneToManyRelation("exceptions","scheduler.scheduler_cyclus_not","scheduler_id"),"cyclus");

       $this->setTable("scheduler_cyclus");
       $this->setSecurityAlias("scheduler.scheduler");
     }
     

     
     /**
      * Custom yearly choice edit
      *
      * @param array $record Record
      * @param string $fieldprefix Fieldprefix
      * @param string $mode Mode
      * @return string Edit for yearly choice
      */
     function yearly_choice_edit($record="",$fieldprefix="",$mode)
     {
       $attrib = &$this->getAttribute("yearly_choice");
       $attrib->setClickableLabel(false);
       $month = &$this->getAttribute("yearly_month");
       $day = &$this->getAttribute("yearly_day");
       $choice = atktext('every','scheduler').' '.$month->edit($record,$fieldprefix,$mode).' ';
       $choice.= $day->edit($record,$fieldprefix,$mode);       
       $attrib->addOption($choice,1);
       $month_time = &$this->getAttribute("yearly_month_time");
       $weekday_list = &$this->getAttribute("yearly_weekday_list");
       $month = &$this->getAttribute("yearly_month2");
       $choice = atktext('the','scheduler').' '.$month_time->edit($record,$fieldprefix,$mode);
       $choice.= $weekday_list->edit($record,$fieldprefix,$mode).' '.atktext('of','scheduler').' ';
       $choice.= $month->edit($record,$fieldprefix,$mode);
       $attrib->addOption($choice,2);
       return $attrib->edit($record,$fieldprefix,$mode);
     }
     
     /**
      * Custom monthly choice edit
      *
      * @param array $record Record
      * @param string $fieldprefix Fieldprefix
      * @param string $mode Mode
      * @return string Edit for monthly choice
      */
     function monthly_choice_edit($record="",$fieldprefix="",$mode)
     {
       $attrib = &$this->getAttribute("monthly_choice");
       $attrib->setClickableLabel(false);
       $day = &$this->getAttribute("monthly_day");
       $every = &$this->getAttribute("monthly_every");
       $choice = atktext('day','scheduler').' '.$day->edit($record,$fieldprefix,$mode).' ';
       $choice.= atktext('of_every','scheduler').' '.$every->edit($record,$fieldprefix,$mode).' '.atktext('months','scheduler');       
       $attrib->addOption($choice,1);
       $month_time = &$this->getAttribute("monthly_month_time");
       $weekday_list = &$this->getAttribute("monthly_weekday_list");
       $month_every = &$this->getAttribute("monthly_every2");
       $choice = atktext('the','scheduler').' '.$month_time->edit($record,$fieldprefix,$mode);
       $choice.= $weekday_list->edit($record,$fieldprefix,$mode).' '.atktext('of_every','scheduler').' ';
       $choice.= $month_every->edit($record,$fieldprefix,$mode).' '.atktext('months','scheduler');
       $attrib->addOption($choice,2);
       return $attrib->edit($record,$fieldprefix,$mode);
     }
     
     /**
      * Custom weekly choice edit
      *
      * @param array $record Record
      * @param string $fieldprefix Fieldprefix
      * @param string $mode Mode
      * @return string Edit for weekly choice
      */
     function weekly_choice_edit($record="",$fieldprefix="",$mode)
     {
       $attrib = &$this->getAttribute("weekly_choice");
       $every = &$this->getAttribute("weekly_every");
       $weekday = &$this->getAttribute("weekly_weekday");
       $text = atktext('every','scheduler').' '.$every->edit($record,$fieldprefix,$mode).' '.atktext('weeks','scheduler').' '.atktext('on','scheduler').'<br>';
       $text.= $weekday->edit($record,$fieldprefix,$mode);
       $attrib->setText($text);
       return $attrib->edit($record,$fieldprefix,$mode);
     }
     
     /**
      * Custom daily choice edit
      *
      * @param array $record Record
      * @param string $fieldprefix Fieldprefix
      * @param string $mode Mode
      * @return string Edit for daily choice
      */
     function daily_choice_edit($record="",$fieldprefix="",$mode)
     {
       $attrib = &$this->getAttribute("daily_choice");
       $attrib->setClickableLabel(false);
       $every = &$this->getAttribute("daily_every");
       $attrib->addOption(atktext('every','scheduler').' '.$every->edit($record,$fieldprefix,$mode).' '.atktext('days','scheduler'),1);
       $attrib->addOption(atktext("every_workday",'scheduler'),2);
       return $attrib->edit($record,$fieldprefix,$mode);
     }
          
     /**
      * Custom end choice edit
      *
      * @param array $record Record
      * @param string $fieldprefix Fieldprefix
      * @param string $mode Mode
      * @return string Edit for end choice
      */
     function end_choice_edit($record="",$fieldprefix="",$mode)
     {
       $attrib = &$this->getAttribute("end_choice");
       $attrib->setClickableLabel(false);
       
       $attrib->addOption(atktext("no_enddate",'scheduler'),1);
       
       $enddate = &$this->getAttribute("cyclus_enddate");
       $attrib->addOption(atktext('ends_on','scheduler').' '.$enddate->edit($record,$fieldprefix,$mode),2);

       $times = &$this->getAttribute("cyclus_times");
       $attrib->addOption(atktext('ends_after','scheduler').' '.$times->edit($record,$fieldprefix,$mode).' '.atktext('times','scheduler'),3);
       return $attrib->edit($record,$fieldprefix,$mode);
     }
     
     
     /**
      * Display the recurring info
      *
      * @param array $record Record
      * @param string $mode Mode
      * @return string String with the recurring info
      */
     function recur_info_display($record,$mode)
     {
       $ret='';
         
       $ret.=$this->getDailyInfo($record,$mode);
       $ret.=$this->getWeeklyInfo($record,$mode);
       $ret.=$this->getMonthlyInfo($record,$mode);
       $ret.=$this->getYearlyInfo($record,$mode);
       if($ret!='') $ret.=', '.$this->getEndDateInfo($record,$mode);
       return $ret;
     }
    
     /**
      * Custom validate for all choices
      *
      * @param array $record Record
      * @param string $mode Mode
      * @param array $ignoreList Ignorelist
      * @return boolean Validated or not
      */
     function validate(&$record, $mode, $ignoreList = array())
     {
       global $ATK_VARS;
       // Add extra obligatory flags when recur is not once
       $valid_recur_types = array("daily","weekly","monthly","yearly");
       $recur = $ATK_VARS['recur'];
       atk_var_dump($record,"CYCLE RECORD");
       atkdebug("<h1>VALIDATE scheduler cycle - $recur</h1>");

       if(in_array($recur,$valid_recur_types))
       {
         switch($recur)
         {
           case 'daily':   if($record['daily_choice']==1)
                           {
                             $this->makeObligatory('daily_every');
                           }
                           break;
           case 'weekly':  $this->makeObligatory('weekly_weekday','weekly_every');
                           break;
           case 'monthly': if($record['monthly_choice']==1)
                           {
                             $this->makeObligatory(array('monthly_day','monthly_every'));
                           }
                           else 
                           {
                             $this->makeObligatory('monthly_month_time','monthly_weekday_list','monthly_every2');
                           }
                           break;
           case 'yearly':  if($record['yearly_choice']==1)
                           {
                             $this->makeObligatory('yearly_month', 'yearly_day');
                           }
                           else 
                           {
                             $this->makeObligatory('yearly_month_time','yearly_weekday_list','yearly_month2');
                           }
                           break;
         }
         if($record['end_choice']==2)
         {
            $this->makeObligatory('cyclus_enddate');           
         }
         else if($record['end_choice']==3)
         {
            $this->makeObligatory('cyclus_times');           
         }
         
       }
        
       // Call the original validate method
       return parent::validate($record, $mode, $ignoreList);
     }
     
     /**
      * Make attribute Obligatory
      *
      * @param string $attribname Attribute name
      */
     function makeObligatory($attrs)
     {
       
       if (func_num_args() > 1)
       {
         $attrs = func_get_args();
       }
       else if (!is_array($attrs))
         $attrs = array($attrs);
        foreach ($attrs as $attr)
           $this->getAttribute($attr)->addFlag(AF_OBLIGATORY);
     }
     
     
     /**
      * Pre add function that will clear all the fields that are not used
      *
      * @param array $rec Record
      * @return boolean
      */
     function preAdd(&$rec)
     {
       global $ATK_VARS;
       $valid_recur_types = array("once","daily","weekly","monthly","yearly");
       $recur = $ATK_VARS['recur'];
       if(in_array($recur,$valid_recur_types))
       {
         switch($recur)
         {
           case 'daily':  if($rec['daily_choice']==1)
                          { 
                            // Don't clear: daily_every
                            $clearfields = array('monthly_day','yearly_day','yearly_month','yearly_month2','weekly_every','monthly_every','monthly_every2',
                                                 'monthly_month_time','yearly_month_time','weekly_weekday','monthly_weekday_list','yearly_weekday_list',
                                                 'monthly_choice','yearly_choice');
                          }
                          else 
                          {
                            // Clear everything
                            $clearfields = array('monthly_day','yearly_day','yearly_month','yearly_month2','daily_every','weekly_every','monthly_every','monthly_every2',
                                               'monthly_month_time','yearly_month_time','weekly_weekday','monthly_weekday_list','yearly_weekday_list',
                                               'monthly_choice','yearly_choice');
                          }
                          break;
           case 'weekly': // Don't clear: weekly_weekday, weekly_every
                          $clearfields = array('monthly_day','yearly_day','yearly_month','yearly_month2','daily_every','monthly_every','monthly_every2',
                                               'monthly_month_time','yearly_month_time','monthly_weekday_list','yearly_weekday_list',
                                               'daily_choice','monthly_choice','yearly_choice');
                          break;
           case 'monthly':if($rec['monthly_choice']==1)
                          { 
                            // Don't clear: monthly_day, monthly_every
                            $clearfields = array('yearly_day','yearly_month','yearly_month2','daily_every','weekly_every','monthly_every2',
                                               'monthly_month_time','yearly_month_time','weekly_weekday','monthly_weekday_list','yearly_weekday_list',
                                               'daily_choice','yearly_choice');
                          }
                          else 
                          {
                            // Don't clear: monthly_month_time, monthly_weekday_list, monthly_every2
                            $clearfields = array('monthly_day','yearly_day','yearly_month','yearly_month2','daily_every','weekly_every','monthly_every',
                                                 'yearly_month_time','weekly_weekday','yearly_weekday_list',
                                                 'daily_choice','yearly_choice');
                          }
                          break;
           case 'yearly': if($rec['yearly_choice']==1)
                          { 
                            // Don't clear: yearly_month, yearly_day
                            $clearfields = array('monthly_day','yearly_month2','daily_every','weekly_every','monthly_every','monthly_every2',
                                               'monthly_month_time','yearly_month_time','weekly_weekday','monthly_weekday_list','yearly_weekday_list',
                                               'daily_choice','monthly_choice');
                          }
                          else 
                          {
                            // Don't clear: yearly_month_time, yearly_weekday_list, yearly_month2
                            $clearfields = array('monthly_day','yearly_day','yearly_month','daily_every','weekly_every','monthly_every','monthly_every2',
                                               'monthly_month_time','weekly_weekday','monthly_weekday_list',
                                               'daily_choice','monthly_choice');
                          }
                          break;
           default :      // Clear everything
                          $clearfields = array('monthly_day','yearly_day','yearly_month','yearly_month2','daily_every','weekly_every','monthly_every','monthly_every2',
                                               'monthly_month_time','yearly_month_time','weekly_weekday','monthly_weekday_list','yearly_weekday_list',
                                               'daily_choice','monthly_choice','yearly_choice','endchoice','cyclus_enddate','cyclus_times');
                          break;
                       
         }
         
         array_shift($valid_recur_types);
         if(in_array($recur,$valid_recur_types))
         {
           switch($rec['end_choice'])
           {
             case 1: $clearfields[]='cyclus_enddate';
                     $clearfields[]='cyclus_times';
                     break;
             case 2: $clearfields[]='cyclus_times';
                     break;
             case 3: $clearfields[]='cyclus_enddate';
                     break;
           }
         }
         $this->emptyFields($rec,$clearfields);
       }
       return true;
     }
     
     /**
      * Empty fields  
      *
      * @param array $rec Current record
      * @param array $fields Array with fields to empty
      */
     function emptyFields(&$rec,$fields)
     {
       foreach($fields as $field)
       {
         if(array_key_exists($field,$rec))
         {
           $rec[$field]=null;
         }
       }
     }

     /**
      * Initial values
      *
      * @return array Array with initial values
      */
     function initial_values()
     {
      $initial=array();
      $initial["weekly_weekday"]=0;
      $initial['daily_choice']=1;
      $initial['monthly_choice']=1;
      $initial['yearly_choice']=1;
      $initial['end_choice']=1;
      $initial['cyclus_enddate']=array('day'=>date("d"),
                                       'month'=>date("m"),
                                       'year'=>date("Y"));
      return $initial;
     }
     
     /**
      * Return daily information
      *
      * @param array $record Record
      * @return string daily information
      */
     function getDailyInfo($record,$mode)
     {
       $ret = "";
       switch($record['daily_choice'])
       {
         case 1: $every = &$this->getAttribute("daily_every");
                 $ret=atktext('info_every','scheduler').' '.$every->display($record,$mode).' '.atktext('days','scheduler');
                 break;
         case 2: $ret=atktext("every_workday",'scheduler');
                 break;
       }
       return $ret;
     }
     
     /**
      * Return weekly information
      *
      * @param array $record Record
      * @return string weekly information
      */
     function getWeeklyInfo($record,$mode)
     {
       $ret = '';
       switch($record['weekly_choice'])
       {
         case 1: $every = &$this->getAttribute("weekly_every");
                 $ret=atktext('info_every','scheduler').' '.$every->display($record,$mode).' '.atktext('weeks','scheduler');
                 break;
         case 2: $weekdays = &$this->getAttribute("weekly_weekdays");
                 $ret.=atktext('weekly','scheduler').' '.atktext('on',"scheduler").' '.$weekdays->display($record,$mode);
                 break;
       }
       return $ret;
     }
     
     /**
      * Return monthly information
      *
      * @param array $record Record
      * @return string monthly information
      */
     function getMonthlyInfo($record,$mode)
     {
       $ret='';
       switch($record['monthly_choice'])
       {
         case 1:  $day = &$this->getAttribute("monthly_day");
                  $every = &$this->getAttribute("monthly_every");
                  $ret = atktext('day','scheduler').' '.$day->display($record,$mode).' ';
                  $ret.= atktext('of_every','scheduler').' '.$every->display($record,$mode).' '.atktext('months','scheduler');
                  break;
         case 2:  $month_time = &$this->getAttribute("monthly_month_time");
                  $weekday_list = &$this->getAttribute("monthly_weekday_list");
                  $month_every = &$this->getAttribute("monthly_every2");
                  $ret = atktext('the','scheduler').' '.$month_time->display($record,$mode);
                  $ret.= $weekday_list->display($record,$mode).' '.atktext('of_every','scheduler').' ';
                  $ret.= $month_every->display($record,$mode).' '.atktext('months','scheduler');
                  break;    
       }
       return $ret;
     }
     
     /**
      * Return yearly information
      *
      * @param array $record Record
      * @return string yearly information
      */
     function getYearlyInfo($record,$mode)
     {
       $ret='';
       switch($record['yearly_choice'])
       {
         case 1: $month = &$this->getAttribute("yearly_month");
                 $day = &$this->getAttribute("yearly_day");
                 $ret = atktext('every','scheduler').' '.$month->display($record,$mode).' ';
                 $ret.= $day->display($record,$mode);     
                 break;
         case 2: $month_time = &$this->getAttribute("yearly_month_time");
                 $weekday_list = &$this->getAttribute("yearly_weekday_list");
                 $month = &$this->getAttribute("yearly_month2");
                 $ret= atktext('the','scheduler').' '.$month_time->display($record,$mode);
                 $ret.= $weekday_list->display($record,$mode).' '.atktext('of','scheduler').' ';
                 $ret.= $month->display($record,$mode);
                 break;
       }
       return $ret;
     }
     
     /**
      * Return enddate information
      *
      * @param array $record Record
      * @return string enddate information
      */
     function getEndDateInfo($record,$mode)
     {
       $ret="";
       switch($record['end_choice'])
       {
         case 1: $ret=atktext("no_enddate",'scheduler');
                 break;
         case 2: $ret=atktext('ends_on','scheduler').' ';
                 $enddate = &$this->getAttribute("cyclus_enddate");
                 $ret.=$enddate->display($record,$mode);
                 break;
         case 3: $ret=atktext('ends_after','scheduler').' ';
                 $times = &$this->getAttribute("cyclus_times");
                 $ret.=$times->display($record,$mode).' '.atktext('times','scheduler');
                 break;
         default:$ret="-";
       }
       return $ret;
     }
  }

?>
