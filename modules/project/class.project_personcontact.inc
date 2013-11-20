<?php

/**
 * Module Project
 * Class project_personcontact
 *
 * This class makes it possible to relate contacts to a project in a certain role(s).
 *
 * $Id$
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision$
 *
 */

userelation("atkmanytoonerelation");

class project_personcontact extends atkNode
{
  function project_personcontact()
  {
    $this->atkNode("project_personcontact");

    $this->add(new atkManyToOneRelation("projectid", "project.project", AF_OBLIGATORY|AF_PRIMARY|AF_HIDE));
    $this->add(new atkManyToOneRelation("personid", "project.select_contact", AF_OBLIGATORY|AF_PRIMARY|AF_RELATION_AUTOLINK|AF_LARGE));
    $this->add(new atkManyToOneRelation("role", "project.role", AF_OBLIGATORY|AF_PRIMARY|AF_RELATION_AUTOLINK));


    $this->addFilter("personid.role = 'contact'");

    // If you may edit the project, you may also add contactpersons to it.
    $this->setSecurityAlias("project.project");
    foreach(array("add", "save", "update", "delete") as $action)
      $this->m_securityMap[$action] = "edit";

    $this->setTable("project_person", "project_person");
    $this->setOrder("projectid.name ASC, personid.lastname ASC");
    $this->setDescriptorTemplate("[projectid.name] ([personid.lastname], [personid.firstname])");
  }

  function action_delete(&$handler)
  {
    // When deleting, the above filter won't work. But that doesn't matter, we
    // don't need it anyway.
    $this->m_flags |= NF_NO_FILTER;
    return $handler->action_delete();
  }

  function assignDocumentVars(&$documentWriter, $selector)
  {
    // Load the selected record from the database
    $project_personcontactrecord = $this->selectDb($selector);
    $documentWriter->assignDocumentSingleRecord($this, $project_personcontactrecord[0]);

    // Assign the referenced project
    $projectnode = &atkGetNode("project.project");
    $projectrecord = $projectnode->selectDb("project.id='".$project_personcontactrecord[0]["projectid"]["id"]."'");
    $documentWriter->assignDocumentSingleRecord($projectnode, $projectrecord[0], "project_", "", "", "", "", "view");

    // Assign the referenced personcontact
    $personcontactnode = &atkGetNode("project.select_contact");
    $personcontactrecord = $personcontactnode->selectDb("person.id='".$project_personcontactrecord[0]["personid"]["id"]."'");
    $documentWriter->assignDocumentSingleRecord($personcontactnode, $personcontactrecord[0], "person_", "", "", "", "", "view");

    // Assign the referenced personcontact's organization (if available)
    $organizationnode = &atkGetNode("organization.organization");
    $organizationrecord = $organizationnode->selectDb("organization.id='".$personcontactrecord[0]["company"]["id"]."'", "", "", "", "", "view");
    if (count($organizationrecord) > 0)
    {
      $documentWriter->assignDocumentSingleRecord($organizationnode, $organizationrecord[0], "organization_");
    }

    // Use an evil global to let the tbsooo engine use the precence of an organization
    global $documentvar_organizationpresent;
    $documentvar_organizationpresent = (count($organizationrecord) > 0) ? 1 : 0;

    // Assign the referenced coordinator
    $employeenode = &atkGetNode("employee.employee");
    $coordinatorrecord = $employeenode->selectDb("person.id='".$projectrecord[0]["coordinator"]["id"]."'", "", "", "", "", "view");
    $documentWriter->assignDocumentSingleRecord($employeenode, $coordinatorrecord[0], "coordinator_");

    // Also assign the generic (date) vars tot the documentWriter
    $documentWriter->assignDocumentGenericVars();
  }
}

?>