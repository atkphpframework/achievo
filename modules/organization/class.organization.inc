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
useattrib("atkdummyattribute");
userelation("atkonetomanyrelation");

class customerFilteredProjectAttrib extends atkDummyAttribute
{
  var $m_customerid = NULL;
  
  function customerFilteredProjectAttrib($customerid)
  {
    $this->atkDummyAttribute("custfilter", "", AF_HIDE);
    $this->m_customerid = $customerid;
  }
 
  function addToQuery(&$query, $table="", $aliasprefix="", $rec="", $level, $mode)
  {
    if ($this->m_customerid != NULL)
    {
      $query->addJoin("project_person", "projectmembers", "project.id = projectmembers.projectid", false);
      $query->addJoin("person", "person", "person.id = projectmembers.personid AND person.company='".$this->m_customerid."'", false);      
      $query->setDistinct(true); // the join we perform could lead to duplicate records.
    }
  }
}

class organization extends atkNode
{
  function organization()
  {
    $this->atkNode("organization", NF_EDITAFTERADD);

    $this->addAttribute(new atkAttribute("id" ,AF_HIDE|AF_AUTO_INCREMENT|AF_PRIMARY));
    $this->addAttribute(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY));
    $this->addAttribute(new atkAttribute("address", AF_SEARCHABLE|AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("zipcode", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("city", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("country", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("phone",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("fax",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("email",AF_SEARCHABLE|AF_HIDE_ADD));
    $this->addAttribute(new atkAttribute("website",AF_HIDE_ADD|AF_HIDE_LIST));
    $this->addAttribute(new atkAttribute("bankaccount", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkTextAttribute("remark", AF_HIDE_LIST|AF_HIDE_ADD));
    $this->addAttribute(new atkOneToManyRelation("contactperson","organization.contact","company",AF_HIDE_LIST));
    $this->addAttribute(new atkOneToManyRelation("contracts","organization.contracts","customer",AF_HIDE_LIST));
    //$this->addAttribute(new atkOneToManyRelation("projects","project.project","customer",AF_HIDE_LIST));
    $this->addAttribute(new atkParserAttribute("projects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD));
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
    $projectnode->add(new customerFilteredProjectAttrib($record["id"]));
    $recs = $projectnode->selectDb();
    if (count($recs)>0)
    {
      return $projectnode->normalRecordList($recs, $projectnode->defaultActions($mode), RL_NO_SEARCH|RL_NO_SORT);
    }
    return text("none");
  }
  
}
?>
