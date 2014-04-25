<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage project
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
userelation("atkmanytoonerelation");

/**
 * Relation has the same functionality as the normal manytoonerelation,
 * but the attribute itself is not stored in the node.
 *
 * @package modules
 * @subpackage project
 *
 * @author dennis <dennis@ibuildings.nl>
 */
class projectManyToOneRelation extends atkManyToOneRelation
{
    /**
     * Store or not
     *
     * @var boolean
     */
    var $m_store = false;

    /**
     * User id
     *
     * @var int
     */
    var $m_userid = "";

    /**
     * Constructor
     *
     * @param string $name Name
     * @param string $destination Destination
     * @param int $flags Flags
     */
    function projectManyToOneRelation($name, $destination, $flags = 0)
    {
        $this->m_userid = &atkGetUserId();
        if (atkConfig::get("project", "project_selection_autocomplete", false))
            $flags |= AF_LARGE;
        parent::atkManyToOneRelation($name, $destination, $flags);
    }

    /**
     * Set Userid
     *
     * @param int $userid User id
     */
    function setUserId($userid)
    {
        $this->m_userid = $userid;
    }

    /**
     * Set store 
     *
     * @param boolean $store Store or not
     */
    function setStore($store = false)
    {
        $this->m_store = $store;
    }

    /**
     * Storage type
     *
     * @param string $mode Mode
     * @return int
     */
    function storageType($mode)
    {
        if ($this->m_store == false)
            return NOSTORE;
        else
            return parent::storageType($mode);
    }

    /**
     * Database fieldtype
     *
     * @return string
     */
    function dbFieldType()
    {
        if ($this->m_store == false)
            return "";
        else
            return parent::dbFieldType();
    }

    /**
     * Override edit function
     *
     * @param array $record Record
     * @param string $fieldprefix Fieldprefix
     * @param string $mode Mode
     * @return string
     */
    function edit(&$record, $fieldprefix = "", $mode = "")
    {
        $filter = $this->parseFilter($this->m_destinationFilter, $record);
        $newsel = $this->getHtmlId($fieldprefix);

        $selectionlink = "";
        if (!$this->hasFlag(AF_RELATION_AUTOCOMPLETE)) {
            $selectionlink = "&nbsp;" . $this->createSelectionLink($newsel, $filter);
        }

        return parent::edit($record, $fieldprefix, $mode) . $selectionlink;
    }

    /**
     * Creates a link to the selection page of the projectnode.
     * @param int $id Id
     * @param string $filter Filter
     *
     */
    function createSelectionLink($id, $filter)
    {
        return $this->_getSelectLink($id, $filter);
    }

    /**
     * When the attribute is used as a non-storing attribute, we need to
     * set the join-condition with the phase manually.
     * otherwise, we can use the normal join condition.
     *
     * @param atkquery $query
     * @param String $tablename
     * @param String $fieldalias
     * @return String join condition
     */
    function getJoinCondition(&$query, $tablename = "", $fieldalias = "")
    {
        if ($this->m_store == false)
            return "$fieldalias.id=phaseid.projectid";

        return parent::getJoinCondition($query, $tablename, $fieldalias);
    }

    /**
     * Get selectable records
     *
     * @param array $record Record
     * @param string $mode Mode
     * @param boolean $force Force
     * @return array
     */
    function getSelectableRecords($record = array(), $mode = "", $force = false)
    {
        $newsel = $this->getHtmlId("");
        if (!$this->hasFlag(AF_RELATION_AUTOCOMPLETE)) {
            //Check if the user has selected another project
            $selectedprojectid = 0;
            if (is_object($this->m_ownerInstance)) { //do we have an ownerInstance
                $selectedprojectid = $this->m_ownerInstance->m_postvars[$newsel];
            }

            $projectids = $this->getRecentProjectsIds($selectedprojectid);

            if (!empty($record[$this->fieldName()]["id"]))
                $this->_addProject($record[$this->fieldName()]["id"], $projectids);
        }

        return $projectids;
    }

    /**
     * Function returns an array with the project-ids, most recently
     * registered hours on. Number of recent projects is configurable
     * with the 'config_numberofrecentprojects'.
     *
     * This function also adds the selected project that was returned
     * after selecting a project on the select page of projects.
     *
     * @param int $selectedprojectid The selected projectid
     * @return array Project ids
     */
    function getRecentProjectsIds($selectedprojectid = 0)
    {
        /* @var $securityManager atkSecurityManager */
        $securityManager = &atkGetSecurityManager();

        /* @var $query atkQuery */
        /* @var $db atkDb */
        $db = &atkGetDb();

        /* Subquery to retrieve the most recently booked times / phases */
        $subquery = $db->createQuery();
        $subquery->addTable("hoursbase");
        $subquery->addField("phaseid");
        $subquery->addField("id");
        $subquery->addCondition("hoursbase.userid='" . $db->escapeSQL($this->m_userid) . "'");
        $subquery->addOrderBy("id DESC");
        $subquery->setLimit(0, atkconfig::get("project", "numberofrecentprojects") * 20);

        /* Main query to retrieve the most recently booked projects */
        $query = $db->createQuery();

        $query->addField("project.id");
        $query->addField("project.name");
        $query->addField("project.abbreviation");

        $query->addTable("phase");
        $query->addJoin("project", "", "phase.projectid=project.id", false);
        $query->addJoin("(" . $subquery->buildSelect() . ")", "hours", "phase.id=hours.phaseid", false);

        $query->addCondition("project.status='active'");

        if (!$securityManager->allowed("timereg.hours", "any_project")) {
            $query->addJoin("project_person", "", "project_person.projectid = project.id", true);
            $query->addCondition("(project_person.personid = " . $db->escapeSQL($this->m_userid) . " OR project.timereg_limit = " . PRJ_TIMEREG_ALL_USERS . ")");
        }

        $query->addOrderBy("hours.id DESC");
        $query->setLimit(0, atkconfig::get("project", "numberofrecentprojects"));

        $arr = $db->getrows($query->buildSelect(true));

        //we add the selected project id on top.
        if (!is_numeric($selectedprojectid)) {
            $values = decodeKeyValuePair($selectedprojectid);
            $value = $values[$this->m_destInstance->m_table . "." . $this->m_destInstance->m_primaryKey[0]];
            if (is_numeric($value) && $value != 0)
                $selectedprojectid = $value;
        }
        $this->_addProject($selectedprojectid, $arr);
        return $arr;
    }

    /**
     * Add project
     *
     * @access private
     * @param int $id Project id
     * @param array $arr Projects array
     */
    function _addProject($id, &$arr)
    {
        if ($id > 0) {
            $flag = true;
            foreach ($arr as $key => $value) {
                if ($value['id'] == $id) {
                    $flag = false;
                    break;
                }
            }

            if ($flag) {
                $db = &atkGetDb();
                $query = $db->createQuery();
                $query->addTable("project");
                $query->addField("project.id");
                $query->addField("project.name");
                $query->addField("project.abbreviation");
                $query->addCondition("id='$id'");

                $result = $db->getrows($query->buildSelect());
                $arr = array_merge($result, $arr);
            }
        }
    }

}

?>
