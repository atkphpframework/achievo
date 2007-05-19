<?php
useattrib("atkupdatestampattribute");

class eventLog extends atkNode
{
  function eventLog()
  {
    $this->atkNode("eventlog");
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("user_id",AF_OBLIGATORY));
    $this->add(new atkUpdateStampAttribute("stamp",AF_OBLIGATORY));
    $this->add(new atkAttribute("node",AF_OBLIGATORY));
    $this->add(new atkAttribute("action",AF_OBLIGATORY));
    $this->add(new atkAttribute("primarykey",AF_OBLIGATORY));
    $this->add(new atkAttribute("summary",AF_OBLIGATORY));
    $this->setTable("crm_eventlog");
    $this->setOrder("stamp");
  }
  
  function postAdd($record)
  {
    $this->pruneHistory($record["user_id"]);
    return true;
  }
  
  function delete_event_history($node,$primarykey)
  {
    $db = &atkGetDb();
    $this->deleteDB("node='".$db->escapeSQL($node)."' AND primarykey='".$db->escapeSQL($primarykey)."'");
  }
  
  function pruneHistory($user_id)
  {
    $db = &$this->getDb();
    $history_max_viewed = atkconfig::get("crm","history_max_viewed",5);
    $count = $this->countDb("user_id='$user_id'");
    atkdebug("CRM Eventlog History size: (current, max) ($count, $history_max_viewed)");
    
    while($count > $history_max_viewed)
    {
      $record = $this->selectDb("user_id='$user_id'",'stamp ASC',array(1,1));
      $this->deleteDb("id='".$record[0]['id']."'");
      $count--;
    }
  }
  
  function getRecentlyViewed($user_id,$node="")
  {
    $selector = "user_id='$user_id'";
    if($node!="") $selector.=" AND node='$node'";
    $max = atkconfig::get("crm","history_max_viewed",5);
    $records = $this->selectDb($selector,"stamp DESC",array(1,$max));
    $items = array();
    foreach($records as $rec)
    {
      $items[] = array("url"=>dispatch_url($rec["node"],"view",array("atkprimkey"=>$rec["primarykey"])),
                        "summary"=>$rec["summary"]);
                              
    }
    return $items;
  }
}

?>