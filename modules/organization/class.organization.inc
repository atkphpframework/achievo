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

class organization extends atkNode
{
  function organization()
  {
    $this->atkNode("organization", NF_EDITAFTERADD|NF_EXPORT);

    $this->add(new atkNumberAttribute("id" ,AF_AUTOKEY));
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
    $this->add(new atkAttribute("vatnumber", AF_HIDE_LIST|AF_HIDE_ADD, 25));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkOneToManyRelation("contactperson","organization.contact","company",AF_HIDE_LIST),"contacts");
    $this->add(new atkOneToManyRelation("contracts","organization.contracts","customer",AF_HIDE_LIST),"contracts");
    $this->add(new atkParserAttribute("projects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD),"active_projects");
    $this->add(new atkParserAttribute("archivedprojects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD),"archived_projects");


    $this->add(new atkParserAttribute("vcard", href("?atkaction=vcard&id=[id]", atktext("vcard"), SESSION_NESTED), AF_NO_LABEL|AF_HIDE_ADD), NULL, 10000);

    $this->add(new atkDocumentAttribute("document"));

    $this->m_securityMap["vcard"] = "view";
    $this->m_securityMap["export"] = "view";

    $this->setTable("organization", "organization");
    $this->setOrder("organization.name");
    $this->setIndex("name");

    atkdebug("organization::organization()");
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
    $view = $this->getView();
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

  function descriptor_def()
  {
    return "[name], [city]";
  }

  function archivedprojects_edit($record)
  {
    return $this->showProjects($record,"relation","archived");
  }

  function archivedprojects_display($record)
  {
    return $this->showProjects($record,"view","archived");
  }

  function projects_edit($record)
  {
    return $this->showProjects($record, "relation");
  }

  function projects_display($record)
  {
    return $this->showProjects($record, "view");
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