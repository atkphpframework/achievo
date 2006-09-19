<?php
  atkimport("modules.timereg.hours");
  class movehours extends hours
  {

    function movehours($name="movehours", $flags=0)
    {
      // Use the normal hours node constructor
      $this->hours($name, $flags);

      // Hide anything but the projectid, phaseid and activityid relations
      $attribkeys = array_keys($this->m_attribList);
      for($i=0,$_i=count($attribkeys); $i<$_i; $i++)
        if (!in_array($attribkeys[$i], array("projectid", "phaseid", "activityid", "javascript")))
          $this->m_attribList[$attribkeys[$i]]->addFlag(AF_HIDE);
    }

    function allowed($action, $record="")
    {
      $secMgr = &atkGetSecurityManager();
      return $secMgr->allowed("timereg.hours", "edit");
    }

    function doMove($ids, $phaseid, $activityid)
    {
      $db = &atkGetDb();
      foreach($ids as $id)
      {
        $db->query("UPDATE hours SET phaseid='$phaseid', activityid='$activityid' WHERE hours.id='$id'");
      }
    }

    function action_save(&$handler)
    {
      // Get the relevant postvars
      $selector = atkArrayNvl($this->m_postvars, "atkselector", array());
      $phaseid = atkArrayNvl($this->m_postvars, "phaseid");
      $activityid = atkArrayNvl($this->m_postvars, "activityid");

      // Get a list of hours id's
      $ids = array();
      foreach($selector as $selectoritem)
      {
        $ids[] = intval(atkArrayNvl(decodeKeyValuePair($selectoritem), "hours.id"));
      }

      // Move the hours
      $this->doMove($ids, $phaseid, $activityid);

      // Get a level back
//      $this->redirect(session_url($_SERVER["PHP_SELF"],SESSION_BACK,2));
      $this->redirect();
    }

    function editArray($mode="add", $record=NULL, $forceList="", $suppressList="", $fieldprefix="")
    {
      $result = parent::editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
      $selector = atkArrayNvl($this->m_postvars, "atkselector", array());
      foreach($selector as $selectoritem)
        $result["hide"][] = '<input type="hidden" name="atkselector[]" value="'.$selectoritem.'">';
      return $result;
    }


  }

?>
