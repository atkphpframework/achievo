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
    $this->atkAttribute($name, $flags, 15);
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
      $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_OBLIGATORY));
    }
    else
    {
      $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE_LIST|AF_OBLIGATORY));
      $this->addFilter("person.status","active");
    }
    $this->add(new atkOneToManyRelation("contract","employee.usercontracts","userid",AF_HIDE_LIST));

    if (atkconfig("authentication")=="db")
    {
      $this->add(new useridAttribute("userid" ,AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE), 1); // first field
      $this->add(new atkDummyAttribute("passwordexplanation", text("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW));
      $this->add(new atkPasswordAttribute("password", false, AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE, 40));
    }
    $this->add(new atkAttribute("role", AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15)); // discriminator.    
    
    $this->m_securityMap["deactivate"] = "edit"; // If you may edit, you may also deactivate.
  }

  function initial_values()
  {
    $initial["role"] = "employee";
    $initial["status"] = "active";
    return $initial;
  }
  
  /**
   * Are we in 'active' emps or 'all' emps mode?
   */
  function getView()
  {
    global $g_sessionManager;
    $view = $g_sessionManager->stackVar("view");
    if ($view=="")
    {
      $view = "active";
    }
    return $view;
  }
  
  function action_admin($handler)
  {
    $view = $this->getView();
    if ($view=="active")
    {
      $this->addFilter("person.status","active");
    }
    return $handler->action_admin();
  }
  
  function adminFooter()
  {  
    $view = $this->getView();
    
    if ($view=="active")
    {
      return text("inactiveemployeesarehidden", $this->m_type, $this->m_module).' '.
             href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=all', text('showallemployees', $this->m_type, $this->m_module));
    }    
  }

  function action_stats()
  {
    include_once moduleDir("employee").'employeestats.inc';
  }

  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }
  
  function recordActions($rec, &$actions, &$mraactions)
  {
    $user = getUser();
    if ($rec["status"] == "active" && $rec["id"]!=$user["id"])
    {
      // Actve users may be deactivated, but not the logged in user.
      $actions["deactivate"] = session_url("dispatch.php?atknodetype=".$this->atkNodeType()."&atkaction=deactivate&atkselector=[pk]", SESSION_NESTED);
    }
  
    // Prevent users from deleting themselves when they are logged in.
    if($rec["id"]==$user["id"]) unset($actions["delete"]);
  }
  
  function action_deactivate()
  {
    global $g_db;
    if ($this->m_postvars["atkselector"]!="")
    {
      $g_db->query("UPDATE person SET status='nonactive' WHERE ".$this->m_postvars["atkselector"]);
    }
    $this->redirect();
  }

}

?>
