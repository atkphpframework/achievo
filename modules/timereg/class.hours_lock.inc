<?php

useattrib("atkboolattribute");
useattrib("atkdummyattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");

class hours_lock extends atkNode
{
  function hours_lock()
  {
    $this->atkNode("hours_lock",NF_NO_EDIT|NF_NO_EXTENDED_SEARCH|NF_NO_VIEW|NF_MRA);

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkDummyAttribute("weekexplain",text("hourlock_week_explanation"),AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("week",AF_SEARCHABLE|AF_OBLIGATORY, 6));
    $this->add(new atkDummyAttribute("useridexplain",text("hourlock_userid_explanation"),AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_SEARCHABLE));
    if (atkconfig("lock_week_approval_required") && is_allowed("hours_lock", "approve"))
    {
      $this->add(new atkBoolAttribute("approved", AF_HIDE_ADD));
    }

    $this->setOrder("week DESC");
    $this->setTable("hours_lock");
  }
  
  function postAdd($record)
  {
    global $g_db;
    atkdebug("postAdd");
    // After adding a lock for all users, we delete individual locks for that same week.  
    if ($this->m_attribList["userid"]->isEmpty($record))
    {
      $query = "DELETE FROM hours_lock WHERE week='".$record['week']."' AND userid IS NOT NULL AND id<>".$record["id"];
      $g_db->query($query);
    }
  }
  
  function week_validate(&$record, $mode)
  {
    $value = $record["week"];
    $week = substr($value,4,2);
    $year = substr($value,0,4);
    if (strlen($value)!=6 || !is_numeric($value) || $week<0 || $week>53 || $year<1970)
    {
      triggerError($record, "week", "error_invalidweek");
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