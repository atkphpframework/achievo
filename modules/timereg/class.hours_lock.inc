<?php

useattrib("atkdummyattribute");
userelation("atkmanytoonerelation");

class hours_lock extends atkNode
{
  function hours_lock()
  {
    $this->atkNode("hours_lock",NF_NO_EDIT|NF_NO_EXTENDED_SEARCH|NF_NO_VIEW|NF_MRA);

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkDummyAttribute("weekexplain",text("hourlock_week_explanation"),AF_HIDE_LIST));
    $this->add(new atkAttribute("week",AF_SEARCHABLE, 6));
    $this->add(new atkDummyAttribute("useridexplain",text("hourlock_userid_explanation"),AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_SEARCHABLE));

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