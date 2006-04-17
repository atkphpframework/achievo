<?php

useattrib("atklistattribute");

class dependencyAttribute extends atkAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function dependencyAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags);
  }

  function fetchValue($postvars)
  {
    $values = isset($postvars[$this->formName()]) ? $postvars[$this->formName()] : null;
    $res = array();
    if (is_array($values))
    {
      if($values["first"]!=""&&$values["last"]!=""&&$values["first"]!=$values["last"])
      {
        $res["first"]=$values["first"];
        $res["last"]=$values["last"];
      }
      // Find out if we need to delete dependencies..
      while (list($name,$val) = each($values))
      {
        if (substr($name,0,3)=="del")
        {
          list($del,$first,$last) = split('_',$name);
          $delarr[] = array("first"=>$first,"last"=>$last);
        }
      }
      $res["del"] = $delarr;
    }
    return $res;
  }

  /**
   * Returns a piece of html code that can be used in a form to edit this
   * attribute's value.
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {
    $deps = $record[$this->fieldname()]["deps"];

    if (count($deps)==0)
    {
      // Someone could have pressed save, and we don't post records in this
      // attribute, so we have to reload them from the database
      $rows = $this->load("",$record);
      $deps = $rows["deps"];
    }

    $result = "<br>";

    if (count($deps)>0)
    {
      $data = array();
      $data[] = array(atktext('dependencies'),atktext('delete'));
      for ($i=0;$i<count($deps);$i++)
      {
        $label = atktext("phase")." '".$deps[$i]["firstname"]."' ".atktext("mustbecompletedbeforephase")." '".$deps[$i]["lastname"]."'";
        $delete = '&nbsp;&nbsp;<input type="checkbox" name="'.$this->formName()."[del_".$deps[$i]["first"]."_".$deps[$i]["last"].']" value="1">';
        $data[] = array($label,$delete);
      }
      $tbl = &atknew("atk.utils.atktablerenderer");
      $result.=$tbl->render($data,TBL_HEADER|TBL_ALTERNATE,"recordlist")."<br>";
    }

    $recs = $this->getPhases($record["id"]);

    $names[0] = "---";
    $ids[0]="";

    for ($i=0;$i<count($recs);$i++)
    {
      $names[] = $recs[$i]["name"];
      $ids[] = $recs[$i]["id"];
    }

    $phaselist1 = new atkListAttribute($this->formName()."[first]",$names, $ids);
    $phaselist2 = new atkListAttribute($this->formName()."[last]",$names, $ids);

    if (count($names)>2)
    {
      $result.=atktext("addnewdependency").":<br>";
      $result.=atktext("phase")." ".$phaselist1->edit()." ".atktext("mustbecompletedbeforephase")." ".$phaselist2->edit();
    }

    return $result;
  }

  function getPhases($projectid)
  {
    $db = &atkGetDb();

    $query = "SELECT
                id, name
              FROM
                phase
              WHERE
                projectid = ".$projectid." ORDER BY name";

    $recs = $db->getrows($query);
    return $recs;
  }

  /**
   * We don't support searching
   * @param $record array with fields
   * @return search field
   */
  function search($record="")
  {
    return "&nbsp;";
  }

  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
     // we don't add anything to the query.. (we have our own load and store methods
  }

  function load($notused, $rec)
  {
    if ($rec["id"]!="")
    {
      $query = "SELECT
                  phaseid_row as first,
                  ROW1.name as firstname,
                  phaseid_col as last,
                  COL1.name as lastname
                FROM
                  dependency, phase ROW1, phase COL1
                WHERE
                  ROW1.id = phaseid_row
                  AND COL1.id = phaseid_col
                  AND (ROW1.projectid = ".$rec["id"]." OR COL1.projectid = ".$rec["id"].")
                  ORDER BY phaseid_row";

      $db = &atkGetDb();
      return array("deps"=>$db->getrows($query));
    }
    else
    {
      return array("deps"=>array());
    }
  }

  function store($notused, $rec)
  {
    $db = &atkGetDb();

    $values = $rec[$this->fieldName()];

    // First delete deleted items.
    $delarr = $values["del"];
    if (is_array($delarr))
    {
      for ($i=0;$i<count($delarr);$i++)
      {
        $delquery = "DELETE
                     FROM
                       dependency
                     WHERE
                       phaseid_row = ".$delarr[$i]["first"]."
                       AND phaseid_col = ".$delarr[$i]["last"];

        $db->query($delquery);
      }
    }

    // Now add a new item..
    if ($values["first"]!="" && $values["last"]!="")
    {
      $query = "INSERT
                  INTO dependency (phaseid_row,phaseid_col)
                  VALUES (".$values["first"].",".$values["last"].")";

      $db->query($query);
    }
    return true;
  }

  function getSearchModes()
  {
    // search not supported.
    return array();
  }

  function searchCondition(&$query, $table, $value, $searchmode)
  {
    // Search not supported.
  }

  function dbFieldType()
  {
    return "";
  }
}
?>
