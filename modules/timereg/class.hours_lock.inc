<?php

useattrib("atkboolattribute");
useattrib("atkdummyattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");

class hours_lock extends atkNode
{
  var $m_lockmode;
  
  function hours_lock()
  {

    if  (atkConfig::get("timereg", "lockmode", "week") == "week") {
      $this->m_lockmode = "week";
    } else {
      $this->m_lockmode = "month";
    }

    $this->atkNode("hours_lock",NF_NO_EDIT|NF_NO_EXTENDED_SEARCH|NF_NO_VIEW|NF_MRA);

    $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
    $this->add(new atkDummyAttribute("periodexplain", atktext("hourlock_".$this->m_lockmode."_explanation"),AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("period",AF_SEARCHABLE|AF_OBLIGATORY, 6));
    $this->add(new atkDummyAttribute("useridexplain",atktext("hourlock_userid_explanation"),AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_SEARCHABLE));
		$hideapproved = (atkconfig("lock_week_approval_required") && is_allowed("hours_lock", "approve")) ? AF_HIDE : 0;
		$this->add(new atkBoolAttribute("approved", AF_HIDE_ADD|$hideapproved));
		$this->add(new atkAttribute("coordinator",AF_HIDE));

    $this->setOrder("period DESC");
    $this->setTable("hours_lock");
  }

  function postAdd($record)
  {
    $db = &atkGetDb();
    atkdebug("postAdd");
    // After adding a lock for all users, we delete individual locks for that same period.
    if ($this->m_attribList["userid"]->isEmpty($record))
    {
      $query = "DELETE FROM hours_lock WHERE period='".$record['period']."' AND userid IS NOT NULL AND id<>".$record["id"];
      $db->query($query);
    }
    return true;
  }

  function period_validate(&$record, $mode)
  {
    $value = $record["period"];
    $period = substr($value,4,2);
    $year = substr($value,0,4);

    if (strlen($value)!=6 || !is_numeric($value) || $period<0 || ($this->m_lockmode == "week" && $period>53) || ($this->m_lockmode == "month" && $period>12) || $year<1970)
    {
      triggerError($record, $this->m_lockmode, "error_invalid".$this->m_lockmode);
    }
  }

  function userid_display($rec, $mode)
  {
    if ($this->m_attribList["userid"]->isEmpty($rec))
    {
      return "All users";
    }
    else
    {
      return $this->m_attribList["userid"]->display($rec, $mode);
    }
  }
}

?>