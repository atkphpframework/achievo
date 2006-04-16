<?php
useattrib("atknumberattribute");
useattrib("pim.pimitemattribute");
useattrib("atkdummyattribute");
userelation("atkManyToOneRelation");

class userpimitems extends atkNode
{

  function userpimitems()
  {
    $this->atkNode("userpimitems");
    $this->setTable("userpimitems");
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("userid","employee.userprefs",AF_HIDE));
    $this->add(new pimItemAttribute("pimitem",AF_OBLIGATORY|AF_NO_TRANSLATION));
    $this->add(new atkDummyAttribute("pim_orderby_help",atktext("pim_orderby_help"),AF_BLANK_LABEL|AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("orderby"));
    $this->setSecurityAlias("employee.userprefs");
    $this->addSecurityMap("add","edit");
    $this->addSecurityMap("delete","edit");
    $this->setOrder("orderby");
    $this->setTable("userpimitems");
    $this->setDescriptorTemplate("[pimitem]");
  }
}

?>