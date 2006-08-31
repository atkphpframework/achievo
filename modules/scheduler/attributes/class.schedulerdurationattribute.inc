<?php

class schedulerDurationAttribute extends atkAttribute
{
  
   function schedulerDurationAttribute($name,$flags=0,$size=0)
   {
     $this->atkAttribute($name,$flags, $size);
   }

   function edit($rec="",$fieldprefix="")
   {
     $id = $fieldprefix.$this->formName();
     $fieldvalue = $this->_minutes2array(atkArrayNvl($rec, $this->fieldName(), 0));
     atk_var_dump($fieldvalue,"SCHEDULERDURATIONATTRIBUTE FIELDVALUE");
     
     $result = "";
     $result .= '<select id="'.$id.'_days" name="'.$fieldprefix.$this->formName().'[days]">';
     $this->registerKeyListener($id.'_days', KB_CTRLCURSOR|KB_LEFTRIGHT); 
     for($i=0;$i<9;$i++)
     {
       $result .= '<option value="'.$i.'" ';
       if ($fieldvalue["days"]==$i) $result.= "selected";
       $result .='>'.$i.' '.atktext('days','atk');
     }
     $result .= '</select>';
     
     $result .= '&nbsp;<select id="'.$id.'_hours" name="'.$fieldprefix.$this->formName().'[hours]">';
     $this->registerKeyListener($id.'_hours', KB_CTRLCURSOR|KB_LEFTRIGHT); 
     for($i=0;$i<24;$i++)
     {
       $result .= '<option value="'.$i.'" ';
       if ($fieldvalue["hours"]==$i) $result.= "selected";
       $result .='>'.$i.' '.atktext('hours','atk');
     }
     $result .= '</select>';    

     $result .= '&nbsp;<select id="'.$id.'_minutes" name="'.$fieldprefix.$this->formName().'[minutes]">';
     $this->registerKeyListener($id.'_minutes', KB_CTRLCURSOR|KB_LEFTRIGHT);
     for($i=0;$i<60;$i+=5)
     {
       $result .= '<option value="'.$i.'" ';
       if ($fieldvalue["minutes"]==$i) $result.= "selected";
       $result .='>'.$i.' '.atktext('minutes','atk');
     }
     $result .= '</select>';
     return $result;
   }
   
   
   /**
    * Function display's the time
    * @param $record Array with time
    * @return String with YYYY-MM-DD
    */
   function display($rec)
   {
     return $this->_minutes2string($rec[$this->fieldName()]);
   }
   
   
   function getSearchModes()
   {
     return array();
   }

   function dbFieldType()
   {
     return "number";
   }
   
   function _array2minutes($value)
   {
     return (($value["days"]*1440)+($value["hours"]*60)+$value["minutes"]);   
   }
   
   function _minutes2array($minutes)
   {
     $ret = array("days"=>0,
                  "hours"=>0,
                  "minutes"=>0);
     if($minutes <1) return $ret;
     
     if($minutes>=1440)
     {
       $ret["days"]=floor($minutes/1440);
       $minutes = $minutes-($ret["days"]*1440);
     }
     if($minutes>=60)
     {
       $ret["hours"] = floor($minutes/60);
       $minutes = $minutes-($ret["hours"]*60);
     }
     $ret["minutes"]=$minutes;
     return $ret;
   }
   
   function _minutes2string($minutes)
   {
     $array = $this->_minutes2array($minutes);
     return $array["days"]." ".atktext("days")." ".$array["hours"]." ".atktext("hours")." ".$array["minutes"]." ".atktext("minutes");
   }
   
   
  /**
   * Fetch values
   * @param $rec Array with values
   * @return String without slashes
   */
  function fetchValue($rec)
  {
    return $this->_array2minutes($rec[$this->fieldName()]);
  }

}

?>