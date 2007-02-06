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

    function action_edit($handler)
    {
      global $g_user;
      
      $atkselector = "";
      if(is_array($this->m_postvars["atkselector"]))
      {
        $atkselector = "(".implode(" OR ", $this->m_postvars["atkselector"]).")";      
      }
      else 
        $atkselector = $this->m_postvars["atkselector"];
      
      $recs = $this->selectDb($atkselector, "hours.id", "", "", array("userid", "activitydate"));
  
      $this->m_lock = $this->getLockType($recs[0]["userid"]["id"],
                        sprintf("%02d-%02d-%02d", $recs[0]["activitydate"]["year"],
                                                  $recs[0]["activitydate"]["month"],
                                                  $recs[0]["activitydate"]["day"]));
      if ($this->m_lock=="")
      {
        if (!$this->allowed("any_user"))
        {
          $this->addFilter("hours.userid",strtolower($g_user["id"]));
        }
        return $handler->action_edit();
      }
      else
      {
        // Week is locked. We may not edit, only view.
        $handler = $this->getHandler("view");
        return $handler->invoke("action_view");
      }
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
      
      $phase = decodeKeyValuePair(atkArrayNvl($this->m_postvars, "phaseid"));
      $activity = decodeKeyValuePair(atkArrayNvl($this->m_postvars, "activityid"));
      $phaseid = $phase["phase.id"];
      $activityid = $activity["activity.id"];

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
