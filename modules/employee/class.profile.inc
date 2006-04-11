<?php

useattrib("atkdummyattribute");
useattrib("atknumberattribute");
useattrib("atkprofileattribute");

class profile extends atkNode
{
  function profile()
  {
    $this->atkNode("profile", NF_EDITAFTERADD|NF_NO_VIEW); // view disabled as long as profileattribute does not support display

    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE, 50));
    $this->add(new atkDummyAttribute("profile_explanation",atktext("profile_explanation"),AF_HIDE_LIST|AF_HIDE_ADD));
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