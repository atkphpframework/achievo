<?php

useattrib("atkcolorpickerattribute");
useattrib("atkdummyattribute");

class schedule_types extends atkNode
{
  function schedule_types()
  {
    $this->atkNode("schedule_types", NF_MRA);

    $this->add(new atkAttribute("id"                 , AF_AUTOKEY));
    $this->add(new atkAttribute("description"        , AF_OBLIGATORY));
    $this->add(new atkColorPickerAttribute("bgcolor" , "", AF_OBLIGATORY|AF_HIDE_LIST|AF_FORCE_LOAD|AF_POPUP));
    $this->add(new atkColorPickerAttribute("fgcolor" , "", AF_OBLIGATORY|AF_HIDE_LIST|AF_FORCE_LOAD|AF_FORCE_LOAD|AF_POPUP));    
    $this->add(new atkDummyAttribute("example"       , "", AF_HIDE_ADD));

    $this->setTable("schedule_types", "schedule_types");         
    $this->setOrder("id");
  }

  function example_display($record)
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
   return "[description]";
  }


  function example_edit($record)
  {
    return text("example").":".$this->example_display($record);
  }

}
?>