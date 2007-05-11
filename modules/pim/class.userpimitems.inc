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

  /**
   * Return the attributes that are used when determining a descriptor.
   * @return array The names of the attributes forming the descriptor.
   */
  function descriptor_fields()
  {
    return array("pimitem");
  }

  /**
   * Determine a descriptor of a record.
   *
   * @param array $record Array containing the record for which a descriptor should be made
   * @return string Descriptor string
   */
  function descriptor(&$record)
  {
    $pimitemattr = &$this->getAttribute("pimitem");
    return $pimitemattr->display($record);
  }

  function addDefaultPimItemsForUser($userid)
  {
    $defaultitems = array("timereg_timeregistration","scheduler_schedule","todo_todos");
    $orderby=10;
    foreach($defaultitems as $pimitem)
    {
      $record=array("userid"=>"$userid",
                    "pimitem"=>$pimitem,
                    "orderby"=>$orderby);
      $this->addDb($record);
      $orderby+=10;
    }
  }
}

?>