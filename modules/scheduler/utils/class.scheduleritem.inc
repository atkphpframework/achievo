<?php

class schedulerItem
{
  var $scheduler_model;
  var $scheduler_cyclus;
  
  function schedulerItem()
  {
    $this->scheduler_model= &atkinstance("module.scheduler.scheduler_model");
    $this->scheduler_cyclus = atkGetNode("scheduler.scheduler_cyclus");
  }
  
  function &getInstance()
  {
    static $s_instance = null;
    if (is_null($s_instance))
    {
      $s_instance = new schedulerItem();
    }
    return $s_instance;
  }
  
  /**
  * Renders a calendar items
  * @param array $item Calendar item
  * @param bool $admin Show admin links or not
  * @return string HTML code with a renderd calendar item
  */
  function render($item,$admin=false,$itemDate="",$showcategory=false)
  {
    atk_var_dump($item,"RENDER ITEM");
    $theme = &atktheme::getInstance();
    if(!is_array($item)) return "";
    
    if($itemDate=="") $itemDate=$this->scheduler_model->m_viewdate;
    $tmp="";

    // If showcategory we will show a box with the category color
    if($showcategory)
    {
      $tmp.=$this->getCategory($item);
    }

    if($this->isBirthday($item))
    {
      $tmp.='<img src="'.$theme->imgPath("cake.gif","scheduler").'" width=16 height=16 title="'.atktext("birthday","scheduler").'" alt="'.atktext("birthday","scheduler").'">';
    }
    if($this->isTodo($item))
    {
      $tmp.='<img src="'.$theme->imgPath("todo.gif","scheduler").'" width=16 height=16 title="'.atktext("todo","todo").'" alt="'.atktext("todo","todo").'">';
    }

    if(!$this->isAllDay($item))
    {
      $tmp.=substr($item["starttime"],0,5)." ".substr($item["endtime"],0,5)." ";
    }

    if($this->hasDescription($item))
       $tmp.='<img src="'.$theme->imgPath("note.gif","scheduler").'" width=16 height=16 title="'.atktext("note").'" alt="'.atktext("note").'">';
    if($this->isRecurring($item))
    { 
      $title= $this->scheduler_cyclus->recur_info_display($item['id']);
      $tmp.='<img src="'.$theme->imgPath("recurring.gif","scheduler").'" width=16 height=16 title="'.atkText("recurring_event","scheduler").' - '.$title.'" alt="'.atkText("recurring_event","scheduler").' - '.$title.'">';
    }
    if($this->hasAlarm($item))
      $tmp.='<img src="'.$theme->imgPath("alarm.gif","scheduler").'" width=16 height=16 title="'.atkText("reminder_event","scheduler").'" alt="'.atkText("reminder_event","scheduler").'">';
    if($this->isPrivate($item))
      $tmp.='<img src="'.$theme->imgPath("private.gif","scheduler").'" width=16 height=16 title="'.atkText("private_event").'" alt="'.atkText("private_event","scheduler").'">';
    $isAttendee = false;
    $tmp.=$this->getAttendees($item,$isAttendee);

    $tmp.="&nbsp;";

    /*
    owner = view private, edit, delete, view
    attendee = view private, view
    assistant = edit, delete, view
    */

    // If item is private and current is not an attendee or owner, show private event.
    $tmp.=$this->getDescription($item,$isAttendee,$showcategory);
    $tmp.="<br>";
    // Add Admin links
    if($admin && !$this->isBirthday($item) && !$this->isTodo($item))
    {
      $tmp = $this->addAdminLinks($tmp,$item,$showcategory);
    }

    return $tmp;
  }
  
  function addAdminLinks($content,$item,$showcategory)
  {
    $user = atkGetUser();
    $theme = &atktheme::getInstance();
    $itemColors = $this->scheduler_model->getLegendColors();
    $assistantFor = schedulertools::assistantFor($user["id"]);
    $tmp='';
     $tmp .= '<table class="scheduleritem" border="0" cellpadding="0" cellspacing="0"><tr>'."\n";
      $tmp .= '<td valign="top" align="left" style="color: '.($showcategory?"#000000":$itemColors[$item["category"]]["fore"]).';">';
      $tmp.=$content;
      $tmp.='</td><td valign="top" align="right">';
      // you may only delete a calendar item if you are the owner,
      // or an assistant of the owner and the item is not private
      if ($item["owner"] == $user["id"] || (!$this->isPrivate($item) && in_array($item["owner"],$assistantFor)))
      {
        $delete = '<img src="'.$theme->iconPath("delete", "recordlist").'" border="0">';
        $tmp.= href(dispatch_url("scheduler.scheduler","delete",array("atkselector"=>"scheduler_scheduler.id=".$item["id"],"scheduler_id"=>$item["id"],"itemdate"=>$item["startdate"])),
                    $delete, SESSION_NESTED);

        $copy = '<img src="'.$theme->iconPath("copy", "recordlist").'" border="0">';
        $tmp.= href(dispatch_url("scheduler.scheduler","editcopy",array("atkselector"=>"scheduler_scheduler.id=".$item["id"],"scheduler_id"=>$item["id"],"itemdate"=>$item["startdate"], "call_original_editcopy"=>true)),
                    $copy, SESSION_NESTED);      
      }
      $tmp.='</td></tr></table>';
      return $tmp;
      
  }
  
  function getAttendees($item,&$isAttendee)
  {
    $theme = &atktheme::getInstance();
    $user = atkGetUser();
    // Get Attendees of the Item
    if ($item["all_users"])
    {
      $names=atkText("allusers","scheduler");
      $attendees='<img src="'.$theme->imgPath("attendees.gif","scheduler").'" width=16 height=16 title="'.$names.'" alt="'.$names.'">';
    }
    elseif(!$this->isBirthDay($item) && !$this->isTodo($item))
    {
      // Also check if the current user is an attendee
      //$attendees_rows = $this->getAttendees($item["id"]);
      $numberofattendees = count($this->scheduler_model->m_attendees[$item["id"]]);
      atk_var_dump($this->scheduler_model->m_attendees,"ATTENDEES ".$item["id"]);
      $names = "";
      for($i=0;$i<$numberofattendees;$i++)
      {

        $userid = $this->scheduler_model->m_attendees[$item["id"]][$i];
        if($userid==$user["id"]) $isAttendee=true;
        if($i>0) $names.=", ";
        $names.= $this->scheduler_model->m_users[$userid]["firstname"]." ".$this->scheduler_model->m_users[$userid]["lastname"].($this->scheduler_model->m_users[$userid]["role"]!="employee"?" (".$this->scheduler_model->m_users[$userid]["role"].")":"");
      }
      
      if($numberofattendees==1)
      {
        $attendees='<img src="'.$theme->imgPath("attendee.gif","scheduler").'" width=16 height=16 title="'.$names.'" alt="'.$names.'">';
      }
      else
      {
        $attendees='<img src="'.$theme->imgPath("attendees.gif","scheduler").'" width=16 height=16 title="'.$names.'" alt="'.$names.'">';
      }
    }
    else 
    {
      $attendees='';
    }
    return $attendees;
  }
  
  function getDescription($item,$isAttendee,$showcategory)
  {
    /*
    owner = view private, edit, delete, view
    attendee = view private, view
    assistant = edit, delete, view
    */
    $tmp = "";
    $user = getUser();
    $itemColors = $this->scheduler_model->getLegendColors();
    $assistantFor = schedulertools::assistantFor($user["id"]);
    // If item is private and current is not an attendee or owner, show private event.
    if($this->isPrivate($item) && !($item["owner"]==$user["id"] || $isAttendee))
    {
      $tmp.=atkText("private_event","scheduler");
    }
    else
    {
      $action = "";
      if((array_key_exists("owner",$item) && $item["owner"]==$user["id"]) ||
         (!$this->isPrivate($item) && (array_key_exists("owner",$item) && in_array($item["owner"],$assistantFor) || $this->allowed("all_non_private"))))
      {
        $action = ($this->isRecurring($item)?"editserieorsingle":"edit");
      }
      elseif($isAttendee)
      {
        $action = "view";
      }

      if($action!="" && !$this->isBirthday($item) && !$this->isTodo($item))
      {
        $style = '';
        if(!$showcategory)
          $style = 'style="color: '.$itemColors[$item["category"]]["fore"].';"';
        $tmp.=href(dispatch_url("scheduler.scheduler","view",array("atkselector"=>"scheduler_scheduler.id=".$item["id"],"scheduler_id"=>$item["id"],"itemdate"=>$item["startdate"])),
                   $item["title"],SESSION_NESTED,false,$style);
      }
      else
      {
        $tmp.=$item["title"];
      }
      if(array_key_exists("location",$item) && $item["location"]!="")
        $tmp.=" (".$item["location"].")";
    }
    return $tmp;
  }
  
  function allowed($action)
  {
    return true;
  }
  
  function isBirthday($item)
  {
    return (array_key_exists("birthday",$item) && $item["birthday"]!=''?true:false);
  }
  
  function isTodo($item)
  {
    return (array_key_exists("todo",$item) && $item["todo"]!=''?true:false);
  }
  
  function getCategory($item)
  {
    if(array_key_exists("category",$item) && isset($item["category"]))
    {
      $itemColors = $this->scheduler_model->getLegendColors();
      return '<img style="background-color: '.$itemColors[$item["category"]]["back"].';" src="images/trans.gif" border="1" width="10" height="10" title="'.$itemColors[$item["category"]]["desc"].'">&nbsp;';
    }
    return '';
  }
  
   /**
   * Is a calendar item recurring
   *
   * @param array $item Calendar item
   * @return bool Is recurring
   */
  function isRecurring($item)
  {
    return (array_key_exists("recur",$item) && $item["recur"]!="once");
  }

  /**
   * Is a calendar item all day
   *
   * @param array $item Calendar item
   * @return bool Is all day
   */
  function isAllDay($item)
  {
    return (array_key_exists("allday",$item) && $item["allday"]==1);
  }

  /**
   * Is a calendar item private
   *
   * @param array $item Calendar item
   * @return bool Is private
   */
  function isPrivate($item)
  {
    return (array_key_exists("private",$item) && $item["private"]==1);
  }

  /**
   * Has a calendar item an alarm
   *
   * @param array $item Calendar item
   * @return bool Has an alarm
   */
  function hasAlarm($item)
  {
    return (array_key_exists("reminder",$item) && $item["reminder"]==1);
  }

  /**
   * Has a calendar item an description
   *
   * @param array $item Calendar item
   * @return bool Has an description
   */
  function hasDescription($item)
  {
    return (array_key_exists("description",$item) && $item["description"]!="");
  }
  
  
}




?>