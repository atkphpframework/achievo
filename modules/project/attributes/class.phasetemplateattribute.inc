<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage project
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
userelation("atkmanytoonerelation");

/**
 * When a new phase is saved, you can base it on a template, i.e. select a set of 
 * default activities. To accomplish this, we make a new attribute based on a ManyToOneRelation
 * that isn't stored in the db, but used to add records to the projectactivities table.
 * Phase dependency attribute
 * 
 * @package achievo
 * @subpackage project
 */
class phasetemplateAttribute extends atkManyToOneRelation
{
  /**
   * Constructor
   *
   * @param string $name Attribute name
   * @param string $target Target
   * @param int $flags Flags
   */
  function phasetemplateAttribute($name, $target, $flags)
  {
    $this->atkManyToOneRelation($name, $target, $flags);
  }
  
  /**
   * Override of store fucntion
   *
   * @param atkDb $db Atk database
   * @param array $record Record
   * @param string $type Type
   * @return boolean
   */
  function store($db, $record, $type)
  {
     if ($type == "add")
    {
      if ($record['template']['id']!="")
      {
        // Make sure we don't activities that don't exist allready
        $query = "SELECT activityid FROM phase_activity WHERE phaseid='".$record['id']."'";
        $rows = $db->getrows($query);
        $where = '';
        if(count($rows))
        {
          $where = "AND activityid NOT IN (";
          for($i=0;$i<count($rows);$i++)
          {
            if($i>0) $where.=",";
            $where.=$rows[$i]['activityid'];
          }
          $where.=")";
        }
        $query = "INSERT INTO phase_activity (activityid, phaseid) SELECT activityid, ".$record['id']." FROM tpl_phase_activity WHERE phaseid = ".$record['template']['id']." ".$where;
        $db->query($query);
      }
    }
    return true;
  }
  
  /**
   * Database field type
   *
   * @return string
   */
  function dbFieldType()
  {
    return "";
  }
  
  /**
   * Post store
   *
   * @param string $mode Mode
   * @return int
   */
  function storageType($mode)
  {
    return POSTSTORE;
  }
  
  /**
   * No load
   *
   * @param string $mode Mode
   * @param boolean $searching Searching
   * @return int
   */
  function loadType($mode, $searching=false)
  {
    return NOLOAD;
  }
}

?>
