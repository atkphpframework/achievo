<?php

  useattrib("atktimeattribute");
  useattrib("atknumberattribute");
  useattrib("atkboolattribute");

  class workperiod extends atkNode
  {
    function workperiod()
    {
      $this->atkNode("workperiod");
      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE, 50));
      $this->add(new atkTimeAttribute("starttime", 0, 23, array("00","15","30","45"), "", AF_OBLIGATORY));
      $this->add(new atkTimeAttribute("endtime", 0, 23, array("00","15","30","45"), "", AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("percentage",AF_OBLIGATORY, 10));
      $this->add(new atkBoolAttribute("defaultrate"));

      $this->setTable("workperiod","workperiod");
      $this->setOrder("defaultrate DESC");
    }

    function descriptorFields()
    {
      return array("name", "starttime", "endtime");
    }

    /**
     * Descriptor override. If start/endtime are equal (time not used), time is not added to the descriptor.
     */
    function descriptor($record)
    {
      $res = $record["name"];
      if ($record["starttime"]!=$record["endtime"])
      {
        $res.=" (".$record["starttime"]["hours"].".".$record["starttime"]["minutes"]." - ".$record["endtime"]["hours"].".".$record["endtime"]["minutes"].")";
      }
      return $res;
    }

    function postUpdate($rec)
    {
      if ($rec["defaultrate"] == "1") $this->clearDuplicateDefaults($rec["id"]);
    }

    /**
     * Only one record may be default. The id of this rec should be passed
     * to the function. All other records will be set to non-default.
     *
     * @param $default_id Number The id of the record that is set to default.
     */
    function clearDuplicateDefaults($default_id)
    {
      $db = &atkGetDb();
      $db->query("UPDATE workperiod SET defaultrate = 0 WHERE id <> ".$default_id);
    }

    function postAdd($rec)
    {
      if ($rec["defaultrate"] == "1") $this->clearDuplicateDefaults($rec["id"]);
      return true;
    }
  }

?>