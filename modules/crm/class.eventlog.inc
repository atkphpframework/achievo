<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage crm
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

useattrib("atkupdatestampattribute");

/**
 * Class for managing the eventlog that is used for the 
 * lastviewed plugin to show the last viewed items.
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class eventLog extends atkNode
{
  /**
   * Constructor
   *
   * @return void
   */
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
  
  /**
   * After adding a record, prune the history for the current user
   *
   * @param array $record
   * @return boolean
   */
  function postAdd($record)
  {
    $this->pruneHistory($record["user_id"]);
    return true;
  }
  
  /**
   * Delete event history for given node and primary key
   *
   * @param string $node Node name
   * @param string $primarykey Primary Key
   * @return void
   */
  function delete_event_history($node,$primarykey)
  {
    $db = &atkGetDb();
    $this->deleteDB("node='".$db->escapeSQL($node)."' AND primarykey='".$db->escapeSQL($primarykey)."'");
  }
  
  /**
   * Prune history of a given user, default we only keep 5 records but his can be set 
   * in the config file of the crm module /configs/crm.php.inc
   *
   * @param int $user_id User id
   * @return void
   */
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
  
  /**
   * Get recently viewed items from the event log
   *
   * @param int $user_id User id
   * @param string $node String
   * @return array Array with event items
   */
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