<?php

class profile extends atkNode
{
  function profile()
  {
    $this->atkNode("profile", NF_EDITAFTERADD);

    $this->addAttribute(new atkAttribute("id",AF_PRIMARY|AF_AUTO_INCREMENT|AF_HIDE));
    $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE));      
    $this->addAttribute(new atkDummyAttribute("profile_explanation",text("profile_explanation"),AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkProfileAttribute("accessrights",AF_HIDE_LIST|AF_HIDE_ADD|AF_BLANKLABEL));

    $this->setTable("profile","profile");
    $this->setOrder("name");
  }

  function descriptor_def()
  {
    return "[name]";
  }
}
?>