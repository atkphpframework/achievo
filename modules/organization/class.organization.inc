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
useattrib("atkparserattribute");
userelation("atkonetomanyrelation");

class organization extends atkNode
{
  function organization()
  {
    $this->atkNode("organization", NF_EDITAFTERADD);

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY, 100));    
    $this->add(new atkDummyAttribute("visitheader", "<B>".text("visitaddress", "organization")."<B>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));
    
    // TODO FIXME: in future version 1.1, the fields for address should be renamed to visit_*.
    // But in the 1.0 branch, we want to remain backwardscompatible.
    $this->add(new atkAttribute("address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("address2", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("zipcode", AF_HIDE_LIST|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("city", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("state", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("country", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    
    $this->add(new atkDummyAttribute("mailheader", "<B>".text("mailaddress", "organization")."<B>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));
    $this->add(new atkAttribute("mail_address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_address2", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_zipcode", AF_HIDE_LIST|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("mail_city", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_state", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("mail_country", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    
    $this->add(new atkDummyAttribute("divider", "<Br>", AF_HIDE_LIST|AF_HIDE_ADD|AF_NO_LABEL));
    $this->add(new atkAttribute("phone",AF_SEARCHABLE|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("fax",AF_SEARCHABLE|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("email",AF_SEARCHABLE|AF_HIDE_ADD, 50));
    $this->add(new atkAttribute("website",AF_HIDE_ADD|AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("bankaccount", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkAttribute("vatnumber", AF_HIDE_LIST|AF_HIDE_ADD, 25));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkOneToManyRelation("contactperson","organization.contact","company",AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("contracts","organization.contracts","customer",AF_HIDE_LIST));
    $this->add(new atkParserAttribute("projects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkParserAttribute("vcard", href("?atkaction=vcard&id=[id]", text("vcard"), SESSION_NESTED), AF_NO_LABEL|AF_HIDE_LIST), NULL, 10000);
    $this->m_securityMap["vcard"] = "view";
    
    $this->setTable("organization", "organization");
    $this->setOrder("organization.name");
    $this->setIndex("name");

    atkdebug("organization::organization()");
  }

  function email_display($record)
  {
    $email = $record["email"];
    if (strlen($email)>0) return '<a href="mailto:'.$email.'">'.$email.'</a>';
    return "";
  }

  function descriptor_def()
  {
    return "[name], [city]";
  }
  
  function projects_edit($record)
  {
    return $this->showProjects($record, "relation");
  }
  
  function projects_display($record)
  {
    return $this->showProjects($record, "view");
  }
  
  function showProjects($record, $mode)
  {
    $projectnode = getNode("project.project");
    useattrib("project.filterprojectbyorganizationattrib");
    $projectnode->add(new filterProjectByOrganizationAttrib($record["id"]));
    $recs = $projectnode->selectDb("", "", "", $projectnode->m_listExcludes);
    if (count($recs)>0)
    {
      return $projectnode->normalRecordList($recs, $projectnode->defaultActions($mode), RL_NO_SEARCH|RL_NO_SORT);
    }
    return text("none");
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
    $filename = $v->getFileName();

    Header("Content-Disposition: attachment; filename=$filename");
    Header("Content-Length: ".strlen($output));
    Header("Connection: close");
    Header("Content-Type: text/x-vCard; name=$filename");

    echo $output; 
    exit;
  }
  
}

?>
