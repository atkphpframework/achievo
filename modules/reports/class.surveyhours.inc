<?php
atkimport("module.timereg.hours");
class surveyhours extends hours 
{
  function surveyhours()
  {
    $this->hours('surveyhours');
    foreach (array_keys($this->m_attribList) as $attrName)
    {
      $attr = &$this->getAttribute($attrName);
      if (is_a($attr, 'atkmanytoonerelation'))
      {
        $attr->removeFlag(AF_RELATION_AUTOCOMPLETE);
        //we remove all dependee - hoursurvey will add own
        $attr->m_dependees = array();
      }
        
      if($attrName == "phaseid")
      {
        $attr->removeFlag(AF_OBLIGATORY);
      }        
    }
    
    /* TODO:
     * This feature was offerd to BlueWind, but never tested. When adding the next line you have
     * the option to choose a organisation in the timereg field.
     * Also the following patch is needed:
     * $setup->addColumn("hours", "organizationid", "int(10)", true, "0");
     */
    //$this->add(new atkManyToOneRelation("organizationid", "organization.organization",AF_HIDE_LIST));
    
    $projectAtt = &$this->getAttribute("projectid");
    $projectAtt->removeFlag(AF_OBLIGATORY);
    $this->setOrder("");    
    $this->setSecurityAlias('timereg.hours');    
  }
  
  /**
   * This filter is only needed in the hoursnode, where activityid
   * depends on the phaseid. So we override the function here to disable the
   * filter.
   *
   * @param array $record The record
   * @param String $mode The mode
   * @return result of function getSelectableRecords
   */
  function activityid_selection($record, $mode="")
  {
    $attr = &$this->getAttribute('activityid');
    return $attr->getSelectableRecords($record, $mode);    
  }

  /**
   * The original action_edit function of the hoursnode conflicts with the functionality
   * the hoursurvey needs.
   */
  function action_edit(&$handler)
  {
    return $handler->action_edit();
  }
}
?>