<?php

  /**
   * Relation has the same functionality as the normal manytoonerelation,
   * but the attribute itself is not stored in the node.
   *
   * @package modules
   * @subpackage timereg
   *
   * @author dennis <dennis@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  userelation("atkmanytoonerelation");

  class projectManyToOneRelation extends atkManyToOneRelation
  {
    var $m_store = false;
    var $m_userid = "";

    function projectManyToOneRelation($name, $destination, $flags=0)
    {
      $this->m_userid = &atkGetUserId();
      if (atkConfig("project_selection_autocomplete", false))
        $flags |= AF_LARGE;
      parent::atkManyToOneRelation($name, $destination,$flags);
    }

    function setUserId($userid)
    {
      $this->m_userid = $userid;
    }

    function setStore($store=false)
    {
      $this->m_store = $store;
    }

    function storageType($mode)
    {
      if($this->m_store == false)
        return NOSTORE;
      else
        return parent::storageType($mode);
    }

    function edit(&$record, $fieldprefix="", $mode="")
    {
      $filter=$this->parseFilter($this->m_destinationFilter,$record);
      $newsel = $this->getHtmlId($fieldprefix);

      $selectionlink = "";
      if(!$this->hasFlag(AF_RELATION_AUTOCOMPLETE))
      {
        $selectionlink = "&nbsp;".$this->createSelectionLink($newsel,$filter);
      }

      return parent::edit($record,$fieldprefix,$mode).$selectionlink;
    }

    /**
     * Creates a link to the selection page of the projectnode.
     *
     */
    function createSelectionLink($id,$filter)
    {
      return $this->_getSelectLink($id, $filter);
    }


    /**
     * When the attribute is used as a non-storing attribute, we need to
     * set the join-condition with the phase manually.
     * otherwise, we can use the normal join condition.
     *
     * @param object $query
     * @param String $tablename
     * @param String $fieldalias
     * @return String join condition
     */
    function getJoinCondition(&$query, $tablename="",$fieldalias="")
    {
      if($this->m_store == false)
        return "$fieldalias.id=phaseid.projectid";

      return parent::getJoinCondition($query,$tablename,$fieldalias);
    }

    function getSelectableRecords($record=array(), $mode="",$force=false)
    {
      $newsel = $this->getHtmlId("");
      if(!$this->hasFlag(AF_RELATION_AUTOCOMPLETE))
      {
        //Check if the user has selected another project
        $selectedprojectid = 0;
        if(is_object($this->m_ownerInstance)) //do we have an ownerInstance
        {
          $selectedprojectid = $this->m_ownerInstance->m_postvars[$newsel];
        }

        $projectids = $this->getRecentProjectsIds($selectedprojectid);
        if (!empty($record[$this->fieldName()]["id"]) && !in_array((int)$record[$this->fieldName()]["id"], $projectids))
          $projectids[] = (int)$record[$this->fieldName()]["id"];

        if(count($projectids))
          $filter = "project.id IN (".implode(",",$projectids).")";
        else
          $filter = "project.id IN (NULL)";

        $this->setDestinationFilter($filter);
      }

      return parent::getSelectableRecords($record, $mode, $force);
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
    function getRecentProjectsIds($selectedprojectid=0)
    {
      global $g_securityManager;

      $user = &getUser();

      /* @var $query atkQuery */
      /* @var $db atkDb */
      $db = &atkGetDb();
      $query = $db->createQuery();

      $query->addTable("hours");

      // Magic Peter Code (tm) to get around a mysql3 limitation
      // that doesn't allow us to use subqueries, which would normally
      // be needed to solve this particular set of distinct/order by
      // requirements.
      $query->addField("project.id");
      $query->addJoin("phase","","hours.phaseid=phase.id",false);
      $query->addJoin("project","","phase.projectid=project.id",false);
      $query->addCondition("hours.userid='{$this->m_userid}'");
      $query->addCondition("project.status='active'");

      if (!$g_securityManager->allowed("timereg.hours", "any_project"))
      {
        $query->addJoin("project_person","members","members.projectid = project.id",false);
        $query->addGroupBy("project.id");
        $query->addCondition("(members.personid = ".$this->m_userid." OR project.timereg_limit = ".PRJ_TIMEREG_ALL_USERS.")");
      }

      $query->addOrderBy("hours.activitydate DESC");

      $arr = $db->getrows($query->buildSelect(false),'0',atkconfig("numberofrecentprojects")+100);

      $newsel = $this->getHtmlId("");
      $newarr = array();

      //we add the selected project id on top.
      if(($selectedprojectid > 0) && (is_numeric($selectedprojectid)))
        $newarr[] = $selectedprojectid;
      else
      {
        $values = decodeKeyValuePair($selectedprojectid);
        $value = $values[$this->m_destInstance->m_table.".".$this->m_destInstance->m_primaryKey[0]];
        if(is_numeric($value) && $value!=0)
        $newarr[] = $value;
      }

      // remove dupes because we can't distinct/group by and order by recency in mysql3
      for ($i=0,$_i=count($arr);($i<$_i)&&count($newarr)<atkconfig("numberofrecentprojects");$i++)
      {
      	if (!in_array($arr[$i]["id"], $newarr))
      	{
      		$newarr[] = (int) $arr[$i]['id'];
      	}
      }

      return $newarr;
    }


  }
?>
