<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
useattrib("atklistattribute");
useattrib("atkdummyattribute");
useattrib("atkpasswordattribute");
include_once (moduleDir("person")."class.person.inc");

class useridAttribute extends atkAttribute
{
  function useridAttribute($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags);
  }

  function value2db($rec)
  {
    return escapeSQL(str_replace(" ", "_", strtolower($rec["userid"])));
  }
}

class employee extends person
{
  function employee()
  {
    global $ATK_VARS,$g_user;

    $this->person("employee");
    $this->addFilter("person.role='employee'");

    $this->add(new atkManyToOneRelation("entity", "employee.profile",AF_RELATION_AUTOLINK));

    // you can't be your own supervisor neither can a nonactive employee be a supervisor
    $preFilter = new atkManyToOneRelation("supervisor","employee.employee",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK);
    $preFilter->m_destinationFilter = "userid !='[userid]' AND status ='active'";

    $this->add($preFilter);
    // If we are in employee administration mode, we show all users. Otherwise, we only show active users.
    if ($ATK_VARS["atknodetype"]=="employee.employee"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit")))
    {
      $this->add(new atkListAttribute("status", array("active","nonactive"), array()));
    }
    else
    {
      $this->add(new atkListAttribute("status", array("active","nonactive"), array()));
      $this->addFilter("person.status","active");
    }
    $this->add(new atkOneToManyRelation("contract","employee.usercontracts","userid",AF_HIDE_LIST));

    if (atkconfig("authentication")=="db")
    {
      $this->add(new useridAttribute("userid" ,AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE));
      $this->add(new atkDummyAttribute("passwordexplanation", text("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH));
      $this->add(new atkPasswordAttribute("password", AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE));
    }
    $this->add(new atkAttribute("role"           , AF_READONLY|AF_HIDE));
  }

  function initial_values()
  {
    $initial["role"] = "employee";
    $initial["status"] = "active";
    return $initial;
  }

  function action_stats()
  {
    include_once moduleDir("employee").'employeestats.inc';
  }

  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }

}

?>