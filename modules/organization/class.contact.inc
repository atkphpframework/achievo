<?php

userelation("atkmanytoonerelation");
atkimport("modules.person.person");
useattrib("organization.contactprojectslist");
useattrib("atkcreatedbyattribute");
useattrib("atkCreateStampAttribute");
useattrib("atkupdatedbyattribute");
useattrib("atkupdatestampattribute");
useattrib("atkdocumentattribute");

class contact extends person
{
  function contact($name="contact")
  {
    $this->person($name);
    $listener = &atknew("modules.crm.listeners.eventlistener");
    $this->addListener($listener);
    $this->remove("userid");

    $this->addFilter("person.role='contact'");
    $this->add(new atkManyToOneRelation("company","organization.organization",AF_HIDE_ADD|AF_RELATION_AUTOLINK));
    $this->add(new atkAttribute("role"           , AF_OBLIGATORY|AF_READONLY|AF_HIDE, 15));
    $this->add(new contactprojectslist("projects", "active", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new contactprojectslist("archivedprojects", "archived", AF_HIDE_LIST|AF_HIDE_ADD));

    $this->add(new atkCreatedByAttribute("created_by",AF_HIDE_LIST),"history");
    $this->add(new atkCreateStampAttribute("created_on",AF_HIDE_LIST),"history");
    $this->add(new atkUpdatedByAttribute("last_modified_by",AF_HIDE_LIST),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST),"history");
    $this->add(new atkDocumentAttribute("document",AF_HIDE_LIST));
    $this->m_securityMap["document"] = "view";
  }

  function assignDocumentVars(&$documentWriter, $selector)
  {
    // Load the selected record from the database
    $contactrecord = $this->selectDb($selector);
    $documentWriter->assignDocumentSingleRecord($this, $contactrecord[0]);

    // Assign the referenced personcontact's organization (if available)
    $organizationnode = &atkGetNode("organization.organization");
    $organizationrecord = $organizationnode->selectDb("organization.id='".$contactrecord[0]["company"]["id"]."'", "", "", "", "", "view");
    if (count($organizationrecord) > 0)
    {
      $documentWriter->assignDocumentSingleRecord($organizationnode, $organizationrecord[0], "organization_");
    }

    // Use an evil global to let the tbsooo engine use the precence of an organization
    global $documentvar_organizationpresent;
    $documentvar_organizationpresent = (count($organizationrecord) > 0) ? 1 : 0;

    // Also assign the generic (date) vars tot the documentWriter
    $documentWriter->assignDocumentGenericVars();
  }

  function initial_values()
  {
    $initial["role"] = "contact";
    return $initial;
  }

  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }

  function postDelete($record)
  {
    $eventlog = &atkGetNode("crm.eventlog");
    $eventlog->delete_event_history($this->atknodetype(),$record["atkprimkey"]);
    return true;
  }
}

?>