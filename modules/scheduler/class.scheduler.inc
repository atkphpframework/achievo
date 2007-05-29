<?php
  /**
   * Module scheduler
   * Class schedule
   *
   * This class makes it possible to schedule a appointment.
   *
   * $Id$
   * @author L. Willems  <lineke@ibuildings.nl>
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

  class scheduler extends atkNode
  {
    var $m_viewdate;
    var $m_small_viewdate;
    var $m_viewUser;
    var $m_view;
    var $m_employees;
    var $m_itemDate;
    var $m_scheduler_id;
    var $m_confirmsingle;

    // Default minute step
    var $m_minute_step = 5;


    function scheduler()
    {
      $sessionmanager = &atkGetSessionManager();
      $this->m_view = $sessionmanager->pageVar("view");
      $this->m_viewUser = $sessionmanager->pageVar("user");
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

      $this->atkNode("scheduler", NF_ADD_LINK|NF_TRACK_CHANGES);

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("title", AF_SEARCHABLE|AF_OBLIGATORY, 50));
      $this->add(new atkOneToManyRelation("attendees", "scheduler.scheduler_attendees", "scheduler_id", AF_HIDE_LIST));
      $this->add(new atkDummyAttribute("hint", $this->text("attendee_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW));
      $this->add(new atkfuzzySearchAttribute("attendee_empsearch", "employee.employee", "storeEmployees", "multiselect"));
      $this->add(new atkfuzzySearchAttribute("attendee_contactsearch", "organization.contact", "storeContacts", "multiselect"));
      $this->add(new atkAttribute("location", AF_SEARCHABLE, 50));
      $this->add(new atkTextAttribute("description"));
      $this->add($recur_attr = new atkRadioAttribute("recur",array("once","daily","weekly","monthly_date","monthly_day","yearly_date","yearly_day"),array()), "cyclus");
      $this->add(new atkDateAttribute("startdate", "F d Y","d F Y", 0, 0));
      $this->add(new atkDateAttribute("enddate", "F d Y","d F Y", 0, 0));
      $this->add($allday_attr = new atkBoolAttribute("allday"));
      $this->add(new atkTimeAttribute("starttime", 0, 23, $m_arr_minute_step, $default_start_time, 0));
      $this->add(new atkTimeAttribute("endtime", 0, 23, $m_arr_minute_step, $default_end_time, 0));
      $this->add(new atkOneToOneRelation("cyclus","scheduler.scheduler_cyclus","scheduler_id",AF_ONETOONE_INTEGRATE|AF_CASCADE_DELETE),"cyclus");
      $this->add(new atkManyToOneRelation("category", "scheduler.scheduler_category",AF_OBLIGATORY|AF_RELATION_AUTOLINK));
      $this->add(new atkBoolAttribute("private"));
      $this->add(new atkBoolAttribute("all_users"));
      $this->add(new atkListAttribute("priority",array("","low","normal","high"),array(0,1,2,3),AF_LIST_NO_NULL_ITEM));
      $this->add(new atkBoolAttribute("nonblocking"));
      //$this->add(new atkOneToManyRelation("alarms", "scheduler.scheduler_alarms", "scheduler_id", AF_HIDE_LIST|AF_HIDE_ADD|AF_CASCADE_DELETE), "alarms"); // Disabled, not working yet
      $this->add(new atkOneToManyRelation("notes", "scheduler.scheduler_notes", "scheduler_id", AF_HIDE_LIST|AF_HIDE_ADD|AF_CASCADE_DELETE), "notes");
      $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_READONLY|AF_OBLIGATORY));
      $this->add(new atkBoolAttribute("owner_attendee"));
      $this->add(new atkNumberAttribute("lastdate", AF_HIDE));

      $recur_attr->addOnChangeHandler("change_recur(newvalue);");
      $allday_attr->addOnChangeHandler("change_allday();");

      if($this->m_viewUser=="" && $this->m_view!="employeeview")
      {
        if ($userprefs["default_view"]!="*" && !empty($userid))
        {
          $this->m_viewUser = $userid;
        }
        else
        {
         // User has opted to show all users by default.
          $this->m_viewUser = "all";
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

    // Display functions
    function starttime_display($rec)
    {
      return $rec["starttime"]["hours"].":".$rec["starttime"]["minutes"];
    }

    function endtime_display($rec)
    {
      return $rec["endtime"]["hours"].":".$rec["endtime"]["minutes"];
    }

    function notes_display($record,$mode)
    {
      $attr = &$this->getAttribute("notes");
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


    function initial_values()
    {
      $user = getUser();
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
        $initial["cyclus_startdate"]=$arr_date;
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
        if($isAttendee) atkdebug("CURRENT USER IS AN ATTENDEE ($action)");

        if((!$isPrivate && in_array($owner,$assistantFor)) || $owner==$userid || ($action=="view" && $isAttendee))
        {
          // Item is not private and user is the owner or an assistant for the owner
          // or action is view and user is an attendee
          atkdebug("USER IS ALLOWED");
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

    function descriptor_def()
    {
      return "[title]";
    }


    // Validate functions
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

    function every_edit($record="", $fieldprefix="",$mode)
    {
      $attrib = $this->getAttribute("every");
      $ret = $attrib->edit($record,$fieldprefix,$mode);
      $ret.= '&nbsp;<span id="every_day">day(s)</span><span id="every_week">week(s)</span><span id="every_month">month(s)</span><span id="every_year">year(s)</span>';

      return $ret;
    }

    // Page functions
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

        if (method_exists($this, $this->m_view."view"))
        {
          $funcname = $this->m_view."view";
          $result = $this->$funcname();
        }
        else
        {
          $result = "Niet bestaande view (".$this->m_view." view)";
        }

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

    function viewPage(&$handler,$record,$node)
    {
      if($record["recur"]=="once") $this->remove("cyclus");
      return $handler->viewPage($record,$node);
    }

    function registerFormScripts()
    {
      $page = &$this->getPage();
      $page->register_script(atkConfig("atkroot")."atk/javascript/dhtml_formtools.js");
      $page->register_script(moduleDir("scheduler")."javascript/scheduler.js");
      $page->register_loadscript("change_recur(); change_allday();");
    }

    function action_edit(&$handler)
    {
      $this->registerFormScripts();
      return $handler->action_edit();
    }

    function action_add(&$handler)
    {
      $this->registerFormScripts();
      return $handler->action_add();
    }

    function action_delete(&$handler)
    {
      if (!empty($this->m_postvars['confirmsingle'])) $this->_doDeleteSingle();
      else $handler->action_delete();
    }

    function confirmDelete($atkselector, $locked)
    {
      $ui = &$this->getUi();

      $this->addStyle("style.css");

      if (is_array($atkselector))
        $atkselector_str = '(('. implode($atkselector, ') OR (').'))';
      else $atkselector_str = $atkselector;

      $formstart ='<form action="'.$_SERVER["PHP_SELF"].'?"'.SID.' method="post">';
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
        $buttons[] = '<input name="confirmsingle" class="btn" type="submit" value="'.atktext('delete_single').'">';
        $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.atktext("delete_cyclus").'">';
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

    function confirmEdit($atkselector)
    {
      $ui = &$this->getUi();

      $this->addStyle("style.css");

      $formstart = '<form action="'.$_SERVER["PHP_SELF"].'?"'.SID.' method="post">';
      $formstart.= session_form();
      $formstart.= '<input type="hidden" name="atkaction" value="editserieorsingle">';
      $formstart.= '<input type="hidden" name="confirmedit" value="true">';
      $formstart.= '<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
      $formstart.= '<input type="hidden" name="atkselector" value="'.$atkselector.'">';
      $formstart.= '<input type="hidden" name="itemdate" value="'.$this->m_postvars["itemdate"].'">';
      $formstart.= '<input type="hidden" name="scheduler_id" value="'.$this->m_postvars["scheduler_id"].'">';

      $buttons = array();
      $buttons[] = '<input name="confirmsingle" class="btn" type="submit" value="'.atktext('edit_single').'">';
      $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.atktext("edit_cyclus").'">';
      $buttons[] = '<input name="cancel" type="submit" class="btn_cancel" value="'.atktext('no').'">';

      $content.= '<br>'.$this->confirmActionText($atkselector, "edit", false);
      $action="edit";

      $output=$ui->renderAction($action, array("content"=>$content,
                                                "formstart"=>$formstart,
                                                "formend"=>'</form>',
                                                "buttons"=>$buttons));
      $record=array();
      $content = $ui->renderBox(array("title"=>$this->actionTitle($action, $record),
                                  "content"=>$output));
      $theme = &atkTheme::getInstance();
      $page = &$this->getPage();
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style(moduledir("scheduler")."styles/scheduler.css");
      $page->head("Schedule");
      $page->body();
      $page->addContent($content);
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
      if($this->m_postvars["confirmedit"]!="" && $this->m_postvars["confirmsingle"]!="")
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
      $starthour = ($rec["allday"]==0?$rec["starttime"]["hours"]:0);
      $startminute = ($rec["allday"]==0?$rec["starttime"]["minutes"]:0);
      $endhour = ($rec["allday"]==0?$rec["endtime"]["hours"]:0);
      $endminute = ($rec["allday"]==0?$rec["endtime"]["minutes"]:59);

      $starttime = mktime($starthour,$startminute,0,$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]);
      $endtime = mktime($endhour,$endminute,0,$rec["enddate"]["month"],$rec["enddate"]["day"],$rec["enddate"]["year"]);
      $duration = $endtime-$starttime;
      atkdebug("<h1>Item DATE: ".date("Y-m-d H:i",$this->m_itemDate)."</h1>");
      $rec["startdate"]["day"]=date("d",$this->m_itemDate);
      $rec["startdate"]["month"]=date("m",$this->m_itemDate);
      $rec["startdate"]["year"]=date("Y",$this->m_itemDate);
      $rec["enddate"]["day"]=date("d",($this->m_itemDate+$duration));
      $rec["enddate"]["month"]=date("m",($this->m_itemDate+$duration));
      $rec["enddate"]["year"]=date("Y",($this->m_itemDate+$duration));
      $rec["recur"]="once";
    }

    function action_editcopy()
    {

      atkdebug("scheduler::action_editcopy()");
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
        atkdebug("No records found with selector: ".$this->m_postvars['atkselector']);
        $this->redirect();
      }
    }

    function action_update(&$handler)
    {
      $sessionmanager = &atkGetSessionManager();
      $confirmsingle = $sessionmanager->stackVar("confirmsingle");
      $itemDate = $sessionmanager->stackVar("itemdate");
      $scheduler_id = $sessionmanager->stackVar("scheduler_id");
      atkdebug("<h1>UPDATE ".$confirmsingle."</h1>");
      if ($handler->m_postvars['atkcancel'] &&  $confirmsingle!="")
      {
         $record = $this->updateRecord();
         $this->deleteDb($this->primaryKey($record));
         $sql = "DELETE FROM scheduler_cuclus_not WHERE scheduler_id='$scheduler_id' AND date='$itemDate'";
         $db = &atkGetDb();
         $db->query($sql);
      }
      return $handler->action_update();
    }




    function dayview()
    {
      $dayview = &atknew("modules.scheduler.views.scheduler_dayview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);
      $dayview->setViewDate($day,$month,$year);
      $day   = substr($this->m_small_viewdate, 8,2);
      $month = substr($this->m_small_viewdate, 5,2);
      $year  = substr($this->m_small_viewdate, 0,4);
      $dayview->setSmallViewDate($day,$month,$year);
      $dayview->setViewUser($this->m_viewUser);
      return $dayview->renderView();
    }

    function weekview()
    {
      $weekview = &atknew("modules.scheduler.views.scheduler_weekview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);
      $weekview->setViewDate($day,$month,$year);
      $day   = substr($this->m_small_viewdate, 8,2);
      $month = substr($this->m_small_viewdate, 5,2);
      $year  = substr($this->m_small_viewdate, 0,4);
      $weekview->setSmallViewDate($day,$month,$year);
      $weekview->setViewUser($this->m_viewUser);
      return $weekview->renderView();

    }

    function monthview()
    {
      // viewdate is something like 2001-07-23
      $monthview = &atknew("modules.scheduler.views.scheduler_monthview");

      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);

      $monthview->setViewDate($day,$month,$year);
      $day   = substr($this->m_small_viewdate, 8,2);
      $month = substr($this->m_small_viewdate, 5,2);
      $year  = substr($this->m_small_viewdate, 0,4);
      $monthview->setSmallViewDate($day,$month,$year);

      $monthview->setViewUser($this->m_viewUser);
      return $monthview->renderView();
    }

    function yearview()
    {
      // viewdate is something like 2001-07-23
      $yearview = &atknew("modules.scheduler.views.scheduler_yearview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);
      $yearview->setViewDate($day,$month,$year);
      $yearview->setViewUser($this->m_viewUser);
      return $yearview->renderView();
    }

    function employeeview()
    {
      $employeeview = &atknew("modules.scheduler.views.scheduler_employeeview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);

      $employeeview->setViewDate($day,$month,$year);
      $employeeview->setEmployees($this->m_employees);
      return $employeeview->renderView();
    }

    function birthdayview()
    {
      $birthdayview = &atknew("modules.scheduler.views.scheduler_birthdayview");
      $day   = substr($this->m_viewdate, 8,2);
      $month = substr($this->m_viewdate, 5,2);
      $year  = substr($this->m_viewdate, 0,4);

      $birthdayview->setViewDate($day,$month,$year);
      $birthdayview->setViewUser($this->m_viewUser);
      return $birthdayview->renderView();
    }


    // Record functions
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
      $this->removeSchedulerDates($rec);
      return true;
    }
    
    function preDelete($rec)
    {
      $this->sendEmail($rec);
      $this->deleteAttendees($rec);
      return true;
    }
    
    /**
     * Send email to all attendees except the owner
     * that the meeting is canceld
     *
     */
    function sendEmail($rec)
    {
      atk_var_dump($rec["attendees"],"ATTENDEES DELETE");
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
        $attendee_node->deleteDb("scheduler_id='".$rec["id"]."' AND person_id='".$attendee["person_id"]["id"]."'",false);
      }
      
    }

    function storeEmployees($rec, $attendees)
    {
      return $this->storeAttendees($rec,$attendees,"employee");
    }
    function storeContacts($rec, $attendees)
    {
      return $this->storeAttendees($rec,$attendees,"contact");
    }
    
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

    function _addAttendeeRecord($scheduler_id, $person_id,$role,$status="no_response",$execTrigger=true)
    {
      
      $attendee_node = &atkGetNode("scheduler.scheduler_attendees");
      
      $total = $attendee_node->countDb("scheduler_id = ".$scheduler_id." AND person_id = ".$person_id);
      if($total==0)
      {
        $rec = array("scheduler_id"=>array("id"=>$scheduler_id),
                     "person_id"=>array("id"=>$person_id,
                                         "role"=>$role),
                     "status"=>$status);
        $attendee_node->addDb($rec,$execTrigger);
      }
    }

    function preAdd(&$rec)
    {
      $this->UpdateScheduleRec($rec);
      return true;
    }

    function preUpdate(&$rec)
    {
      $this->UpdateScheduleRec($rec);
      return true;
    }

    function UpdateScheduleRec(&$rec)
    {
      if($rec["recur"]=="once")
      {
        $rec["cyclus"]["cyclus_startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["cyclus_enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["startday"]=1;
        $rec["cyclus"]["endday"]=1;
        $rec["cyclus"]["startmonth"]=1;
        $rec["cyclus"]["endmonth"]=1;
        $rec["cyclus"]["every"]=1;
        $rec["cyclus"]["month_time"]=1;
        $rec["cyclus"]["weekday"]=0;
      }
      elseif($rec["recur"]=="daily")
      {
        $rec["cyclus"]["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["startday"]=1;
        $rec["cyclus"]["endday"]=1;
        $rec["cyclus"]["startmonth"]=1;
        $rec["cyclus"]["endmonth"]=1;
        $rec["cyclus"]["month_time"]=1;
        $rec["cyclus"]["weekday"]=0;
      }
      elseif($rec["recur"]=="weekly")
      {
        $rec["cyclus"]["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["startday"]=1;
        $rec["cyclus"]["endday"]=1;
        $rec["cyclus"]["startmonth"]=1;
        $rec["cyclus"]["endmonth"]=1;
        $rec["cyclus"]["month_time"]=1;
      }
      elseif($rec["recur"]=="monthly_date")
      {
        $rec["cyclus"]["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["startmonth"]=1;
        $rec["cyclus"]["endmonth"]=1;
        $rec["cyclus"]["month_time"]=1;
        $rec["cyclus"]["weekday"]=0;
      }
      elseif($rec["recur"]=="monthly_day")
      {
        $rec["cyclus"]["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["startday"]=1;
        $rec["cyclus"]["endday"]=1;
        $rec["cyclus"]["startmonth"]=1;
        $rec["cyclus"]["endmonth"]=1;
      }
      elseif($rec["recur"]=="yearly_date")
      {
        $rec["cyclus"]["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["month_time"]=1;
        $rec["cyclus"]["weekday"]=0;
      }
      elseif($rec["recur"]=="yearly_day")
      {
        $rec["cyclus"]["startdate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["enddate"]=array("year"=>"0000","month"=>"0","day"=>"0");
        $rec["cyclus"]["startday"]=1;
        $rec["cyclus"]["endday"]=1;
        $rec["cyclus"]["endmonth"]=1;
      }
    }

    function getUserSchedulerPrefs($userid)
    {
      $userprefsnode = &atkNew("module.scheduler.scheduler_userpreferences");
      $userprefs = $userprefsnode->selectdb("userid='".$userid."'");
      return atkArrayNvl($userprefs, 0);
    }

    function updateSchedulerDates($rec)
    {
      $db = &atkGetDb();
      $id = $rec["id"];
      $this->removeSchedulerDates($rec);

      atkimport("module.scheduler.utils.schedulertools");
      $startdate = $this->m_viewdate;
      if($rec["recur"]=="once")
      {
        $startdate = date("Y-m-d",mktime(12,0,0,$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]));
      }
      else
      {
        $startdate = date("Y-m-d",mktime(12,0,0,$rec["cyclus"]["cyclus_startdate"]["month"],$rec["cyclus"]["cyclus_startdate"]["day"],$rec["cyclus"]["cyclus_startdate"]["year"]));
      }
      // Enddate is startdate + 2 months (because of the small calendars)
      $enddate = date("Y-m-d",mktime(12,0,0,(substr($this->m_viewdate,5,2)+2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4)));
      $dates = schedulertools::getDates($rec,$startdate,$enddate);

      foreach($dates as $date)
      {
        $tmp_start = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],$rec["startdate"]["month"],$rec["startdate"]["day"],$rec["startdate"]["year"]);
        $tmp_end = mktime($rec["endtime"]["hours"],$rec["endtime"]["minutes"],$rec["endtime"]["seconds"],$rec["enddate"]["month"],$rec["enddate"]["day"],$rec["enddate"]["year"]);
        $duration = $tmp_end-$tmp_start;
        $startdate = mktime($rec["starttime"]["hours"],$rec["starttime"]["minutes"],$rec["starttime"]["seconds"],substr($date,5,2),substr($date,8,2),substr($date,0,4));
        $enddate = $startdate+$duration;

        $sql = "INSERT INTO scheduler_dates (scheduler_id,startdate,enddate) VALUES ('$id','$startdate','$enddate')";
        $db->query($sql);
      }
      $sql = "UPDATE scheduler_scheduler SET lastdate = '$enddate' WHERE id = '$id'";
      $db->query($sql);
    }

    function removeSchedulerDates($rec)
    {
      $id = $rec["id"];
      $s_dates = &atkGetNode("scheduler.scheduler_dates");
      $s_dates->deleteDb("scheduler_id = '$id'");
    }
  }

?>
