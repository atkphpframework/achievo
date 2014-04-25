<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage crm
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atkListAttribute");
useattrib("atkTextAttribute");
useattrib("atkDateAttribute");
useattrib("atkEmailAttribute");
useattrib("atkaggregatedcolumn");
useattrib("atkdummyattribute");
userelation("atkManyToOneRelation");
useattrib("atkcreatedbyattribute");
useattrib("atkCreateStampAttribute");
useattrib("atkupdatedbyattribute");
useattrib("atkupdatestampattribute");

/**
 * Class for managing leads
 * 
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package achievo
 * @subpackage crm
 * @since 1.3.0
 */
class lead extends atkNode
{

    /**
     * Constructor
     *
     * @return void
     */
    function lead()
    {
        $this->atkNode("lead", NF_ADD_LINK);
        $listener = &atknew("modules.crm.listeners.eventlistener");
        $this->addListener($listener);
        $this->add(new atkAttribute("id", AF_AUTOKEY));

        $this->add(new atkManyToOneRelation("lead", "crm.source", AF_HIDE_LIST));
        $this->add(new atkTextAttribute("lead_description", 0, AF_HIDE_LIST));
        $this->add(new atkManyToOneRelation("campaign", "crm.campaign", AF_HIDE_LIST));

        $this->add(new atkListAttribute("status", array("new", "assigned", "inprocess", "converted", "dead"), array()));
        $this->add(new atkTextAttribute("status_description", 0, AF_HIDE_LIST));
        $this->add(new atkManyToOneRelation("title_id", "person.title", AF_RELATION_AUTOLINK | AF_HIDE_LIST));
        $this->add(new atkAttribute("lastname", AF_SEARCHABLE | AF_OBLIGATORY | AF_HIDE_LIST, 50));
        $this->add(new atkAttribute("firstname", AF_FORCE_LOAD | AF_HIDE_LIST, 50));
        $this->add(new atkAggregatedColumn("name", "[firstname] [lastname]", AF_SEARCHABLE, array("firstname", "lastname")));
        $this->add(new atkAttribute("initials", AF_FORCE_LOAD | AF_HIDE_LIST, 50));
        $this->add(new atkAttribute("phone", AF_SEARCHABLE, 20));
        $this->add(new atkAttribute("cellular", AF_HIDE_LIST | AF_FORCE_LOAD, 20));
        $this->add(new atkAttribute("fax", AF_HIDE_LIST | AF_FORCE_LOAD, 20));
        $this->add(new atkEmailAttribute("email", false, AF_SEARCHABLE, 50));

        $this->add(new atkDummyAttribute("visitheader", "<br><b>" . atktext("organization", "organization") . " " . atktext("organization_visitaddress", "organization") . "</b>", AF_HIDE_LIST | AF_HIDE_ADD | AF_NO_LABEL));
        $this->add(new atkAttribute("company", AF_SEARCHABLE));
        $this->add(new atkAttribute("address", AF_HIDE_LIST, 100));
        $this->add(new atkAttribute("address2", AF_HIDE_LIST | AF_HIDE_ADD, 100));
        $this->add(new atkAttribute("zipcode", AF_HIDE_LIST, 20));
        $this->add(new atkAttribute("city", AF_HIDE_LIST, 100));
        $this->add(new atkAttribute("state", AF_HIDE_LIST, 100));
        $this->add(new atkAttribute("country", AF_HIDE_LIST, 100));
        $this->add(new atkUrlAttribute("website", AF_SEARCHABLE | AF_URL_STRIPHTTP | AF_POPUP, 100));
        $this->add(new atkDummyAttribute("divider", "<br>", AF_HIDE_LIST | AF_HIDE_ADD | AF_NO_LABEL));

        $this->add(new atkManyToOneRelation("assigned_to", "employee.employee", AF_SEARCHABLE));
        $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST));

        $this->add(new atkCreatedByAttribute("created_by", AF_HIDE_LIST), "history");
        $this->add(new atkCreateStampAttribute("created_on", AF_HIDE_LIST), "history");
        $this->add(new atkUpdatedByAttribute("last_modified_by", AF_HIDE_LIST), "history");
        $this->add(new atkUpdateStampAttribute("last_modified_on", AF_HIDE_LIST), "history");

        $this->setOrder("crm_lead.lastname");
        $this->setTable("crm_lead");
        $this->setDescriptorTemplate('[name]');
    }

    /**
     * Add convert record action
     *
     * @param array $rec
     * @param array $actions
     * @param array $mraactions
     */
    function recordActions($rec, &$actions, &$mraactions)
    {
        if ($this->allowed("convert") && $rec["status"] != "converted")
            $actions["convert"] = session_url(dispatch_url($this->atknodetype(), "convert", array("atkselector" => "[pk]")));
    }

    /**
     * Override name display to make it clickable
     *
     * @param array $record
     * @return string
     */
    function name_display($record)
    {
        global $ATK_VARS;
        $action = $ATK_VARS["atkaction"];
        $name = $record["firstname"] . ($record["firstname"] != "" ? " " : "") . $record["lastname"];
        $viewUrl = session_url(dispatch_url($this->atknodetype(), "view", array("atkselector" => $this->primaryKey($record))), SESSION_NESTED);
        if (strlen($name) > 0 && ($action != "view" && $action != "select"))
            return '<a href="' . $viewUrl . '">' . $name . '</a>';
        return $name;
    }

    /**
     * Convert lead to a contact
     *
     * @param object $handler
     * @param array $record
     * @param string $mode
     */
    function action_convert(&$handler, $record = array(), $mode = "")
    {
        // Convert lead to contact/company
        $rows = $this->selectDb($this->m_postvars["atkselector"], "", "", "", "", "edit");
        if (count($rows) == 1) {
            $row = $rows[0];

            $organization = array("name" => $row["company"],
                "address" => $row["address"],
                "address2" => $row["address2"],
                "zipcode" => $row["zipcode"],
                "city" => $row["city"],
                "state" => $row["state"],
                "country" => $row["country"],
                "website" => $row["website"],
                "assigned_to" => $row["assigned_to"]);
            $org_node = &atkGetNode("organization.organization");
            $org_node->addDb($organization);
            $org_url = href(dispatch_url("organization.organization", "view", array("atkselector" => "organization.id=" . $organization["id"])), $row["company"], SESSION_REPLACE);

            $contact = array("title_id" => $row["title_id"],
                "lastname" => $row["lastname"],
                "firstname" => $row["firstname"],
                "initials" => $row["initials"],
                "phone" => $row["phone"],
                "cellular" => $row["cellular"],
                "fax" => $row["fax"],
                "email" => $row["email"],
                "assigned_to" => $row["assigned_to"],
                "remark" => $row["remark"],
                "role" => "contact",
                "company" => $organization["id"]);

            $contact_node = &atkGetNode("organization.contact");
            $contact_ret = $contact_node->addDb($contact);
            $contact_url = href(dispatch_url("organization.contact", "view", array("atkselector" => "person.id=" . $contact["id"])), $row["lastname"], SESSION_REPLACE);

            $row["status"] = "converted";
            $this->updateDb($row);

            $msg = atktext("created_new_contact", "crm") . ": " . $contact_url . "<br>";
            $msg.=atktext("created_new_organization") . ": " . $org_url . "<br>";
            $location = session_url(dispatch_url($this->atknodetype(), "feedback", array("atkfbaction" => "convert", "atkactionstatus" => ACTION_SUCCESS, "atkfbmessage" => $msg)), SESSION_REPLACE);
        } else {
            $location = $this->feedbackUrl("convert", ACTION_FAILED);
        }
        $this->redirect($location);
    }

    /**
     * When lead is deleted, delete it from the eventlog
     *
     * @param array $record
     * @return boolean
     */
    function postDelete($record)
    {
        $eventlog = &atkGetNode("crm.eventlog");
        $eventlog->delete_event_history($this->atknodetype(), $record["atkprimkey"]);
        return true;
    }

}

?>