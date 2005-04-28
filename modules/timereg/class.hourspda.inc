<?php

include("class.hours.inc");
useattrib("atklistattribute");

class hoursPDA extends hours
{
  var $m_projects;
  var $m_textprojects;

  var $m_phaseids;
  var $m_textphaseids;

  var $m_activityids;
  var $m_textactivityids;

  var $m_node_errors;

  function _fillProject()
  {
    global $g_project;

    $projectnode = &getNode("project.project");
    $projects = $projectnode->selectDb(null,null,null,array("todos","notes","members","contacts"));

    $this->m_projects["EMPTY"] = $this->removeVarFromCurrentURL(array("g_project", "g_phase", "g_activity"));;

    foreach ($projects as $project)
    {
      if (!empty($project["phase"]))
      {
        $this->m_textprojects[] = $project['name'];
        $addedurl = $this->addVarToCurrentURL(array("g_project"=>$project["id"]));

        if ($project["id"]!==$g_project)
        {
          $this->m_projects[] = $this->removeVarFromCurrentURL(array("g_phase","g_activity"), $addedurl);
        }
        else
        {
          $this->m_projects[] = $this->addVarToCurrentURL(array("g_project"=>$project["id"]));
        }
      }
    }
  }

  function _fillPhaseId()
  {
    global $g_project, $g_phase;

    $phasenode = &getNode("project.phase");

    $phases = $phasenode->selectDb("projectid.id='$g_project'");

    $this->m_phaseids["EMPTY"] = $this->removeVarFromCurrentURL(array("g_phase", "g_activity"));;

    foreach ($phases as $phase)
    {
      $this->m_textphaseids[] = $phase['name'];
      $addedurl = $this->addVarToCurrentURL(array("g_phase"=>$phase["id"]));

      if ($phase["id"]!==$g_phase)
      {
        $this->m_phaseids[] = $this->removeVarFromCurrentURL(array("g_activity"), $addedurl);
      }
      else
      {
        $this->m_phaseids[] = $this->addVarToCurrentURL(array("g_phase"=>$phase["id"]));
      }
    }
  }

  function _fillActivityId()
  {
    global $g_project, $g_phase, $g_activity;

    $activitynode = &getNode("project.phase_activity");
    $activities = $activitynode->selectDb("phase_activity.phaseid='$g_phase'");

    $this->m_activityids["EMPTY"] = $this->removeVarFromCurrentURL(array("g_activity"));

    foreach ($activities as $activity)
    {
      $this->m_textactivityids[] = $activity['activityid']['name'];
      $addedurl = $this->addVarToCurrentURL(array("g_activity"=>$activity['activityid']["id"]));
      $this->m_activityids[] = $addedurl;
    }
  }

  function _initialize()
  {
    $this->_fillProject();
    $this->_fillPhaseId();
    $this->_fillActivityId();
  }

  function _presetGlobals()
  {
    global $g_project, $g_activity, $g_phase, $ATK_VARS;

    if (($ATK_VARS["atkaction"] == "add" || $ATK_VARS["atkaction"] == "admin") && $ATK_VARS["atknodetype"]=="timereg.hourspda")
    {
      if (!$g_activity)
      {
        if ($g_phase && $g_project)
        {
          if ($this->m_activityids[0])  header("Location: ".$this->m_activityids[0]);
        }
        else if (!$g_phase)
        {
          if ($g_project)
          {
            if ($this->m_phaseids[0]) header("Location: ".$this->m_phaseids[0]);
          }
          else
          {
            if ($this->m_projects[0]) header("Location: ".$this->m_projects[0]);
          }
        }
      }
    }
  }

  function hoursPDA()
  {
    $errors = sessionLoad("atkerror");
    $this->_initialize();
    $this->_presetGlobals();
    $this->setFeedback("save", ACTION_SUCCESS);

    $this->m_type = "hourspda";
    if (is_array($errors)
    )
    {
      $this->add(new atkDummyAttribute("formdataerror", "<b>".text("error_formdataerror")."</b>"));

      foreach ($errors as $error)
      {
        if ($error['attrib_name'])
        $this->add(new atkDummyAttribute($error['attrib_name']."_error",$error['msg']));
      }
      $this->add(new atkDummyAttribute("spacer","&nbsp;"));
      sessionStore("atkerror","nothing");
      sessionStore("atkerror_record","nothing");
    }
    $this->hours();
    $this->m_flags-= NF_MRA;
    $this->m_flags|= NF_NO_EDIT;

    $this->m_type = "hourspda";
    $this->setSecurityAlias("timereg.hours");
  }

  function adminHeader()
  {
  }

  function addPhaseAndActivity()
  {
    global $ATK_VARS, $g_project, $g_phase, $g_activity, $g_sessionManager;

    if (($ATK_VARS["atkaction"]=="add" || $ATK_VARS["atkaction"]=="edit" || $ATK_VARS["atkaction"]=="admin"))
    $this->_addSpecialListAttribute("project","g_project", AF_OBLIGATORY);
    if ($g_project || ($ATK_VARS["atkaction"]!=="add" || $ATK_VARS["atkaction"]!=="edit"))
    $this->_addSpecialListAttribute("phaseid","g_phase", AF_OBLIGATORY);
    if ($g_phase || ($ATK_VARS["atkaction"]!=="add" || $ATK_VARS["atkaction"]!=="edit"))
    $this->_addSpecialListAttribute("activityid","g_activity", AF_OBLIGATORY);
  }

  // override the admin action, because we have the weekview/dayview switch..
  function action_admin()
  {
    global $atkorderby;
    $this->setLock();

    $this->addFilter("hours.userid",$this->m_user);

    if ($this->m_weekview!=1)
    {
      if ($this->m_lock=="" && !(browserInfo::detectPDA())) $this->addPage();

      // Set the viewnode up
      $viewnode = &getNode("timereg.hours");
      $viewnode->m_action = "admin";

      if (!$atkorderby) $atkorderby = $this->m_default_order;
      $viewnode->addFilter("activitydate",$this->m_viewdate);
      $viewnode->addFilter("hours.userid",$this->m_user);
      $viewnode->m_flags-=NF_MRA;
      $viewnode->m_flags|=NF_NO_EDIT;

      $viewnode->setOrder($atkorderby);
      $viewnode->m_postvars = $this->m_postvars;
      $viewnode->adminPage();
    }
    else
    {
      if (atkconfig("timereg_week_bookable")==true && $this->m_lock=="") $this->addPage();
      $this->weekview();
    }
  }

  function preAdd(&$record)
  {
    global $g_sessionManager;
    parent::preAdd($record);

    $url = $record['activityid'];
    $this->_urlToDb($record, "project", "g_project");
    $this->_urlToDb($record, "phaseid", "g_phase");
    $this->_urlToDb($record, "activityid", "g_activity");

    if ($record["atkerror"])
    {
      sessionStore("atkerror",$record["atkerror"]);
      sessionStore("atkerror_record", $record);
      header("Location: $url");exit;
    }
  }

  /**
  * (evil?) Hack to ensure that, when we edit the overridden strings, 
  * that we get the original values in the listattributes from the record
  * @param array $record the record of the overridden string
  * @return array an empty array
  */
  function edit_values($record)
  {
    parent::edit_values($record);
    global $ATK_VARS, $g_project, $g_phase, $g_activity, $g_sessionManager;

    if (!$record['project']) $project = $g_sessionManager->stackVar("g_project");
    else $project = $record['project'];

    if (!$record['phaseid']) $phase = $g_sessionManager->stackVar("g_phase");
    else $phase = $record['phaseid'];

    if (!$record['activityid']) $activity = $g_sessionManager->stackVar("g_activity");
    else $activity = $record['activityid'];

    //    $g_otherone_session  = $g_sessionManager->stackVar("g_otherone");
    $atknodetype = $g_sessionManager->stackVar("atknodetype");

    if ($ATK_VARS["atkaction"] == "edit" && $atknodetype=="timereg.hourspda")
    {
      if (!$g_project && $project)
      {
        if (!$g_phase && $phase)
        {
          if (!$g_activity && $activity)
          {
            $url = $this->addVarToCurrentURL(array("g_activity"=>$activity));
            header("Location: $url"); exit;
          }
        }
        else
        {
          $url = $this->addVarToCurrentURL(array("g_phase"=>$phase));
          header("Location: $url");exit;
        }
      }
      else
      {
        $url = $this->addVarToCurrentURL(array("g_project"=>$project));
        header("Location: $url");exit;
      }
    }
    return array();
  }

  /////////////////// speciallist functions //////////////////////////////////////
  /*
  * To add a special list attribute, you must take the following steps:
  * 1. Add member variables (m_{fieldname}s && m_text{fieldname}s)
  * 2. Add an _fill{fieldname} function to fill the member variables and add it to _initialize()
  * 3. Add the special list attribute to the constructor
  * 4. Add a _urlToDb to the preAdd
  * 5. Add it to the edit_values function so that when the record is editted that it does a redirect
  * 6. Add it to the presetGlobals() so it's preset in the beginning
  */

  /**
	* Special function that will add the special pfmllistattributes with an onChange handler
	* and call the setSelected function
	* @param string $name       the name of the attribute
	* @param string $globalvar  the name of the global variable we use for the attribute
	* @param string $extraflags extra flags to be added to the special list attribute
	*/
  function _addSpecialListAttribute($name, $globalvar, $extraflags=NULL, $label="", $removeonchange = false)
  {
    global ${$globalvar};
    $varids = "m_".$name."s";
    $vartexts = "m_text".$name."s";

    $flags = AF_HIDE_LIST|AF_HIDE_SELECT;
    if ($extraflags)
    {
      $flags |= $extraflags;
    }

    if ($this->{$varids}["EMPTY"])
    {
      $emptyvalue = $this->{$varids}["EMPTY"];
      unset($this->{$varids}["EMPTY"]);
    }

    if(!$label)
    $label = $name;

    $temp = new atkListAttribute($label, $this->{$vartexts}, $this->{$varids}, $flags);
    if (!$removeonchange) $temp->addOnChangeHandler("self.location.href=newvalue;");
    $temp->setSelected($this->addVarToCurrentURL(array($globalvar => ${$globalvar})));
    $temp->setEmptyValue($emptyvalue);
    $this->add($temp);
  }

  /**
	* Adds a variable to the current URL and returns the new URL
	* @param string $variables      the name of the variable to be added
	*                               the format is:
	*                                 array("var1name"=>"var1value",
	*                                       "var1name_default"=>"defaultvalue",
	*                                       "var2name"=>"var2value");
	* @param string $url            the url to use if we want to use a different
	*                               url than the current one
	* @return string the changed URL with the extra variable
	*/
  function addVarToCurrentURL($variables, $url=null)
  {
    global $PHP_SELF, $HTTP_SERVER_VARS;

    if (!$url)
    {
      if($HTTP_SERVER_VARS["REQUEST_URI"] == "")
      $HTTP_SERVER_VARS["REQUEST_URI"] = $_SERVER["URL"]."?".$_SERVER["QUERY_STRING"];

      $protocol = $this->getProtocol();
      $cururl = $protocol."://".$HTTP_SERVER_VARS["HTTP_HOST"].
      substr($HTTP_SERVER_VARS['REQUEST_URI'], 0, strpos($HTTP_SERVER_VARS['REQUEST_URI'], "?"));
      $urlvars = substr($HTTP_SERVER_VARS['REQUEST_URI'], strpos($HTTP_SERVER_VARS['REQUEST_URI'], "?")+1);
    }
    else
    {
      $cururl = substr($url, 0, strpos($url, "?"));
      $urlvars = substr($url, strpos($url, "?")+1);
    }
    $urlvars = $this->_getURLVars($urlvars);

    foreach ($variables as $varname => $varvalue)
    {
      if (substr($varname, -8) !== "_default")
      {
        if ($varvalue)
        {
          $urlvars[$varname]=$varvalue;
        }
        else if ($variables[$varname."_default"])
        {
          $urlvars[$varname] = $variables[$varname."_default"];
        }
      }
    }
    return $this->_createURL($cururl,$urlvars);
  }

  /**
	* Removes a variable from the current URL and returns the new URL
	* @param mixed $varname      the name or names of the variable(s) to be removed
	*                             if you pass along an array it will remove every value
	*                             mentioned in it
	* @param string $url         the url to use if we want to use another url than the
	*                             current one
	* @return string the changed URL without the variable
	*/
  function removeVarFromCurrentURL($varname, $url=null)
  {
    global $PHP_SELF, $HTTP_SERVER_VARS;

    if (!$url)
    {
      if($HTTP_SERVER_VARS["REQUEST_URI"] == "")
      $HTTP_SERVER_VARS["REQUEST_URI"] = $_SERVER["URL"]."?".$_SERVER["QUERY_STRING"];

      $protocol = $this->getProtocol();
      $cururl = $protocol."://".$HTTP_SERVER_VARS["HTTP_HOST"].
      substr($HTTP_SERVER_VARS['REQUEST_URI'], 0, strpos($HTTP_SERVER_VARS['REQUEST_URI'], "?"));
      $urlvars = substr($HTTP_SERVER_VARS['REQUEST_URI'], strpos($HTTP_SERVER_VARS['REQUEST_URI'], "?")+1);
    }
    else
    {
      $cururl = substr($url, 0, strpos($url, "?"));
      $urlvars = substr($url, strpos($url, "?")+1);
    }
    $urlvars = $this->_getURLVars($urlvars);

    if (is_array($varname))
    {
      foreach ($varname as $var)
      {
        if ($urlvars[$var]) unset($urlvars[$var]);
      }
    }
    else if ($urlvars[$varname]) unset($urlvars[$varname]);

    return $this->_createURL($cururl,$urlvars);
  }

  function getProtocol()
  {
    global $HTTP_SERVER_VARS;
    $http = explode("://",$HTTP_SERVER_VARS['HTTP_REFERER']);
    if (!$http[0])
    {
      // Crappy IE doesn't send along a referrer
      if ($HTTP_SERVER_VARS["HTTPS"]=="on" || $HTTP_SERVER_VARS['HTTP_X_SSL_CIPHER']) $protocol = "https";
      else $protocol = "http";
    }
    else
    {
      $protocol = $http[0];
    }
    return $protocol;
  }

  /**
	* Turns a single string with the url vars (name=value&somevar=test)
	* into an array (["name"]=>"value",["somevar"]=>"test")
	* @param string $urlvars the variables in URL format
	* @return array the array with the URL variables
	*/
  function _getURLVars($urlvars)
  {
    $urlvars = explode("&", $urlvars);
    foreach ($urlvars as $urlvarname => $urlvarvalue)
    {
      unset($urlvars[$urlvarname]);
      $values = explode("=",$urlvarvalue);
      $urlvars[$values[0]] = $values[1];
    }
    return $urlvars;
  }

  /**
	* Creates a URL with page of $cururl and vars (in array form)
	* @param string $cururl the page where we need to add the urlvars to
	* @param array $urlvars the array with every url variable
	* @return the URL with the variables
	*/
  function _createURL($cururl, $urlvars)
  {
    foreach ($urlvars as $urlvarname => $urlvarvalue)
    {
      if ($urlvarname!='')
      {
        $urlvarstring.= "$urlvarname=$urlvarvalue&";
      }
    }
    $newurl = $cururl."?".$urlvarstring;

    if (!$urlvars["atkstackid"])
    return session_url($newurl);
    else
    return $newurl;
  }

  /**
	* Because we use the listattributes in a non-standard way,
	* the values have to be converted back to normal values
	* before we they get written to the database
	* @param object &$record   the record we have to alter
	* @param string $variable  the variable that we need to alter
	* @param string $globalvar the variable name in the uri
	* @param bool   $relation  wether or not the var is a relation
	*/
  function _urlToDb(&$record, $variable, $globalvar, $relation=false)
  {
    $url = $record[$variable];

    $urlvars = substr($url, strpos($url, "?")+1);
    $urlvars = $this->_getURLVars($urlvars);

    if ($relation) $record[$variable] = array('id'=>$urlvars[$globalvar]);
    else $record[$variable] = $urlvars[$globalvar];
  }

  ////////////////// end speciallist functions //////////////////
}

?>