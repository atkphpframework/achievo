<?php

class schedule_types extends atkNode
{
  function schedule_types()
  {
    $this->atkNode("schedule_types");

    $this->addAttribute(new atkAttribute("id"                                , AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("description"                  , AF_OBLIGATORY));
    $this->addAttribute(new atkColorPickerAttribute("bgcolor"        , "", AF_OBLIGATORY|AF_POPUP));
    $this->addAttribute(new atkColorPickerAttribute("fgcolor"        , "", AF_OBLIGATORY|AF_HIDE_LIST|AF_FORCE_LOAD|AF_POPUP));    
    $this->addAttribute(new atkDummyAttribute("example"                      , "", AF_HIDE_ADD|AF_HIDE_LIST));

    $this->setTable("schedule_types", "schedule_types");         
    $this->setOrder("id");
  }

  function item_background_display($record)
  {
   $bgcolor = $record["bgcolor"]; 
   $fgcolor = $record["fgcolor"];

   $buff  = "<table cellpadding='0' cellspacing='0' border='0'><tr>";
   $buff .= "<td bgcolor='$bgcolor'><font face='verdana' size='-1' color='$fgcolor'>&nbsp;Scheduler item&nbsp;</font></td>";
   $buff .= "</tr></table>";
   return $buff;
  }

  function descriptor_def()
  {
   return "[item_description]";
  }


  function example_edit($record)
  {
    return text("example").":".$this->item_background_display($record);
  }



}
?>
