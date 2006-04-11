<?php
useattrib("atkListAttribute");
useattrib("atknumberattribute");
useattrib("atkdummyattribute");
useattrib("atknumberattribute");
userelation("atkManyToOneRelation");

class userpimitems extends atkNode
{

  function userpimitems()
  {
    $this->atkNode("userpimitems");
    $this->setTable("userpimitems");
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("userid","employee.userprefs",AF_HIDE));
    $pimitems = $this->getPimItems();
    $this->add(new atkListAttribute("pimitem",array_values($pimitems),array_keys($pimitems),AF_OBLIGATORY|AF_NO_TRANSLATION,255));
    $this->add(new atkDummyAttribute("pim_orderby_help",atktext("pim_orderby_help"),AF_BLANK_LABEL|AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("orderby"));
    $this->setSecurityAlias("employee.userprefs");
    $this->addSecurityMap("add","edit");
    $this->addSecurityMap("delete","edit");
    $this->setOrder("orderby");
    $this->setTable("userpimitems");
    $this->setDescriptorTemplate("[pimitem]");
  }

  function getPimItems()
  {
    $pimItems = atkHarvestModules("getPimItems","",true);
    $ret = array();
    foreach ($pimItems as $module => $items)
    {
      foreach($items as $itemName => $itemFunc)
      {
        $item = $module.'_'.$itemName;
        $ret[$item] = atkText($module,$module).' - '.atkText(array("pim_$itemName", $itemName),$module);
      }
    }

    return $ret;
  }
}

?>