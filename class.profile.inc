<?php

useattrib("atkdummyattribute");
useattrib("atkprofileattribute");

class profile extends atkNode
{
  function profile()
  {
    $this->atkNode("profile", NF_EDITAFTERADD);

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE));      
    $this->add(new atkDummyAttribute("profile_explanation",text("profile_explanation"),AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkProfileAttribute("accessrights",AF_HIDE_LIST|AF_HIDE_ADD|AF_BLANKLABEL));

    $this->setTable("profile","profile");
    $this->setOrder("name");
  }

  function descriptor_def()
  {
    return "[name]";
  }
}

?>