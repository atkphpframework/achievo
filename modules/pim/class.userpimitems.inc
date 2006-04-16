<?php
useattrib("atkListAttribute");
useattrib("atknumberattribute");
useattrib("atkdummyattribute");
useattrib("atknumberattribute");
userelation("atkManyToOneRelation");

class pimItemAttribute extends atkListAttribute
{

  var $m_pimitemsloaded = false;

  function pimItemAttribute($name, $flags=0, $size=255)
  {
    $this->atkListAttribute($name, array(), array(), $flags, $size);
    $this->m_dbfieldtype = "string";
  }

  function loadpimitems()
  {
    if ($this->m_pimitemsloaded)
      return;

    $pimItems = atkHarvestModules("getPimItems","",true);
    $this->m_lookup = array();
    foreach ($pimItems as $module => $items)
    {
      foreach($items as $itemName => $itemFunc)
      {
        $item = $module.'_'.$itemName;
        $this->m_lookup[$item] = atkText($module,$module).' - '.atkText(array("pim_$itemName", $itemName),$module);
      }
    }
    $this->m_options = array_values($this->m_lookup);
    $this->m_values = array_keys($this->m_lookup);
    $this->m_pimitemsloaded = true;
  }

  function display($record)
  {
    $this->loadpimitems();
    return parent::display($record);
  }

  function edit($record="", $fieldprefix="")
  {
    $this->loadpimitems();
    return parent::edit($record, $fieldprefix);
  }
}

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