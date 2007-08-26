<?php
useattrib("atkcolorpickerattribute");
useattrib("atkdummyattribute");

class scheduler_category extends atkNode
{
  function scheduler_category()
  {
    $this->atkNode("scheduler_category", NF_MRA);

    $this->add(new atkAttribute("id"                 , AF_AUTOKEY));
    $this->add(new atkAttribute("description"        , AF_OBLIGATORY, 50));
    $this->add(new atkColorPickerAttribute("bgcolor" , "", AF_OBLIGATORY|AF_HIDE_LIST|AF_FORCE_LOAD|AF_POPUP));
    $this->add(new atkColorPickerAttribute("fgcolor" , "", AF_OBLIGATORY|AF_HIDE_LIST|AF_FORCE_LOAD|AF_FORCE_LOAD|AF_POPUP));
    $this->add(new atkDummyAttribute("example"       , "", AF_HIDE_ADD));

    $this->setTable("scheduler_category");
    $this->setOrder("description");
  }

  function example_display($record)
  {
   $bgcolor = $record["bgcolor"];
   $fgcolor = $record["fgcolor"];
   $buff = '<span style="border: medium none ; background-color: '.$bgcolor.'; color: '.$fgcolor.';">&nbsp;Scheduler Item&nbsp;</span>';
   return $buff;
  }

  function descriptor_def()
  {
   return "[description]";
  }

  function example_edit($record)
  {
    return atktext("example").":".$this->example_display($record);
  }
  
  

}
?>