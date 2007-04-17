<?php

/**
 * Module Organization
 * Class organization
 *
 * This class makes it possible to add, edit and delete organizations.
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */


useattrib("atktextattribute");
useattrib("atkdummyattribute");
useattrib("atknumberattribute");
useattrib("atkparserattribute");
useattrib("atkdocumentattribute");
useattrib("atklistattribute");
userelation("atkonetomanyrelation");
userelation("atkmanytoonerelation");
useattrib("atkcreatedbyattribute");
useattrib("atkCreateStampAttribute");
useattrib("atkupdatedbyattribute");
useattrib("atkupdatestampattribute");
useattrib("atktabbedpane");

class organization extends atkNode
{
  function organization()
  {
    $this->atkNode("organization", NF_EDITAFTERADD|NF_EXPORT);
    $listener = &atknew("modules.crm.listeners.eventlistener");
    $this->addListener($listener);

    $this->add(new atkNumberAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkAttribute("organizationcode", AF_SEARCHABLE));
    $this->add(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY, 100));
    $this->add(new atkListAttribute("type", array("customer", "supplier"), "", AF_HIDE_LIST));
    $this->add(new atkDummyAttribute("visitheader", "<b>".atktext("organization_visitaddress", "organization")."</b>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));

    // TODO FIXME: in future version 1.1, the fields for address should be renamed to visit_*.
    // But in the 1.0 branch, we want to remain backwardscompatible.
    $this->add(new atkAttribute("address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("address2", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("zipcode", AF_HIDE_LIST|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("city", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("state", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("country", AF_HIDE_LIST|AF_HIDE_ADD, 100));

    $this->add(new atkDummyAttribute("mailheader", "<b>".atktext("organization_mailaddress", "organization")."</b>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));
    $this->add(new atkAttribute("mail_address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_address2", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_zipcode", AF_HIDE_LIST|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("mail_city", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_state", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_country", AF_HIDE_LIST|AF_HIDE_ADD, 100));

    $this->add(new atkDummyAttribute("invoiceheader", "<b>".atktext("organization_invoiceaddress", "organization")."</b>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));
    $this->add(new atkAttribute("invoice_address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("invoice_address2", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("invoice_zipcode", AF_HIDE_LIST|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("invoice_city", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("invoice_state", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("invoice_country", AF_HIDE_LIST|AF_HIDE_ADD, 100));

    $this->add(new atkDummyAttribute("divider", "<br>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));
    $this->add(new atkAttribute("phone",AF_SEARCHABLE|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("fax",AF_SEARCHABLE|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("email",AF_SEARCHABLE|AF_HIDE_ADD, 50));
    $this->add(new atkAttribute("website",AF_HIDE_ADD|AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("bankaccount", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkAttribute("bankaccount2", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkAttribute("bankaccount3", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkAttribute("bankaccount4", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkAttribute("vatnumber", AF_HIDE_LIST|AF_HIDE_ADD, 25));
    $this->add(new atkAttribute("debtornumber", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkManyToOneRelation("member_of","organization.organization",AF_HIDE_LIST|AF_LARGE|AF_HIDE_ADD));
    $this->add(new atkManyToOneRelation("status","crm.organization_status",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkManyToOneRelation("source","crm.source",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkManyToOneRelation("industry","crm.industry",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkNumberAttribute("employees",AF_HIDE_LIST|AF_HIDE_ADD,5));
    $this->add(new atkAttribute("revenue",AF_HIDE_LIST|AF_HIDE_ADD,5));
    $this->add(new atkManyToOneRelation("rate","crm.rate",AF_HIDE_LIST|AF_READONLY|AF_HIDE_ADD));
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee",AF_SEARCHABLE|AF_HIDE_ADD|AF_HIDE_LIST));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkOneToManyRelation("contactperson","organization.contact","company",AF_HIDE_LIST),"contacts");
    $this->add(new atkTabbedPane("project_subtabs",array("activeprojects"=>array("projects"),"archivedprojects"=>array("archivedprojects"),"contracts"=>array("contracts")),AF_HIDE_ADD|AF_NO_LABEL),"project");

    $this->add(new atkOneToManyRelation("contracts","organization.contracts","customer",AF_HIDE_LIST));
    $projectRelatie = new atkOneToManyRelation("projects", "project.project", "customer",AF_HIDE_LIST);
    $projectRelatie->addDestinationFilter("project.status='active'");
    $this->add($projectRelatie);
    $this->add(new atkParserAttribute("archivedprojects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD));
    //$this->add(new atkTabbedPane("extra_subtabs",array("quotations"=>array("quotations")),AF_HIDE_ADD|AF_NO_LABEL),"extra");
    $this->add(new atkOneToManyRelation("former_names","crm.former_name","account",AF_HIDE_LIST|AF_HIDE_ADD),"extra");
    $this->add(new atkOneToManyRelation("relation","crm.organization_relation","account",AF_HIDE_LIST|AF_HIDE_ADD),"extra");
    $this->add(new atkOneToManyRelation("quotations","quotation.quotation","organization_id",AF_HIDE_LIST),"extra");
    $this->add(new atkParserAttribute("vcard", href("?atkaction=vcard&id=[id]", atktext("vcard"), SESSION_NESTED), AF_NO_LABEL|AF_HIDE_ADD), NULL, 10000);

    $this->add(new atkDocumentAttribute("document",AF_HIDE_LIST));

    $this->add(new atkCreatedByAttribute("created_by",AF_HIDE_LIST),"history");
    $this->add(new atkCreateStampAttribute("created_on",AF_HIDE_LIST),"history");
    $this->add(new atkUpdatedByAttribute("last_modified_by",AF_HIDE_LIST),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST),"history");


    $this->m_securityMap["vcard"] = "view";
    $this->m_securityMap["export"] = "view";

    $this->setTable("organization", "organization");
    $this->setOrder("organization.name");
    $this->setIndex("name");
    $this->setDescriptorTemplate("[name], [city] ([organizationcode])");
  }

  /**
   * Determines the current view to be used (active or archive)
   *
   * @return string Current view mode ("all", "customer" or "supplier")
   */
  function getView()
  {
    // We use the session manager to retrieve the view.
    // this way, when paging, or when returning from an
    // edit screen etc., the system will have remembered
    // if we were viewing active records or the archive.
    $sessionManager = &atkGetSessionManager();
    $view = $sessionManager->pageVar("view");
    if (!in_array($view, array("all", "customer", "supplier")))
      $view = "all";
    return $view;
  }

  /**
   * Admin action handler override to implement archive and active view modes
   *
   * This function overrides the default adminHandlers action_admin, in
   * order to add our filtering logic to the admin action. This is done to
   * implement the archive and active view modes
   *
   * @param atkActionHandler &$handler Default admin actionhandler
   * @return atkAdminHandler Default admin action handler
   */
  function action_admin(&$handler)
  {
    // Add the filtering logic to the admin screen
    $view = $this->getView();
    if ($view != "all")
      $this->addFilter("organization.type='$view'");

    // Call the default adminHandler action_admin so the default action will be undertaken.
    return $handler->action_admin();
  }

  /**
   * Add html interface for switching to archive and active in user interface footer
   *
   * In this function we provide the user with a way to switch the view by
   * adding a link to the view the user is not in.
   *
   * @return string HTML Code fore the link to the other view
   */
  function adminHeader()
  {
    $result = "<br>" . $this->text("view"). ":";
    foreach (array("all", "customer", "supplier") as $filter)
      $result .= " " . href(dispatch_url("organization.organization", "admin", array("view"=>$filter)), $this->text($filter));
    $result .= "<br><br>";
    return $result;
  }

  function getExportAttributeGroup($attributename)
  {
    if (in_array($attributename, array("address", "address2", "zipcode", "city", "state", "country"))) return "organization_visitaddress";
    if (strpos($attributename, "mail_") === 0) return "organization_mailaddress";
    if (strpos($attributename, "invoice_") === 0) return "organization_invoiceaddress";
  }

  function action_export(&$handler)
  {
    // Some attributes are not usefull in exports
    foreach(array("contactperson", "contracts", "projects", "archivedprojects", "vcard", "document") as $attrname)
    {
      $attr = &$this->getAttribute($attrname);
      $attr->addFlag(AF_HIDE);
    }

    // Let the export handler do it's work
    return $handler->action_export();
  }

  function email_display($record, $mode)
  {
    $email = $record["email"];
    if (!in_array($mode, array("csv", "plain")) && (strlen($email)>0))
      $email = sprintf('<a href="mailto:%1$s">%1$s</a>', $email);
    return $email;
  }

   function website_display($record, $mode)
  {
    $website = $record["website"];
    if (!in_array($mode, array("csv", "plain")) && strlen($website)>0)
      $website = '<a href="http://'.$website.'" target="_blank">'.$website.'</a>';
    return $website;
  }

  function website_edit($record, $mode)
  {
     $attr = &$this->getAttribute("website");
     $org = $attr->edit($record, $mode);
     return "http://".$org;
  }

  function archivedprojects_edit($record)
  {
    return $this->showProjects($record,"relation","archived");
  }

  function archivedprojects_display($record)
  {
    return $this->showProjects($record,"view","archived");
  }

  function showProjects($record, $mode,$status="active")
  {
    $projectnode = &atkGetNode("project.project");
    $projectnode->m_fuzzyFilters=array("project.status='$status'");
    $recs = $projectnode->selectDb("customer = ".$record["id"], "", "", $projectnode->m_listExcludes);
    if (count($recs)>0)
    {
      $rl = &atknew("atk.recordlist.atkrecordlist");
      return $rl->render($projectnode, $recs, $projectnode->defaultActions($mode), RL_NO_SEARCH|RL_NO_SORT);
    }
    return atktext("none");
  }

  function action_vcard(&$handler)
  {
    return $this->createVCard($this->m_postvars["id"]);
  }

  function postDelete($record)
  {
    $eventlog = &atkGetNode("crm.eventlog");
    $eventlog->delete_event_history($this->atknodetype(),$record["atkprimkey"]);
    return true;
  }

  function createVCard($id)
  {
    include_once(moduleDir("utils")."vcard.php");
    $v = new vCard();

    $recs = $this->selectDb("id='".$id."'", "", "", "", array("phone", "fax", "name",
                                                              "address", "city", "state", "zipcode",
                                                              "country", "mail_address", "mail_city", "mail_state",
                                                              "mail_zipcode", "mail_country", "email", "website", "remark"));
    $record = $recs[0];

    $v->setPhoneNumber($record["phone"], "PREF;WORK;VOICE");
    $v->setPhoneNumber($record["fax"], "FAX");
    $v->setName($record["name"], "", "", "");
    $v->setAddress("", "", $record["address"], $record["city"], $record["state"], $record["zipcode"], $record["country"], "HOME");
    $v->setAddress("", "", $record["mail_address"], $record["mail_city"], $record["mail_state"], $record["mail_zipcode"], $record["mail_country"], "POSTAL");
    $v->setEmail($record["email"]);
    $v->setURL($record["website"]);
    $v->setNote($record["remark"]);

    $output = $v->getVCard();

    //$filename = $v->getFileName();
    //Remove spaces that would break the vcard filename
    $name = str_replace(" ", "_", $record['name'] );
    $filename = $name.".vcf";

    Header("Content-Disposition: attachment; filename=$filename");
    Header("Content-Length: ".strlen($output));
    Header("Connection: close");
    Header("Content-Type: text/x-vCard; name=$filename");

    echo $output;
    exit;
  }

}

?>
