<?php
class contracttemplateAttribute extends atkManyToOneRelation
{
  /**
   * Constructor
   *
   * @param string $name Attribute name
   * @param string $target Target
   * @param int $flags Flags
   */
  function contracttemplateAttribute($name, $target, $flags)
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
    atkdebug("Type: $type");
    atk_var_dump($record,'Add record contract');

    if ($type == "add")
    {
      if ($record['contract_template']['id']!="")
      {
        $contract = $this->getTemplate($db, $record);
        if(is_array($contract))
        {
          $new_id = $db->nextid('usercontract');
          $sql = "INSERT INTO usercontract (id,
                                            userid,
                                            uc_hours,
                                            startdate,
                                            enddate,
                                            description,
                                            workingdays,
                                            workstarttime,
                                            workendtime)
                  VALUES (".$db->escapeSQL($new_id).",
                          ".$db->escapeSQL($record['id']).",
                          '".$db->escapeSQL($contract['uc_hours'])."',
                          ".$db->func_now().",
                          NULL,
                          '".$db->escapeSQL($contract['description'])."',
                          '".$db->escapeSQL($contract['workingdays'])."',
                          '".$db->escapeSQL($contract['workstarttime'])."',
                          '".$db->escapeSQL($contract['workendtime'])."')

          ";
          $db->query($sql);
        }
      }
    }
    return true;
  }

  protected function getTemplate($db,$record)
  {
    $sql = "SELECT * FROM tpl_usercontract WHERE id = %d";
    $rows =  $db->getrows(sprintf($sql,$record['contract_template']['id']));
    if(count($rows)>0)
    {
      return $rows[0];
    }
    return false;
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