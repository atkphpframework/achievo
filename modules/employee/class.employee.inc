<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");
userelation("atkmanyboolrelation");
useattrib("atklistattribute");
useattrib("atkdummyattribute");
useattrib("atkpasswordattribute");
useattrib("atkdateattribute");
include_once (moduleDir("person")."class.person.inc");
include_once (moduleDir("utils")."class.dateutil.inc");

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

    $this->person("employee", NF_EDITAFTERADD);

    // In employee edit screen, we hide most fields in the add screen.
    foreach (array_keys($this->m_attribList) as $attribname)
    {
      if (!in_array($attribname, array("lastname", "firstname","email")))
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $p_attrib->m_flags|=AF_HIDE_ADD;
      }
    }

    $this->addFilter("person.role='employee'");
    $this->add(new atkDateAttribute("birthdate","F d Y","d F Y", 0, date("Ymd"), AF_HIDE_LIST));
    
    $this->add(new atkAttribute("bankaccount", AF_HIDE_LIST));
    $this->add(new atkAttribute("socialsecuritynumber", AF_HIDE_LIST));
    
    $this->add(new atkManyToOneRelation("functionlevel", "employee.functionlevel",AF_HIDE_ADD|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("department","employee.department",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK));
    
    //$this->add(new atkManyToOneRelation("entity", "employee.profile",AF_RELATION_AUTOLINK|AF_HIDE_ADD));
    $rel = &$this->add(new atkManyBoolRelation("roles", "employee.employeerole", "employee.profile", AF_HIDE_LIST));
    $rel->setLocalKey("employee_id");
    $rel->setRemoteKey("role_id");
    

    // you can't be your own supervisor neither can a nonactive employee be a supervisor
    $preFilter = new atkManyToOneRelation("supervisor","employee.employee",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK);
    $preFilter->m_destinationFilter = "userid !='[userid]' AND status ='active'";

    $this->add($preFilter);
    // If we are in employee administration mode, we show all users. Otherwise, we only show active users.
    $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE_LIST|AF_HIDE_ADD|AF_FORCE_LOAD));
    if ($ATK_VARS["atknodetype"]=="employee.employee"&& in_array($ATK_VARS["atkaction"],array("admin","add","update","edit")))
    {
      $this->addFilter("person.status","active");
    }
    $this->add(new atkOneToManyRelation("usercontract","employee.usercontracts","userid",AF_HIDE_LIST|AF_HIDE_ADD));
    
    $this->add(new useridAttribute("userid" ,AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE), 1); // first field
    
    if (atkconfig("authentication")=="db")
    {      
      $this->add(new atkDummyAttribute("passwordexplanation", text("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW));
      $this->add(new atkPasswordAttribute("password", false, AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE, 40));
    }
    $this->add(new atkAttribute("role", AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15)); // discriminator.    

    $this->m_securityMap["deactivate"] = "edit"; // If you may edit, you may also deactivate.
    
    $this->setOrder("lastname");
  }

  function initial_values()
  {
    $initial["role"] = "employee";
    $initial["status"] = "active";
    return $initial;
  }

  /**
   * Are we in 'active' emps or 'archive' mode?
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
      return text("onlyactiverecordsareshown")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=nonactive', text('clicktoviewarchive', $this->m_type, $this->m_module))."<br>";
    }
    else
    {
      return text("youareinthearchive")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=active', text('clicktoviewactiverecords', $this->m_type, $this->m_module))."<br>";
    }
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

  function action_deactivate(&$handler)
  {    
    if ($this->m_postvars["atkselector"]!="")
    {
      $db = &atkGetDb();
      $db->query("UPDATE person SET status='nonactive' WHERE ".$this->m_postvars["atkselector"]);
    }
    $this->redirect();
  }

  function graph_prjtime($params, $raw=false)
  {
    $db = &atkGetDb();
    $data = $db->getrows("SELECT
                              project.id, project.name, SUM(time) as total
                            FROM
                              hours, phase, project
                            WHERE
                              hours.phaseid = phase.id ".
                              ($params["employeeid"]!=""?" AND hours.userid = ".$params["employeeid"]:"")."
                              AND phase.projectid = project.id".
                              ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                              ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                          " GROUP BY
                              project.id
                            ORDER BY
                              project.name");

    if ($raw)
    {
      return $data;
    }

     // convert records to graph-compatible array.
    $dataset = array();
    for ($i=0, $_i=count($data); $i<$_i; $i++)
    {
      $dataset[$data[$i]["name"]] = $data[$i]["total"];
    }

    return array("registeredtimeperproj"=>$dataset);
  }

  function graph_activitytime($params, $raw=false)
  {
    $db = &atkGetDb();
    $data = $db->getrows("SELECT
                              activity.id, activity.name, SUM(time) as total
                            FROM
                              hours, activity
                            WHERE
                              hours.activityid = activity.id ".
                              ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                              ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                          ($params["employeeid"]!=""?" AND hours.userid = ".$params["employeeid"]:"")."
                            GROUP BY
                              activity.name
                            ORDER BY
                              activity.name");
    // In raw mode the data is sufficient.
    if ($raw) return $data;

    // convert records to graph-compatible array.
    $dataset = array();

    for ($i=0, $_i=count($data); $i<$_i; $i++)
    {
      $dataset[$data[$i]["name"]] = $data[$i]["total"];
    }

    return array("registeredtimeperactivity"=>$dataset);
  }
  
  function graph_dowtime($params, $raw=false)
  {
    $db = &atkGetDb();
    $days = array("sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday");
    
    $data = $db->getrows("SELECT 
                              date_format(hours.activitydate, '%w') as dow, SUM(time) as total
                            FROM
                              hours, activity
                            WHERE
                              hours.activityid = activity.id ".
                              ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                              ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                              ($params["employeeid"]!=""?" AND hours.userid = ".$params["employeeid"]:"")."
                            GROUP BY 
                              date_format(hours.activitydate, '%w')
                            ORDER BY 
                              dow");
                          
    // Add weekday names.
    for ($i=0, $_i=count($data); $i<$_i; $i++)                            
    {
      $data[$i]["dow"] = text($days[$data[$i]["dow"]]);
    }
                          
    // In raw mode the data is sufficient.
    if ($raw) return $data;

    // convert records to graph-compatible array.
    $dataset = array();

    for ($i=0, $_i=count($data); $i<$_i; $i++)
    {
      $dataset[$data[$i]["dow"]] = $data[$i]["total"];
    }

    return array("registeredtimeperweekday"=>$dataset);
  }
  
  function rowColor($record)
  {            
    $contractnode = &getNode("employee.usercontracts");
    $contract = $contractnode->getMostRecentContract($record["id"]);                
    
    if ($record["status"]=="active")
    {        
      $enddate = dateutil::arr2str($contract["enddate"]);                      
      
      if (!count($contract)||($enddate!=""&&$enddate<date("Ymd")))
      {
        return COLOR_ERROR;
      }
      else if ($enddate!=""&&dateutil::date_diff(date("Ymd"), $enddate)<14)
      {
        return COLOR_WARNING;
      }
    }
  }
        

}

?>
