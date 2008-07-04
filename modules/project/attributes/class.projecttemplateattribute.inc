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

// When a new project is saved, you can base it on a template, i.e. select a set of
// default activities. To accomplish this, we make a new attribute based on a ManyToOneRelation
// that isn't stored in the db, but used to add records to the projectactivities table.

/**
 * Phase dependency attribute
 * 
 * @package achievo
 * @subpackage project
 */
class projecttemplateAttribute extends atkManyToOneRelation
{
  /**
   * Constructor
   *
   * @param string $name
   * @param string $target
   * @param int $flags
   */
  function projecttemplateAttribute($name, $target, $flags)
  {
    $this->atkManyToOneRelation($name, $target, $flags);
  }
  
  
  /**
   * get Storage type
   *
   * @param string $mode
   * @return int
   */
  function storageType($mode)
  {
    return ADDTOQUERY|POSTSTORE;
  }

 /**
   * Store phases and activities
   *
   * @todo add template field to the phase table (again) to activate the projectphasematrix 
   * 
   * @param object $db
   * @param array $record
   * @param string $type
   * @return boolean
   */
  function store($db, $record, $type)
  {
    // Todo, here we must save the activities from the template to the projectactivities table
    if ($type == "add")
    {
      if ($record['template']['id']!="")
      {
        // phase templates
        $phases = $this->selectTemplates($db, $record);

        // loop to insert phases , create reference table
        for ($i=0;$i<count($phases);$i++)
        {
           $new_id = $db->nextid('phase');
           $id_table[$phases[$i]['id']] = $new_id;

           $query = "INSERT INTO phase (id, name, status, projectid, description)
                     VALUES ('".$new_id."','".escapeSQL($phases[$i]['name'])."','active','".$record['id']."','".escapeSQL($phases[$i]['description'])."')";
           $db->query($query);

           //activities per phase
           $query = "INSERT INTO phase_activity (activityid, phaseid)
                     SELECT activityid, ".$new_id."
                     FROM tpl_phase_activity
                     WHERE phaseid = ".$phases[$i]['id'];

           $db->query($query);
        }

        // dependency templates
        $query = "SELECT phaseid_row, phaseid_col FROM tpl_dependency
                  WHERE tpl_dependency.projectid = ".$record['template']['id'];
        $dependencies = $db->getRows($query);

        // loop to insert dependencies, uses reference table
        for ($i=0;$i<count($dependencies);$i++)
        {
           $row = $id_table[$dependencies[$i]['phaseid_row']];
           $col = $id_table[$dependencies[$i]['phaseid_col']];

           if ($row!='' && $col!='')
           {
              $query = "INSERT INTO dependency (phaseid_row, phaseid_col)
                        VALUES ('".$row."','".$col."')";
              $db->query($query);
           }
        }
      }
    }
    return true;
  }
  
  /**
   * Select templates
   *
   * @param atkDb $db Atk DB
   * @param array $record Record
   * @return array
   */
  function selectTemplates(&$db, $record)
  {
    $query = "
      SELECT 
        tpl_phase.id, tpl_phase.name, tpl_phase.description 
      FROM 
        tpl_phase,tpl_project_phase
      WHERE 
        tpl_phase.id = tpl_project_phase.phaseid 
        AND tpl_project_phase.projectid = ".$record['template']['id'];

    return $db->getRows($query);
  }    
  

  /*function dbFieldType()
  {
    return "";
  }*/
}

?>