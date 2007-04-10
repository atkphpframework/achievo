<?php
  userelation("atkmanytoonerelation");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("atkdummyattribute");
  useattrib("atknumberattribute");
  useattrib("atktextattribute");
  useattrib("atkcreatedbyattribute");
  useattrib("atkCreateStampAttribute");
  useattrib("atkupdatedbyattribute");
  useattrib("atkupdatestampattribute");

  class quotation extends atkNode
  {
    var $m_view;

    function quotation()
    {
      global $g_sessionManager;

      $this->atkNode("quotation",NF_ADD_LINK);
      $listener = &atknew("modules.crm.listeners.eventlistener");
      $this->addListener($listener);
      $this->m_view = $g_sessionManager->pageVar("view"); // Can be 'all' or 'filtered'.

      if ($this->m_view=="") $this->m_view = "filtered";

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("quotation_nr", AF_READONLY|AF_SEARCHABLE|AF_OBLIGATORY, 10));

      $organization_attribute = &$this->add(new atkManyToOneRelation("organization_id","organization.organization"  , AF_OBLIGATORY|AF_RELATION_AUTOLINK|AF_SEARCHABLE));

      if (atkConfig::get('quotation','contact_is_relation',false))
      {
        $organization_attribute->addDependee("contact");
        $contact_attribute = &$this->add(new atkManyToOneRelation("contact","organization.contact", AF_OBLIGATORY|AF_HIDE_LIST));
        $contact_attribute->addDestinationFilter("company = '[organization_id.id]'");
      }
      else $this->add(new atkAttribute("contact", AF_OBLIGATORY|AF_HIDE_LIST, 50));

      //Show warning message when method has no value.
      //Get source object (method attribute) and destination object (table row method_warning)
      $srcMethod 	= "document.getElementById('method')";
	    $destMethod = "document.getElementById('ar_method_warning')";

	    //Add dummy attribute with warning
      $this->add(new atkDummyAttribute("method_warning", atktext("method_warning_null","quotation","quotation"), AF_HIDE_LIST | AF_HIDE_VIEW));

      //Create list attribute of method
      $attr = new atkListAttribute("method", array("fax","mail","phone","email","other"),"",AF_HIDE_LIST);

      //add Listener
	    $attr->addOnChangeHandler("$destMethod.style.visibility=(($srcMethod.selectedIndex == 0) ? 'visible' : 'hidden');");

      //Add attribute to page
	    $this->add($attr);

      $this->add(new atkAttribute("title", AF_OBLIGATORY|AF_SEARCHABLE, 100));
      $this->add(new atkTextAttribute("description", AF_HIDE_LIST));
      $this->add(new atkDateAttribute("issue_date", "", "", 0, 0, AF_HIDE_LIST | AF_OBLIGATORY));
      $this->add(new atkDateAttribute("expire_date", "", ""));



      //Show warning message when status has no value.
      //Get source object (status attribute) and destination object (table row status_warning)
      $srcStatus 	= "document.getElementById('status')";
	    $destStatus = "document.getElementById('ar_status_warning')";
	    //Add dummy attribute with warning
	    $this->add(new atkDummyAttribute("status_warning", atktext("status_warning_null","quotation","quotation"), AF_HIDE_LIST | AF_HIDE_VIEW));

	    //Create list attribute of status
	    $states = array("new","sent","called","accepted","rejected","prepaid","prepaidcompleted", "completed","billed");

	    //translate status
	    $states_translated=array();
	    foreach($states as $state)
	     $states_translated[] = atktext($state,"quotation","quotation");


	    $attr = new atkListAttribute("status", $states_translated, $states, AF_SEARCHABLE);

	    //Add Listener
	    $attr->addOnChangeHandler("$destStatus.style.visibility=(($srcStatus.selectedIndex == 0) ? 'visible' : 'hidden');");

	    //Add attribute to page
      $this->add($attr);

      $this->add(new atkManyToOneRelation("assigned_to","employee.employee", AF_HIDE_LIST|AF_OBLIGATORY|AF_SEARCHABLE));
      $this->add(new atkNumberAttribute("price", AF_TOTAL | AF_OBLIGATORY, 13, 2));
      $this->add(new atkManyToOneRelation("payment_id","quotation.payment"));

      $percattrib = new atkNumberAttribute("success_chance",AF_HIDE_LIST,3,0);
      $percattrib->m_minvalue = 0;
      $percattrib->m_maxvalue = 100;
      $this->add($percattrib);

      $this->add(new atkNumberAttribute("profit_expectance",AF_HIDE_ADD|AF_HIDE_EDIT|AF_TOTAL,10,2));
      $this->add(new atkManyToOneRelation("campaign","crm.campaign",AF_HIDE_LIST));
      $this->add(new atkManyToOneRelation("source","crm.source",AF_HIDE_LIST));

      $this->add(new atkCreatedByAttribute("created_by",AF_HIDE_LIST),"history");
      $this->add(new atkCreateStampAttribute("created_on",AF_HIDE_LIST),"history");
      $this->add(new atkUpdatedByAttribute("last_modified_by",AF_HIDE_LIST),"history");
      $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST),"history");



      $this->m_securityMap["setstate"] = "edit";
      $this->m_securityMap["document"] = "view";

      for ($i=0, $_i=count($states); $i<$_i; $i++)
      {
        $this->m_securityMap["set_".$states[$i]] = "edit";
      }

      $this->setOrder("quotation_nr DESC");
      $this->setTable("quotation_quotation");
    }

    function descriptor_def()
    {
      return "[quotation_nr] - [title]";
    }


    /**
     * Assigns a bill record and any important relations to a documentwriter
     *
     * @param atkDocumentWrter $documentWriter Document writer to which the record should be assigned
     * @param string $selector SQL Where clause to be used when selecting the bill from the database
     */
    function assignDocumentVars(&$documentWriter, $selector)
    {
      // Assign the quotation to the document
      $quotations = $this->selectDb($selector);
      $documentWriter->assignDocumentSingleRecord($this,$quotations[0]);
      $documentWriter->assignRecordByNodeAndSelector("organization.organization", sprintf("organization.id='%d'", $quotations[0]["organization_id"]["id"]), "organization_");
      $documentWriter->assignRecordByNodeAndSelector("employee.employee", sprintf("person.id='%d'", $quotations[0]["owner"]["id"]), "owner_");
      $documentWriter->assignDocumentGenericVars();
    }

    function action_edit(&$handler)
    {
    	$this->registerLoadScripts();
      return $handler->action_edit();
    }

    function action_add(&$handler)
    {
    	$this->registerLoadScripts();
    	return $handler->action_add();
    }


    /**
     * Register scripts for initial visibility of warnings
     *
     * Function registers a script that runs on page load.
     * script checks if the method and/or status dropdownlists contain a legitimate value, if
     * not: then show the warning message for that dropdownlist
     */
    function registerLoadScripts()
    {
    	$page = &$this->getPage();
      $page->register_scriptcode("
        function warning(srcid, destid) {
          var srcObj = document.getElementById(srcid);
	    		var destObj = document.getElementById(destid);
	    		destObj.style.visibility = (srcObj.selectedIndex == 0) ? 'visible' : 'hidden';
        }
      ");

      $page->register_loadscript("warning('method', 'ar_method_warning'); warning('status', 'ar_status_warning');");
    }

    function action_admin(&$handler)
    {
      if ($this->m_view == "filtered")
      {
        $this->addFilter("quotation_quotation.status NOT IN ('rejected', 'prepaidcompleted', 'billed')");
      }
      return $handler->action_admin();
    }

    function rowColor($record)
    {
      $expire_date = sprintf("%d-%02d-%02d", $record["expire_date"]["year"], $record["expire_date"]["month"], $record["expire_date"]["day"]);
      $open = ($record["status"] == "new" || $record["status"] == "sent" || $record["status"] == "called");
      $days_left = $this->daysLeft(date("Y-m-d"), $expire_date);

      // If quotation has expired, and it's still new, sent or called, we get red..
      if ($days_left<=0 && $open)
      {
        return "#ff0000";
      }
      // Quotation expires in 5 days and is still open -> orange.
      else if ($days_left<=5 && $open)
      {
        return "#ffad63";
      }
      // do nothing (will result in the default color)
    }

    function initial_values()
    {
      $user = &getUser();
      $db = &atkGetDb();

      $query = "SELECT quotation_nr FROM quotation_quotation WHERE quotation_nr LIKE '".date("ymd")."%' ORDER BY quotation_nr DESC LIMIT 0,1";
      $rows = $db->getrows($query);


      $quo_nr = count($rows) ? strval(substr($rows[0]["quotation_nr"],0,6).(substr($rows[0]["quotation_nr"],6,strlen($rows[0]["quotation_nr"]))+1)) : date("ymd")."0";

      $stamp = mktime(12,0,0,date("m"), date("d")+35, date("Y"));
      $expiredate = array("day"=>date("d", $stamp),
                          "month"=>date("m", $stamp),
                          "year"=>date("Y", $stamp));

      return array("quotation_nr"=>$quo_nr, "owner"=>$user, "expire_date"=>$expiredate);
    }

    /**
     * RecordActions definitions container function
     *
     * This function contains the definitions of all custom actions to be offered on records by adding actionname=>actionurl pairs to the $actions array
     *
     * @param array $record The record for which the actions need to be determined.
     * @param array &$actions Reference to an array with the already defined actions. This is an associative array with the action identifier as key, and an url as value. Actions can be removed from it, or added to the array.
     * @param array &$mraactions List of multirecordactions that are supported forthe passed record.
     */
    function recordActions($record, &$actions, &$mraactions)
    {
      $states = array();

      if ($record["status"]!="billed"&&$record["status"]!="rejected")
      {
        // Only 'closed' quotations may be deleted.
        unset ($actions["delete"]);
      }
      switch ($record["status"])
      {
        case "new":
          $states[] = "sent";
          // geen break!
        case "sent":
          $states[] = "called";
          $states[] = "accepted";
          $states[] = "rejected";
          break;
        case "called":
          $states[] = "accepted";
          $states[] = "rejected";
          break;
        case "accepted":
          $states[] = "completed";
          $states[] = "prepaid";
          break;
        case "prepaid":
          $states[] = "prepaidcompleted";
        case "rejected":
          // nothing
          break;
        case "completed":
          $states[] = "billed";
          break;
        case "billed":
          // nothing
          break;
      }

      for($i=0;$i<count($states);$i++)
      {
        $actions["set_".$states[$i]] = dispatch_url("quotation.quotation", "setstate", array("status"=>$states[$i], "id"=>"[id]", "title"=>"[title]"));
      }

      $quotationdoc = atkconfig("doctemplatedir", "doctemplates/")."quotation/quotation/quotation.odt";
      if (file_exists($quotationdoc))
      {
        // Recordaction definition to call the document action handler without using the documentattribute.
        $templatefile = "quotation.odt";
        $actions["document"] = dispatch_url($this->atkNodeType(), "document", array("atkdoctpl"=>$templatefile, "atkselector"=>"[pk]"));
      }
    }

    function action_setstate()
    {
      $db = &atkGetDb();

      $id = $this->m_postvars["id"];

      $newstatus = $this->m_postvars["status"];
      if ($id!="" && $newstatus!="")
      {
        $db->query("UPDATE quotation_quotation SET status = '$newstatus' WHERE id = $id");
      }

      // get quotation number
      $record = array_pop($this->selectDb($this->m_table.".id='$id'"));
      $quotation_number = $record['quotation_nr'];

      if ($newstatus == "accepted" && atkConfig::get('quotation','quotation_addproject',true)) $this->redirect(session_url(dispatch_url("project.project", "add", array("name"=>$this->m_postvars["title"],"quotation_number"=>$quotation_number, "customer"=>array("id" => $record["organization_id"]["id"]))), SESSION_NESTED));
      else $this->redirect();
    }

    function adminFooter()
    {
      if ($this->m_view == "all")
      {
        return href(dispatch_url("quotation.quotation", $this->m_action, array("view"=>"filtered")), $this->text("hideclosed"));
      }
      else
      {
        return $this->text("archiveexplanation")." ".href(dispatch_url("quotation.quotation", $this->m_action, array("view"=>"all")), $this->text("showall"));
      }
    }

    function daysLeft($start, $end)
    {
      if ($start==""||$end=="") return 0;

      $d1 = mktime(12,0,0,substr($start,5,2), substr($start,8,2), substr($start,0,4));
      $d2 = mktime(12,0,0,substr($end,5,2), substr($end,8,2), substr($end,0,4));
      return floor(($d2-$d1)/(60*60*24));
    }

    function postAdd($record)
    {
      $this->updateProfitExpectance($record);
      return true;
    }

    function postUpdate($record)
    {
      $this->updateProfitExpectance($record);
      return true;
    }

    function updateProfitExpectance($record)
    {
      // calculate value
      $value = round($record['price'] * ($record['success_chance'] / 100),2);
      $query = "UPDATE `" . $this->m_table . "` SET `profit_expectance` = '$value' WHERE " . $record['atkprimkey'];
      $db = &atkGetDb();
      $db->query($query);
    }

    /**
     * Graph Pipeline function
     *
     * @param unknown_type $params
     * @param unknown_type $raw
     * @return unknown
     */
    function graph_cmpmypipeline($params, $raw=false)
    {

      $db = $this->getDb();
      $userid = atkGetUserId();
      if($params["startdate"]!="")
      {
        $starttime = strtotime($params["startdate"]);
        $startdate = $params["startdate"];

      }
      else
      {
        $starttime = time();
        $startdate = date("Y-m-d",$starttime);
      }
      if($params["enddate"]!="")
      {
        $enddate = $params["enddate"];
      }
      else
      {
        $enddate = date("Y-m-d",strtotime("+6 months",$starttime));
      }
      if($params["status"]!="")
      {
        $status=$params["status"];
      }
      else
      {
        $status="";
      }
      $sql = "SELECT SUM( price ) AS total, status
                              FROM quotation_quotation
                              WHERE expire_date BETWEEN '$startdate' AND '$enddate'
                                    ".($userid!=""?"AND assigned_to = '".$userid."' ":"");
      if(is_array($status))
      {
        $ids = array();
        foreach($status as $key=>$value)
        {
          $ids[]=$key;
        }

        $sql.=" AND status IN ('".implode("','",$ids)."') ";
      }
      $sql.="                 GROUP BY status";
      //var_dump($sql);
      $data = $db->getrows($sql);

      if ($raw)
      {
        return $data;
      }

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["status"]] = $data[$i]["total"];
      }

      return array(atktext("my_pipeline","quotation")=>$dataset);
    }
    
    function postDelete($record)
    {
      $eventlog = &atkGetNode("crm.eventlog");
      $eventlog->delete_event_history($this->atknodetype(),$record["atkprimkey"]);
      return true;
    }
  }

?>
