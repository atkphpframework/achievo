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

class employee extends person
{
  function employee()
  {
    global $ATK_VARS,$g_user;

    $this->person("employee", NF_EDITAFTERADD|NF_IMPORT);

    // In employee edit screen, we hide most fields in the add screen.
    foreach (array_keys($this->m_attribList) as $attribname)
    {
      if (!in_array($attribname, array("userid","lastname", "firstname","email")))
      {
        $p_attrib = &$this->getAttribute($attribname);
        $p_attrib->m_flags|=AF_HIDE_ADD;
      }
      elseif($attribname=="userid")
      {
        $p_attrib = &$this->getAttribute($attribname);
        $p_attrib->m_flags|=AF_OBLIGATORY;
      }
    }

    $this->addFilter("person.role='employee'");
    $this->add(new atkDateAttribute("birthdate","","", 0, date("Ymd"), AF_HIDE_LIST));

    $this->add(new atkAttribute("bankaccount", AF_HIDE_LIST|AF_HIDE_ADD), "finance");
    $this->add(new atkAttribute("socialsecuritynumber", AF_HIDE_LIST|AF_HIDE_ADD), "finance");

    $this->add(new atkManyToOneRelation("functionlevel", "employee.functionlevel",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK));
    $this->add(new atkManyToOneRelation("department","employee.department",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK));
    $this->add(new atkManyToOneRelation("creditor_id","organization.organization",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK),"finance");

    //$this->add(new atkManyToOneRelation("entity", "employee.profile",AF_RELATION_AUTOLINK|AF_HIDE_ADD));
    $rel = &$this->add(new atkManyBoolRelation("profiles", "employee.employeerole", "employee.profile", AF_HIDE_LIST|AF_MANYBOOL_AUTOLINK));
    $rel->setLocalKey("employee_id");
    $rel->setRemoteKey("role_id");

    // you can't be your own supervisor neither can a nonactive employee be a supervisor
    $preFilter = new atkManyToOneRelation("supervisor","employee.employee",AF_HIDE_ADD|AF_HIDE_LIST|AF_RELATION_AUTOLINK);
    $preFilter->setDestinationFilter("userid !='[userid]' AND status ='active'");

    $this->add($preFilter);
    // If we are in employee administration mode, we show all users. Otherwise, we only show active users.
    $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE_LIST|AF_HIDE_ADD|AF_FORCE_LOAD));
    if ((atkArrayNvl($ATK_VARS, "atknodetype", "") == "employee.employee") && in_array(atkArrayNvl($ATK_VARS, "atkaction", ""), array("admin", "add", "update", "edit")))
    {
      $statusfilter = ($this->getView() == "active") ? "active" : "nonactive";
      $this->addFilter("person.status",$statusfilter);
    }
    $this->add(new atkOneToManyRelation("usercontract","employee.usercontracts","userid",AF_HIDE_LIST|AF_HIDE_ADD));

    //$this->add(new useridAttribute("userid" ,AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE), 1); // first field

    if (in_array("db",explode(",",strtolower(atkconfig('authentication')))))
      $this->addPasswordAttribute();

    $this->add(new atkAttribute("role", AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15)); // discriminator.

    $this->m_securityMap["deactivate"] = "edit"; // If you may edit, you may also deactivate.

    $this->setOrder("lastname");
    $this->setDescriptorTemplate("[lastname], [firstname] ([userid])");
  }

  function addPasswordAttribute()
  {
    $this->add(new atkDummyAttribute("passwordexplanation", atktext("password_leaveblankifunchanged"), AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW));
    $passwordrestrictions = array(
      "minsize" => atkconfig("password_minsize", 0),
      "minupperchars" => atkconfig("password_minupperchars", 0),
      "minlowerchars" => atkconfig("password_minlowerchars", 0),
      "minalphabeticchars" => atkconfig("password_minalphabeticchars", 0),
      "minnumbers" => atkconfig("password_minnumbers", 0),
      "minspecialchars" => atkconfig("password_minspecialchars", 0)
    );
    $this->add(new atkPasswordAttribute("password", false, AF_HIDE_LIST|AF_PASSWORD_NOVALIDATE, 40, $passwordrestrictions));
  }

  function initial_values()
  {
    $initial = array();
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
      return atktext("onlyactiverecordsareshown")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("view"=>"nonactive")),
                                                           atktext('clicktoviewarchive', $this->m_module, $this->m_type))."<br>";
    }
    else
    {
      return atktext("youareinthearchive")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("view"=>"active")),
                                                    atktext('clicktoviewactiverecords', $this->m_module, $this->m_type))."<br>";
    }
  }

  function recordActions($rec, &$actions, &$mraactions)
  {
    // Get the userid of the user currently logged in
    $user = getUser();
    $userid = atkArrayNvl($user, "id", -1);

    // Actve users may be deactivated, but not the logged in user.
    if ($rec["status"] == "active" && $rec["id"]!=$userid)
    {
      $actions["deactivate"] = session_url(dispatch_url($this->atkNodeType(),"deactivate",array("atkselector"=>"[pk]")), SESSION_NESTED);
    }

    // Prevent users from deleting themselves when they are logged in.
    if($rec["id"] == $userid)
    {
      unset($actions["delete"]);
    }
  }

  function action_deactivate(&$handler)
  {
    $select = $this->m_postvars["atkselector"];
    $query = "UPDATE person SET status='nonactive' WHERE $select";

    // Verify that the selector or id were not tampered with before passing on to the db
    if(preg_match("/person\.id='\d+'/i", $select) === 1)
    {
      $db = &atkGetDb();
      $db->query($query);
    }
    else
    {
      // TODO: give the name of the module, the logged userid and the IP address
      // of the offending user in the debug entry so it gets logged properly
      atkdebug("SQL injection attack detected in 'action_deactivate', not executed. The selector was '$select' and the resulting query would have been '$query'", true);
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
                              project.id,project.name
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
      $data[$i]["dow"] = atktext($days[$data[$i]["dow"]]);
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
    if ($record["status"]=="active")
    {
      $contractnode = &atkGetNode("employee.usercontracts");

      $endlesscontracts = $contractnode->countDb("usercontract.enddate IS NULL AND usercontract.userid='{$record["id"]}'");

      if ($endlesscontracts == 0)
      {
        $contract = $contractnode->getMostRecentContract($record["id"]);

        $enddate = isset($contract["enddate"]) ? (dateutil::arr2str($contract["enddate"])) : "";

        if (!count($contract)||($enddate!=""&&$enddate<date("Ymd")))
        {
          return array(COLOR_ERROR, COLOR_HIGHLIGHT_ERROR);
        }
        else if ($enddate!=""&&dateutil::date_diff(date("Ymd"), $enddate)<14)
        {
          return array(COLOR_WARNING, COLOR_HIGHLIGHT_WARNING);
        }
      }
    }
  }

  function postAdd($record)
  {
    $this->removeBirthdaysCache();
    return true;
  }

  function postUpdate($record)
  {
    $this->removeBirthdaysCache();
    return true;
  }

  function postDel($record)
  {
    $this->removeBirthdaysCache();
    return true;
  }

  /**
   * Remove birthdays cache file
   */
  function removeBirthdaysCache()
  {
    atkimport("atk.utils.atktmpfile");
    $file = new atkTmpFile("employee_birthdays.inc");
    if($file->exists())
      $file->remove();
  }

  /**
   * Get birthdays from all employees
   *
   * @return array Array with all employees birthdays
   */
  function getBirthdates()
  {
    atkimport("atk.utils.atktmpfile");
    $file = new atkTmpFile("employee_birthdays.inc");
    if(!$file->exists())
    {
      $birthday_recs = $this->selectDb("birthdate IS NOT NULL AND status='active'","month(birthdate), dayofmonth(birthdate)","","",array("firstname","lastname","birthdate"));
      $items = array();
      foreach($birthday_recs as $birthday)
      {

        $day = intval($birthday["birthdate"]["day"]);
        $month = intval($birthday["birthdate"]["month"]);
        $name = $birthday["firstname"]." ".$birthday["lastname"];
        atkdebug("$day - $month - $name");
        $items[$month][$day][] = array("name"=>$name,"birthday"=>$birthday["birthdate"]);
      }

      $file->writeAsPhp("birthdays",$items);
    }
    else
    {
      include $file->getPath();
    }

    return $birthdays;
  }


}

?>
