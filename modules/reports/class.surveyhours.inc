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

    $this->add(new atkManyToOneRelation("organizationid", "organization.organization",AF_HIDE_LIST));

    $projectAtt = &$this->getAttribute("projectid");
    $projectAtt->removeFlag(AF_OBLIGATORY);
    $this->setOrder("");
    $this->setSecurityAlias('timereg.hours');
  }

  function recordActions($record, &$actions, &$mraactions)
  {
    parent::recordActions($record, $actions, $mraactions);
    foreach($actions as $key=>$url)
    {
      $actions[$key] = str_replace("reports.surveyhours", "timereg.hours", $url);
    }
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
  
  function phaseid_selection($record, $mode="")
  {
    $p_attr = &$this->getAttribute('phaseid');
    if(atkArrayNvl($this->m_postvars, "subproject", 0) == 0)
    {
      return $p_attr->getSelectableRecords($record, $mode);
    }
    else 
    {
      $ids = $this->_getProjectAndSubProjectId($record['projectid']['id']);
      
      $p_attr->createDestination();

      $selector = "phase.projectid IN(".implode(",",$ids).")";
      $result = $p_attr->m_destInstance->selectDb($selector,"","","",atk_array_merge($p_attr->m_destInstance->descriptorFields(),$p_attr->m_destInstance->m_primaryKey),$mode);
      return $result;
    }
  }
  
  function _getProjectAndSubProjectId($projectid)
  {
    $node = &atkGetNode("project.project");
    $rows = $node->selectDb("project.master_project='$projectid'","","","",array("id"));
    
    $id = array();
    $id[] = $projectid;
    
    foreach ($rows as $row)
    {
      $id[] = $row['id'];
    }
    return $id;
  }

}
?>