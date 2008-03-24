<?php
  /**
   * Module scheduler
   * Class schedule
   *
   * This class makes it possible to schedule a appointment.
   *
   * $Id$
   * @author Sandy Pleyte <sandy@achievo.org>
   * @version $Revision$
   * @todo $scheduler->isAvailable("sandy", "2003-10-28", "11.00", "13.00",30);
   *       heeft sandy op 28 oktober tussen 11 en 13 minimaal een half uur vrij in zijn agenda
   */
  useattrib("atknumberattribute");
  useattrib("atkdateattribute");
  useattrib("atktextattribute");
  useattrib("atkboolattribute");
  useattrib("atktimeattribute");
  useattrib("atklistattribute");
  useattrib("atkdummyattribute");
  useattrib("atkflagattribute");
  useattrib("atkdurationattribute");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  userelation("atkonetoonerelation");
  useattrib("atkfuzzysearchattribute");
  useattrib("atkradioattribute");
  atkimport("modules.scheduler.utils.schedulertools");
  useattrib("scheduler.dummydurationattribute");


  class scheduler extends atkNode
  {
    var $m_viewdate;
    var $m_small_viewdate;
    var $m_viewUsers = array();
    var $m_view;
    var $m_employees;
    var $m_itemDate;
    var $m_scheduler_id;
    var $m_confirmsingle;

    // Default minute step
    var $m_minute_step = 5;


    /**
     * Constructor
     *
     * @todo Fix alarms
     * @return scheduler
     */
    function scheduler($type = "scheduler", $flags = 0)
    {
      $flags |= NF_ADD_LINK|NF_TRACK_CHANGES;
      $sessionmanager = &atkGetSessionManager();
      $this->m_view = $sessionmanager->pageVar("view");
      $this->m_viewUsers = $sessionmanager->stackVar("user");
      $this->m_employees = $sessionmanager->pageVar("employee");
      $this->m_itemDate = $sessionmanager->stackVar("itemdate");
      $this->m_scheduler_id = $sessionmanager->stackVar("scheduler_id");
      $this->m_confirmsingle = $sessionmanager->stackVar("confirmsingle");

      $userid = atkArrayNvl(getUser(), "id");
      $userprefs = (!empty($userid)) ? $this->getUserSchedulerPrefs($userid) : array();

      if(isset($userprefs["default_eventtime"]))
      {
        $eventtime = $userprefs["default_eventtime"];
        $user_eventtime = ($eventtime["hours"]*60)+$eventtime["minutes"];
      }
      else
      {
        $user_eventtime = 60; // 1 hour
      }

      $time = $sessionmanager->stackVar("time");
      for($i=0;$i<60;$i+=$this->m_minute_step)
      {
        $m_arr_minute_step[] = sprintf("%02d",$i);
      }
      if ($time!="")
      {
        $default_start_time = $time; // passed from the url.
        $default_end_time = strftime("%H:%M", mktime(substr($time,0,2), substr($time,3,2)+$user_eventtime)); // endtime is half an hour later than starttime by default
      }
      else
      {
        $default_start_time = strftime("%H:%M");
        $default_end_time = strftime("%H:%M", mktime(substr($default_start_time,0,2),substr($default_start_time,3,2)+$user_eventtime));
      }

      $this->atkNode($type,$flags);

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("title", AF_SEARCHABLE|AF_OBLIGATORY, 50));
      $this->add(new atkOneToManyRelation("attendees", "scheduler.scheduler_attendees", "scheduler_id", AF_HIDE_LIST));
      $this->add(new atkDummyAttribute("hint", $this->text("attendee_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW));
      $this->add(new atkfuzzySearchAttribute("attendee_empsearch", "employee.employee", "storeEmployees", "multiselect"));
      // Only add contact search if they have contact admin rigts
      if(is_allowed("organization.contats","admin"))
        $this->add(new atkfuzzySearchAttribute("attendee_contactsearch", "organization.contact", "storeContacts", "multiselect"));
      $this->add(new atkAttribute("location", AF_SEARCHABLE, 50));
      $this->add(new atkTextAttribute("description"),'description');
      $this->add(new atkDummyAttribute("recur_info",'',AF_NO_LABEL),'cyclus');
      $this->add($recur_attr = new atkListAttribute("recur",array("once","daily","weekly","monthly","yearly"),array(), AF_LIST_NO_NULL_ITEM), "cyclus");
      $this->add(new atkDateAttribute("startdate", "F d Y","d F Y", 0, 0));
      $this->add(new atkDateAttribute("enddate", "F d Y","d F Y", 0, 0,AF_FORCE_LOAD))->setManualPaint(true);
      $this->add($duration_attr = new dummydurationattribute('duration'));
      $this->add(new atkBoolAttribute("allday"));
      $this->add(new atkTimeAttribute("starttime", 0, 23, $m_arr_minute_step, $default_start_time,AF_FORCE_LOAD))->setManualPaint(true);
      $this->add(new atkTimeAttribute("endtime", 0, 23, $m_arr_minute_step, $default_end_time, AF_FORCE_LOAD))->setManualPaint(true);
      $this->add(new atkOneToOneRelation("cyclus","scheduler.scheduler_cyclus","scheduler_id",AF_ONETOONE_INTEGRATE|AF_ONETOONE_ERROR|AF_CASCADE_DELETE|AF_FORCE_LOAD),"cyclus");
      $this->add(new atkManyToOneRelation("category", "scheduler.scheduler_category",AF_OBLIGATORY|AF_RELATION_AUTOLINK));
      $this->add(new atkBoolAttribute("all_users"));
      $this->add(new atkListAttribute("priority",array("","low","normal","high"),array(0,1,2,3),AF_LIST_NO_NULL_ITEM));
      $this->add(new atkBoolAttribute("private"));
      $this->add(new atkBoolAttribute("nonblocking"));
      $this->add(new atkOneToManyRelation("alarms", "scheduler.scheduler_alarms", "scheduler_id", AF_HIDE_LIST|AF_HIDE_ADD|AF_CASCADE_DELETE), "alarms"); // Disabled, not working yet
      $this->add(new atkOneToManyRelation("notes", "scheduler.scheduler_notes", "scheduler_id", AF_HIDE_LIST|AF_HIDE_ADD|AF_CASCADE_DELETE), "notes");
      $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_READONLY|AF_OBLIGATORY));
      $this->add(new atkBoolAttribute("owner_attendee"));
      $this->add(new atkNumberAttribute("lastdate", AF_HIDE));
      $this->add(new atkNumberAttribute("times", AF_HIDE));

      //$url = partial_url('scheduler.scheduler_cyclus','displaycyclus','displaycyclus');
      $recur_attr->addOnChangeHandler("change_recur(newvalue);");

      $duration_attr->addOnChangeHandler('change_duration(newvalue)');

      if($this->m_viewUsers=="" && $this->m_view!="employeeview")
      {
        if ($userprefs["default_userview"]!="*") // &&  substr($userprefs["default_userview"],0,2)!='g_' && !empty($userid))
        {
          $this->m_viewUsers = array($userprefs["default_userview"]);
        }
        else
        {
         // User has opted to show all users by default.
          $this->m_viewUsers = array("all");
        }
      }

      if (trim($this->m_view) == "")
      {
         $this->m_view = (isset($userprefs["default_view"])?$userprefs["default_view"]:"day");
      }

      $this->m_viewdate = $sessionmanager->stackVar("viewdate");
      $this->m_small_viewdate = $sessionmanager->stackVar("small_viewdate");

      if(is_array($this->m_viewdate))
      {
        $this->m_viewdate = sprintf('%04d-%02d-%02d',$this->m_viewdate['year'],$this->m_viewdate['month'],$this->m_viewdate['day']);
      }
      elseif ($this->m_viewdate=="")
      {
        $this->m_viewdate = strftime("%Y-%m-%d");
      }

      $this->addAllowedAction("view");
      $this->addAllowedAction("editserieorsingle");
      $this->addAllowedAction("editcopy");

      $this->setTable("scheduler_scheduler", "scheduler_scheduler");
      if ($this->m_view == "day")
      {
        $this->setOrder("startdate DESC");
      }
      else
      {
        $this->setOrder("startdate DESC, starttime");
      }
    }

    function startdate_edit($record="", $fieldprefix="",$mode)
    {
      $attrib = $this->getAttribute("startdate");
      $ret = $attrib->edit($record,$fieldprefix,$mode);
      $ret.='&nbsp;&nbsp;&nbsp;<span id="starttime" style="display: inline;">';
      $attrib = $this->getAttribute("starttime");
      $ret.= $attrib->edit($record,$fieldprefix,$mode)."</span>";
      return $ret;
    }

    function startdate_display($record,$mode)
    {
      $attrib = $this->getAttribute("startdate");
      $ret = $attrib->display($record,$mode);
      if(intval($record['allday'])==0)
      {
        $ret.='&nbsp;&nbsp;&nbsp;';
        $attrib = $this->getAttribute("starttime");
        $ret.= $attrib->display($record)."</span>";
      }
      return $ret;
    }

    function duration_edit($record="",$fieldprefix="",$mode)
    {
      $attrib = $this->getAttribute("duration");
      $ret.= $attrib->edit($record,$fieldprefix,$mode);
      $ret.='&nbsp;&nbsp;&nbsp;<span id="enddate" style="display: none;">';
      $attrib = $this->getAttribute("enddate");
      $ret.= $attrib->edit($record,$fieldprefix,$mode);
      $ret.='</span>&nbsp;&nbsp;&nbsp;<span id="endtime" style="display: none;">';
      $attrib = $this->getAttribute("endtime");
      $ret.= $attrib->edit($record,$fieldprefix,$mode);
      $ret.='</span>';
      return $ret;
    }

    function duration_display($record,$mode)
    {

      if(intval($record['allday'])==0)
      {
        $attrib = $this->getAttribute("duration");
        $ret.= $attrib->display($record,$mode);
        if($record['duration']==-1)
        {
          $ret.='&nbsp;&nbsp;&nbsp;';
          $attrib = $this->getAttribute("enddate");
          $ret.= $attrib->display($record,$mode);

          $ret.='&nbsp;&nbsp;&nbsp;';
          $attrib = $this->getAttribute("endtime");
          $ret.= $attrib->display($record,$mode);
        }
      }
      else
      {
        $ret = '--';
      }
      return $ret;
    }

    /**
     * We override the display of notes, because attendees do have the possibility to
     * add nodes even when they can't edit the scheduler item
     *
     * @param array $record Scheduler item record
     * @param string $mode Mode
     * @return string notes edit
     */
    function notes_display($record,$mode)
    {
      $attr = &$this->getAttribute("notes");
      if ($mode=="view") // in view mode we don't want readonly display
                         // of the notes
      {
          $page = &$this->getPage();
          $page->register_script('./atk/javascript/formsubmit.js');
          return $attr->edit($record,'', 'edit');
      }
      else
      {
         // regular situation
         return $attr->display($record, $mode);
      }
    }


    /**
     * Override alarms display, because we want the edit mode
     * in the mode is view
     *
     * @param array $record
     * @param string $mode
     * @return string
     */
    function alarms_display($record,$mode)
    {
      $attr = &$this->getAttribute("alarms");
      if ($mode=="view") // in view mode we don't want readonly display
                         // of the notes
      {
          $page = &$this->getPage();
          $page->register_script('./atk/javascript/formsubmit.js');
          return $attr->edit($record);
      }
      else
      {
         // regular situation
         return $attr->display($record, $mode);
      }
    }

    /**
     * Set some initial values
     *
     * @return unknown
     */
    function initial_values()
    {
      $user = atkgetUser();
      $initial["owner"]["id"] = $user["id"];
      $initial["private"] = 0;
      $initial["status"] = "unconfirmed";
      $initial["recur"]="once";
      $initial["owner_attendee"]=1;
      if ($this->m_viewdate!="")
      {
        $arr_date = array("year"=>substr($this->m_viewdate, 0, 4),
                          "month"=>substr($this->m_viewdate, 5, 2),
                          "day"=>substr($this->m_viewdate, 8, 2));
        $initial["startdate"] = $arr_date;
        $initial["enddate"] = $arr_date;
      }
      $userid = atkArrayNvl(getUser(), "id");
      $userprefs = (!empty($userid)) ? $this->getUserSchedulerPrefs($userid) : array();

      if(isset($userprefs["default_eventtime"]))
      {
        $initial['duration']=(intval($userprefs["default_eventtime"]['hours'])*60)+$userprefs["default_eventtime"]['minutes'];
      }
      return $initial;
    }

    /**
     *
     * owner = view private, edit, delete, view
     * attendee = view private, view
     * assistant = edit, delete, view
     *
     * @param string $action The action we want to allow
     * @param array $record The record for the action
     * @return boolean Are we allowed to execute the action
     */
    function allowed($action,$record="")
    {
      if(is_array($record))
      {
        atkimport("modules.scheduler.utils.schedulertools");
        $userid = atkGetUserId();
        $owner = $record["owner"]["id"];
        $isPrivate = $record["private"];
        $isAttendee = schedulertools::isAttendee($record["id"],$userid);
        $assistantFor = schedulertools::assistantFor($userid);

        if((!$isPrivate && (in_array($owner,$assistantFor) || parent::allowed("all_non_private"))) || $owner==$userid || ($action=="view" && $isAttendee))
        {
          // Item is not private and user is the owner or an assistant for the owner
          // or action is view and user is an attendee
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return parent::allowed($action,$record);
      }
    }

    /**
     * Discriptor for this scheduler item
     *
     * @return unknown
     */
    function descriptor_def()
    {
      return "[title]";
    }

    /**
     * Validate enddate
     *
     * @param unknown_type $record
     * @param unknown_type $mode
     */
    function enddate_validate(&$record, $mode)
    {
      if ($record["enddate"]["year"] < $record["startdate"]["year"])
      {
        triggerError($record, "enddate", "error_date");
      }
      elseif ($record["enddate"]["year"] == $record["startdate"]["year"])
      {
        if ($record["enddate"]["month"] < $record["startdate"]["month"])
        {
          triggerError($record, "enddate", "error_date");
        }
        elseif ($record["enddate"]["month"] == $record["startdate"]["month"])
        {
          if ($record["enddate"]["day"] < $record["startdate"]["day"])
          {
            triggerError($record, "enddate", "error_date");
          }
        }
      }
    }

    function endtime_validate(&$record, $mode)
    {
      // check if start and enddate are the same else don't validate
      $startdate = $record["startdate"]["year"].$record["startdate"]["month"].$record["startdate"]["day"];
      $enddate = $record["enddate"]["year"].$record["enddate"]["month"].$record["enddate"]["day"];

      if($startdate==$enddate && $record["allday"]==0)
      {
        if ($record["endtime"]["hours"] < $record["starttime"]["hours"])
        {
          triggerError($record, "endtime", "error_time");
        }
        elseif ($record["endtime"]["hours"] == $record["starttime"]["hours"])
        {
          if ($record["endtime"]["minutes"] <= $record["starttime"]["minutes"])
          {
            triggerError($record, "endtime", "error_time");
          }
        }
      }
    }


    /**
     * Create customer admin handler, this will call the scheduler controller
     * to show the diffrent views
     *
     * @param object $handler
     */
    function action_admin(&$handler)
    {
      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $theme = &atkTheme::getInstance();
        $page = &$this->getPage();
        $page->register_style($theme->stylePath("style.css"));
        $page->register_style(moduledir("scheduler")."styles/scheduler.css");
        $page->head("Schedule");
        $page->body();

        $controller = &atkinstance("module.scheduler.scheduler_controller");
        $request = array("view"=>$this->m_view,
                         "viewdate"=>$this->m_viewdate,
                         "small_viewdate"=>$this->m_small_viewdate,
                         "viewusers"=>$this->m_viewUsers);

        $result = $controller->handleRequest($request);

        // Add the refresh code if autorefresh is set and the refresh_interval is at least 1.
        $userprefs = $this->getUserSchedulerPrefs(atkArrayNvl(getUser(), "id"));
        if (($userprefs["autorefresh"] == 1) && ($userprefs["refresh_interval"] > 0))
        {
          $page->register_loadscript("setTimeout(\"document.location.reload()\",".($userprefs["refresh_interval"]*1000).")");
        }

        $box = $ui->renderBox(array("title"=>$this->text($this->m_type)." - ".$this->text($this->m_view),
                                               "content"=>$result));
        $actionpage = $this->renderActionPage("admin", array($box));
        $page->addContent($actionpage);
      }
      else
      {
         atkerror("ui object failure");
      }

    }

    /**
     * Custom view page, when recur is once, remove cyclus tab
     *
     * @param object $handler
     * @param array $record
     * @param object $node
     * @return string view page
     */
    function viewPage(&$handler,$record,$node)
    {
      if($record["recur"]=="once") $this->remove("cyclus");
      return $handler->viewPage($record,$node);
    }

    /**
     * Register some javascript that are used by the scheduler module
     */
    function registerFormScripts()
    {
      $page = &$this->getPage();
      $page->register_script(atkConfig("atkroot")."atk/javascript/dhtml_formtools.js");
      $page->register_script(moduleDir("scheduler")."javascript/scheduler.js");
      $page->register_loadscript("change_recur(); change_duration();");
    }

    /**
     * Customer edit action to register the javascripts
     *
     * @param object $handler
     * @return string Edit apge
     */
    function action_edit(&$handler)
    {
      $this->registerFormScripts();
      return $handler->action_edit();
    }

    /**
     * Customer add action to register the javascripts
     *
     * @param object $handler
     * @return string add page
     */
    function action_add(&$handler)
    {
      $this->registerFormScripts();
      return $handler->action_add();
    }

    /**
     * Customer delete action to handle the choice if we
     * should delete the complete item (if recurring) of just a single
     * item
     *
     * @param object $handler
     */
    function action_delete(&$handler)
    {
      if (!empty($this->m_postvars['confirmsingle'])) $this->_doDeleteSingle();
      else $handler->action_delete();
    }

    /**
     * Custom delete view which will appear when a user deletes a
     * recurring item. It will ask if it should delete a single item or
     * the complete cyclus
     *
     * @param string $atkselector
     * @param boolean $locked
     * @return string Confirm delete view
     */
    function confirmDelete($atkselector, $locked)
    {
      $ui = &$this->getUi();

      $this->addStyle("style.css");

      if (is_array($atkselector))
        $atkselector_str = '(('. implode($atkselector, ') OR (').'))';
      else $atkselector_str = $atkselector;

      $formstart ='<form action="'.atkSelf().'?"'.SID.' method="post">';
      $formstart.=session_form();
      $formstart.='<input type="hidden" name="atkaction" value="delete">';
      $formstart.='<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
      $formstart.='<input type="hidden" name="atkselector" value="'.$atkselector_str.'">';
      $formstart.='<input type="hidden" name="itemdate" value="'.$this->m_postvars["itemdate"].'">';
      $formstart.='<input type="hidden" name="scheduler_id" value="'.$this->m_postvars["scheduler_id"].'">';

      $buttons = array();

      $content = "";
      $recs = $this->selectDb($atkselector_str, "", "", "", array_merge($this->descriptorFields(),array("recur")));
      $record = $recs[0];

      if($record["recur"]!="once")
      {
        $buttons[] = '<input name="confirmsingle" class="btn" type="submit" value="'.$this->text("delete_single").'">';
        $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.$this->text("delete_cyclus").'">';
      }
      else
      {
        $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.atktext('yes').'">';

      }
      $buttons[] = '<input name="cancel" type="submit" class="btn_cancel" value="'.atktext('no').'">';

      $content.= '<br>'.$this->confirmActionText($atkselector, "delete", false);
      $action="delete";

      $output=$ui->renderAction($action, array("content"=>$content,
                                                "formstart"=>$formstart,
                                                "formend"=>'</form>',
                                                "buttons"=>$buttons));
      return $ui->renderBox(array("title"=>$this->actionTitle($action, $record),
                                  "content"=>$output));
    }

    /**
     * Delete a single item
     *
     */
    function _doDeleteSingle()
    {
      $db = &$this->getDb();
      $query = &$db->createQuery();
      $query->addTable('scheduler_cyclus_not');
      $query->addField('scheduler_id',$this->m_postvars["scheduler_id"],'scheduler_cyclus_not');
      $query->addField('date',$this->m_postvars["itemdate"],'scheduler_cyclus_not');
      $sql = $query->buildInsert();
      $db->query($sql);
      $sql = "DELETE FROM scheduler_dates WHERE scheduler_id='".$this->m_postvars["scheduler_id"]."' AND startdate='".$this->m_postvars["itemdate"]."'";
      $db->query($sql);
      $location = $this->feedbackUrl("delete", ACTION_SUCCESS);
      $this->redirect($location);
    }

    /**
     * Custom edit action for recurring events
     * It will first ask if the use want's to edit a single
     * item or the complete serie, after that it will execute
     * the correct action.
     *
     * @return unknown
     */
    function action_editserieorsingle()
    {
      $itemDate="";
      $sessionmanager = &atkGetSessionManager();
      $confirmsingle = $sessionmanager->stackVar("confirmsingle");
      if(atkArrayNvl($this->m_postvars, "confirmedit")!="" && atkArrayNvl($this->m_postvars, "confirmsingle")!="")
      {
         $this->action_editcopy();
      }
      elseif($this->m_postvars["confirmedit"]!="" && $this->m_postvars["confirm"]!="")
      {
        // Use redirect to get the correct atklevel
        $location = session_url(dispatch_url($this->atknodetype(), "edit", array("atkselector" => $this->m_postvars["atkselector"],"itemdate"=>$itemDate,"scheduler_id"=>$this->m_scheduler_id)), SESSION_REPLACE);
        $this->redirect($location);
      }
      elseif ($sessionmanager->PageVar("confirmedit")=="")
      {
        $this->confirmEdit($this->m_postvars["atkselector"]);
      }
      else
      {
        $this->redirect();
      }
    }

    /**
     * Precopy when An user edits a single item of a recurring item.
     * It will replace the start/end date with the selected date. Also
     * the recur will be set to once. (we don't copy the cyclus record)
     *
     * @param array $rec The current record.
     */
    function preCopy(&$rec)
    {
      if ($this->m_postvars['call_original_editcopy'] == true)
      {
        return;
      }

      $starthour = ($rec["allday"]==0?$rec["starttime"]["hours"]:0);
      $startminute = ($rec["allday"]==0?$rec["starttime"]["minutes"]:0);
      $endhour = ($rec["allday"]==0?$rec["endtime"]["hours"]:0);
      $endminute = ($rec["allday"]==0?$rec["endtime"]["minutes"]:59);

      $starttime = mktime($starthour,$startminute,0,$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]);
      $endtime = mktime($endhour,$endminute,0,$rec["enddate"]["month"],$rec["enddate"]["day"],$rec["enddate"]["year"]);
      $duration = $endtime-$starttime;
      $rec["startdate"]["day"]=date("d",$this->m_itemDate);
      $rec["startdate"]["month"]=date("m",$this->m_itemDate);
      $rec["startdate"]["year"]=date("Y",$this->m_itemDate);
      $rec["enddate"]["day"]=date("d",($this->m_itemDate+$duration));
      $rec["enddate"]["month"]=date("m",($this->m_itemDate+$duration));
      $rec["enddate"]["year"]=date("Y",($this->m_itemDate+$duration));
      $rec["recur"]="once";
    }

    /**
     * Edit copy action for when a user edits a single item of a recurring item
     *
     */
    function action_editcopy(&$handler)
    {
      if ($this->m_postvars['call_original_editcopy'] == true)
      {
        return $handler->action_editcopy();
      }

      $sessionmanager = &atkGetSessionManager();
      $confirmsingle = $sessionmanager->stackVar("confirmsingle");
      $itemDate = $sessionmanager->stackVar("itemdate");
      $scheduler_id = $sessionmanager->stackVar("scheduler_id");

      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","",array("alarms","cyclus","notes"),"","copy");

      if(count($recordset) > 0)
      {
        $db = &$this->getDb();
        if(!$this->copyDb($recordset[0]))
        {
          $db->rollback();
          $location = $this->feedbackUrl("editcopy", ACTION_FAILED, $recordset[0], $db->getErrorMsg());
          $this->redirect($location);
        }
        else
        {
          $db->commit();
          // Delete the recurring date and add an exception
          $name = "atk".atkconfig("database")."query";
          $query = atknew($name);
          $query->addTable('scheduler_cyclus_not');
          $query->addField('scheduler_id',$scheduler_id,'scheduler_cyclus_not');
          $query->addField('date',$itemDate,'scheduler_cyclus_not');
          $sql = $query->buildInsert();
          $db->query($sql);
          $sql = "DELETE FROM scheduler_dates WHERE scheduler_id='".$scheduler_id."' AND startdate='".$itemDate."'";
          $db->query($sql);

          //$this->clearCache();
          $location = session_url(dispatch_url($this->atknodetype(), "edit", array("atkselector" => $this->primaryKey($recordset[0]),"itemdate"=>$itemDate,"scheduler_id"=>$scheduler_id,"confirmsingle"=>$confirmsingle)), SESSION_REPLACE);
          $this->redirect($location);
        }
      }

      else
      {
        $this->redirect();
      }
    }

    /**
     * Custom update action
     *
     * @param object $handler
     * @return Update action
     */
    function action_update(&$handler)
    {
      $sessionmanager = &atkGetSessionManager();
      $confirmsingle = $sessionmanager->stackVar("confirmsingle");
      $itemDate = $sessionmanager->stackVar("itemdate");
      $scheduler_id = $sessionmanager->stackVar("scheduler_id");
      if ($handler->m_postvars['atkcancel'] &&  $confirmsingle!="")
      {
         $record = $this->updateRecord();
         $this->deleteDb($this->primaryKey($record));
         $sql = "DELETE FROM scheduler_cyclus_not WHERE scheduler_id='$scheduler_id' AND date='$itemDate'";
         $db = &atkGetDb();
         $db->query($sql);
         $this->removeSchedulerDates($scheduler_id);
         $sql = "UPDATE scheduler_scheduler SET lastdate=null,times=null WHERE id='$scheduler_id'";
         $db->query($sql);
      }
      return $handler->action_update();
    }

   /**
    * Get forum buttons, we use this function to override the buttons of the view page
    *
    * @param string $mode Mode
    * @param array $record Scheduler item record
    * @return array Array with buttons
    */
   function getFormButtons($mode, $record)
   {
     if ($mode==='view')
     {
       $controller = atkController::getInstance();
       atkimport("modules.scheduler.utils.scheduleritem");
       $schedulerItem = schedulerItem::getInstance();
       $buttons[] = $controller->getButton('back');
       if($schedulerItem->isRecurring($record))
       {

         if($this->allowed("edit", $record))
         {
           $buttons[] = '<input type="hidden" name="atkaction" value="editserieorsingle">
                         <input type="hidden" name="confirmedit" value="true">
                         <input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">
                         <input type="hidden" name="atkselector" value="'.$this->m_postvars["atkselector"].'">
                         <input type="hidden" name="itemdate" value="'.$this->m_postvars["itemdate"].'">
                         <input type="hidden" name="scheduler_id" value="'.$this->m_postvars["scheduler_id"].'">';

           $buttons[] = '&nbsp;<input name="confirm" type="submit" class="btn_save" value="'.$this->text("edit_cyclus").'">&nbsp;';

           $buttons[] = '&nbsp;<input name="confirmsingle" type="submit" class="btn_save" value="'.$this->text("edit_single").'">&nbsp;';
         }
       }
       else
       {
          $buttons[] = '<input type="hidden" name="atkaction" value="edit">'.
                      '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">'.
                      '&nbsp;'.$controller->getButton('edit').'&nbsp;';
       }
     }
     else
     {
       // Get the normal buttons
       $buttons = parent::getFormButtons($mode,$record);
     }
     return $buttons;
  }



    /**
     * Add owner as attendee
     *
     * @param unknown_type $rec
     * @return unknown
     */
    function postAdd($rec)
    {
      // insert the owner as first attendee.
      if($rec["owner_attendee"]==1)
      {
        $this->_addAttendeeRecord($rec["id"],$rec["owner"]["id"],"employee","accepted",false);
      }
      $this->updateSchedulerDates($rec);
      return true;
    }

    function postUpdate($rec)
    {
      $this->updateSchedulerDates($rec);
      return true;
    }

    function postDelete($rec)
    {
      $this->removeSchedulerDates($rec['id']);
      return true;
    }

    function preDelete($rec)
    {
      $this->sendEmail($rec);
      $this->deleteAttendees($rec);
      return true;
    }

    function preAdd(&$record)
    {
      $this->updateEndDate($record);
      return true;
    }

    function preUpdate(&$record)
    {
      $this->removeSchedulerDates($record['id']);
      $this->updateEndDate($record);
      return true;
    }

    /**
     * Calculate the endddate if the user used the duration dropdown
     *
     * @param unknown_type $rec
     */
    function updateEndDate(&$rec)
    {
      if($rec['duration']!=-1)
      {
        $enddate = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],$rec['startdate']['month'],$rec['startdate']['day'],$rec['startdate']['year']);
        $enddate+=$rec['duration']*60;
        $rec['endtime']['hours']=date("H",$enddate);
        $rec['endtime']['minutes']=date("i",$enddate);
        $rec['endtime']['seconds']=date("s",$enddate);
        $rec['enddate']['day']=date("d",$enddate);
        $rec['enddate']['month']=date("m",$enddate);
        $rec['enddate']['year']=date("Y",$enddate);
      }
      $rec['lastdate'] = mktime($rec["endtime"]["hours"],$rec["endtime"]["minutes"],$rec["endtime"]["seconds"],$rec['enddate']['month'],$rec['enddate']['day'],$rec['enddate']['year']);
    }

    /**
     * Send email to all attendees except the owner
     * that the meeting is canceld
     *
     * @todo Fix mail
     */
    function sendEmail($rec)
    {
      return true;
      $date="";
      foreach($rec["attendees"] as $attendee)
      {
        // Don't send mail to the owner
        if($attendee["person"]["id"]!=$rec["owner"]["id"] && $attendee["person"]["role"]=="employee")
        {
          $template = &atknew("modules.scheduler.scheduler_email_template");
          $template->initialize("cancel_event",$attendee["person_id"]["id"]);
          $template->loadData("cancel_event",$rec["id"],$date,"canceled");
          $template->send();
        }
      }
    }
    /**
     * Delete all attendees
     *
     * @param array $rec Array with the record of the scheduler item
     */
    function deleteAttendees($rec)
    {
      $attendee_node = &atkGetNode("scheduler.scheduler_attendees");
      foreach($rec["attendees"] as $attendee)
      {
        $attendee_node->deleteDb("scheduler_attendees.scheduler_id='".$rec["id"]."' AND person_id='".$attendee["person_id"]["id"]."'",false);
      }

    }

    /**
     * Store employees
     *
     * @param array $rec Scheduler item record
     * @param array $attendees Array with attendees
     * @return boolean
     */
    function storeEmployees($rec, $attendees)
    {
      return $this->storeAttendees($rec,$attendees,"employee");
    }

    /**
     * Store contacts
     *
     * @param array $rec Scheduler item record
     * @param array $attendees Array with attendees
     * @return boolean
     */
    function storeContacts($rec, $attendees)
    {
      return $this->storeAttendees($rec,$attendees,"contact");
    }

    /**
     * Store attendees
     *
     * @param array $rec Array with the item record
     * @param array $attendees Array with the attendees
     * @param string $role Role of the person
     * @return boolean
     */
    function storeAttendees($rec,$attendees,$role)
    {
      for ($i=0, $_i=count($attendees); $i<$_i; $i++)
      {
        if($attendees[$i]["id"]==$rec["owner"]["id"])
        {
          $execTrigger=false;
          $status="accepted";
        }
        else
        {
          $execTrigger=true;
          $status="no_response";
        }
        $this->_addAttendeeRecord($rec["id"], $attendees[$i]["id"],$role,$status,$execTrigger);
      }
      return true;
    }

    /**
     * Add attendee record
     *
     * @access private
     * @param int $scheduler_id Scheduler id
     * @param int $person_id Person id
     * @param string $role Role of the person
     * @param string $status Status of the schedule item from this person
     * @param boolean $execTrigger Execute trigger or not for addDb
     */
    function _addAttendeeRecord($scheduler_id, $person_id,$role,$status="no_response",$execTrigger=true)
    {

      $attendee_node = &atkGetNode("scheduler.scheduler_attendees");

      $total = $attendee_node->countDb("scheduler_attendees.scheduler_id = ".$scheduler_id." AND person_id = ".$person_id);
      if($total==0)
      {
        $rec = array("scheduler_id"=>$scheduler_id,
                     "person_id"=>array("id"=>$person_id,
                                         "role"=>$role),
                     "status"=>$status);
        $attendee_node->addDb($rec,$execTrigger);
      }
    }

    /**
     * Get User Scheduler preferences
     *
     * @param unknown_type $userid
     * @return array Array with the user preferences
     */
    function getUserSchedulerPrefs($userid)
    {
      static $userprefs_cache = null;

      if(!is_null($userprefs_cache))
      {
        $userprefs = $userprefs_cache;
      }
      else
      {
        $userprefsnode = &atkNew("module.scheduler.scheduler_userpreferences");
        $userprefs = $userprefsnode->selectDb("userid='".$userid."'");
        $userprefs_cache = $userprefs;
      }
      return atkArrayNvl($userprefs, 0);
    }


    /**
     * Update scheduler dates for a record
     *
     * @param array $rec Scheduler item record
     */
    function updateSchedulerDates($rec)
    {
      $id = $rec["id"];
      atkimport("module.scheduler.utils.schedulertools");
      $startdate = date("Y-m-d",mktime(12,0,0,$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]));
      $enddate = date("Y-m-d",mktime(12,0,0,(substr($this->m_viewdate,5,2)+2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4)));
      schedulertools::updateRecurringEvents($startdate,$enddate,$rec);
    }

    /**
     * Remove calculated scheduler dates
     *
     * @param integer $id Scheduler item id
     */
    function removeSchedulerDates($id)
    {
      $s_dates = &atkGetNode("scheduler.scheduler_dates");
      $s_dates->deleteDb("scheduler_id = '$id'");
      $db = $this->getDb();
      $db->query("UPDATE scheduler_scheduler SET lastdate = NULL, times = 0 WHERE id='$id'");
      $db->commit();
    }
  }

?>
