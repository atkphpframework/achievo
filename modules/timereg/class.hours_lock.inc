<?php

useattrib("atkdummyattribute");

class hours_lock extends atkNode
{
  function hours_lock()
  {
    $this->atkNode("hours_lock",NF_NO_EDIT|NF_NO_EXTENDED_SEARCH|NF_NO_VIEW|NF_MRA);

    $this->add(new atkDummyAttribute("label",text("hourlock_week_explanation"),AF_HIDE_LIST));
    $this->add(new atkAttribute("week",AF_PRIMARY|AF_SEARCHABLE, 6));
    $this->add(new atkAttribute("userid",AF_PRIMARY|AF_SEARCHABLE, 15));

    $this->setOrder("week DESC");
    $this->setTable("hours_lock");
  }

  function userid_edit($record)
  {
    $res = '<select name="userid">';
    $res.= $this->get_employees($record["userid"]);
    $res.= '</select>';
    return $res;
  }

  function userid_display($record)
  {
    if ($record["userid"] != "*")
    {
      global $g_db;
      $sql = "SELECT lastname, firstname FROM person WHERE role ='employee' AND userid='".$record["userid"]."'";
      $res = $g_db->getRows($sql);
      $buff = $res[0]["lastname"].", ".$res[0]["firstname"];
      return $buff;
    }
    return text("all_users");
  }

  function get_employees($userid)
  {
    global $g_db;

    $sql = "SELECT lastname, firstname, userid
            FROM person
            WHERE status='active' AND role='employee'
            ORDER BY lastname
           ";

    $records = $g_db->getrows($sql);

    // add the 'select all users' option.
    $employee_code.='<OPTION VALUE="*">'.text("all_users").'</OPTION>';

    for($i=0;$i<count($records);$i++)
    {
      if ($userid==$records[$i]["userid"]) $selected="selected";
      else $selected="";
      $employee_code.='<OPTION VALUE="'.$records[$i]["userid"].'" '.$selected.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
    }
    return $employee_code;
  }

  function postAdd($record)
  {
    global $g_db;
    atkdebug("postAdd");
    // After adding a lock for all users, we delete individual locks for that same week.
    if ($record["userid"]=="*")
    {
      $query = "DELETE FROM hours_lock WHERE week='".$record['week']."' AND userid <> '*'";
      $g_db->query($query);
    }
  }
}

?>