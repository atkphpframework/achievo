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
useattrib("atkparserattribute");
userelation("atkonetomanyrelation");

class organization extends atkNode
{
  function organization()
  {
    $this->atkNode("organization", NF_EDITAFTERADD);

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY, 100));
    $this->add(new atkAttribute("address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("zipcode", AF_HIDE_LIST|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("city", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("state", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("country", AF_HIDE_LIST|AF_HIDE_ADD, 100));
    $this->add(new atkAttribute("phone",AF_SEARCHABLE|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("fax",AF_SEARCHABLE|AF_HIDE_ADD, 20));
    $this->add(new atkAttribute("email",AF_SEARCHABLE|AF_HIDE_ADD, 50));
    $this->add(new atkAttribute("website",AF_HIDE_ADD|AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("bankaccount", AF_HIDE_LIST|AF_HIDE_ADD, 30));
    $this->add(new atkTextAttribute("remark", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkOneToManyRelation("contactperson","organization.contact","company",AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("contracts","organization.contracts","customer",AF_HIDE_LIST));
    //$this->addAttribute(new atkOneToManyRelation("projects","project.project","customer",AF_HIDE_LIST));
    $this->add(new atkParserAttribute("projects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD));
    
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
    $recs = $projectnode->selectDb();
    if (count($recs)>0)
    {
      return $projectnode->normalRecordList($recs, $projectnode->defaultActions($mode), RL_NO_SEARCH|RL_NO_SORT);
    }
    return text("none");
  }
  
}
?>
